#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "ActorFactory.h"
#include "Actor.h"
#include "ActorID.h"
#include "ActorComponent.h"
#include "GeneralRenderComponent.h"
#include "FiniteTimeActionComponent.h"

#include "../Script/WorldScript.h"
#include "../Utilities/GenericFactory.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActorFactory::ActorFactoryImpl.
//////////////////////////////////////////////////////////////////////////
struct ActorFactory::ActorFactoryImpl
{
	ActorFactoryImpl();
	~ActorFactoryImpl();

	//This function is called in the constructor of impl.
	//You must modify this function whenever the types of components are changed.
	void registerComponents();

	std::shared_ptr<ActorComponent> createComponent(tinyxml2::XMLElement * componentElement);
	void postInitActor(std::shared_ptr<Actor> & actor);

	ActorID getNextID() const;
	void updateID();

	ActorID m_currentID{ INVALID_ACTOR_ID };
	GenericFactory<ActorComponent> m_ComponentFactory;
};

ActorFactory::ActorFactoryImpl::ActorFactoryImpl()
{
	registerComponents();
}

ActorFactory::ActorFactoryImpl::~ActorFactoryImpl()
{
}

void ActorFactory::ActorFactoryImpl::registerComponents()
{
	//#TODO: Register all of the concrete components here.
	m_ComponentFactory.registerType<GeneralRenderComponent>();
	m_ComponentFactory.registerType<FiniteTimeActionComponent>();

	m_ComponentFactory.registerType<WorldScript>();
}

std::shared_ptr<ActorComponent> ActorFactory::ActorFactoryImpl::createComponent(tinyxml2::XMLElement * componentElement)
{
	auto componentType = componentElement->Value();
	auto component = m_ComponentFactory.createShared(componentType);

	//If can't create the component, log and return nullptr.
	if (!component){
		cocos2d::log("ActorFactoryImpl::createComponent couldn't find an ActorComponent named %s", componentType);
		return nullptr;
	}

	//Try to initialize the component. If it fails, log and return nullptr.
	if (!component->vInit(componentElement)){
		cocos2d::log("ActorFactoryImpl::createComponent failed to initialize a(n) %s", componentType);
		return nullptr;
	}

	//The component is created and initialized successfully.
	return component;
}

void ActorFactory::ActorFactoryImpl::postInitActor(std::shared_ptr<Actor> & actor)
{
	//If you want to post-init the components in some order, do it here.
	actor->postInit();
}

ActorID ActorFactory::ActorFactoryImpl::getNextID() const
{
	return m_currentID + 1;
}

void ActorFactory::ActorFactoryImpl::updateID()
{
	++m_currentID;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ActorFactory.
//////////////////////////////////////////////////////////////////////////
ActorFactory::ActorFactory() : pimpl{ std::make_unique<ActorFactoryImpl>() }
{
}

ActorFactory::~ActorFactory()
{
}

std::vector<std::shared_ptr<Actor>> ActorFactory::createActorAndChildren(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
{
	//Load the resource file. If failed, log and return an empty vector.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(resourceFile);
	const auto rootElement = xmlDoc.RootElement();
	if (!rootElement){
		cocos2d::log("ActorFactory::createActor failed to load resource file %s", resourceFile);
		return{};
	}

	//Create and init the parent actor. If failed, log and return an empty vector.
	auto parentActor = std::make_shared<Actor>();
	if (!parentActor || !parentActor->init(pimpl->getNextID(), rootElement)){
		cocos2d::log("ActorFactory::createActor failed to create or init an actor.");
		return{};
	}

	//Update the m_NextID in case that the components tries to create an actor, invalidating the id.
	pimpl->updateID();

	//Loop through each component element, create components and attach them to actor.
	if (auto componentsElement = rootElement->FirstChildElement("Components")){
		for (auto componentElement = componentsElement->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()){
			auto component = pimpl->createComponent(componentElement);

			//If failed to create a component, return an empty vector because the actor will be partially complete and may cause more troubles than goods.
			if (!component)
				return{};

			//The component is created successfully so add it to the actor.
			component->setOwner(parentActor);
			parentActor->addComponent(std::move(component));
		}
	}

	//The parent actor is created completely. Place it in the returning vector.
	auto actorVector = std::vector<std::shared_ptr<Actor>>{parentActor};

	//Loop through each child actor element, create children actors and attach them to the parent actor.
	if (auto childrenElement = rootElement->FirstChildElement("ChildrenActors")){
		for (auto childElement = childrenElement->FirstChildElement("Resource"); childElement; childElement = childElement->NextSiblingElement()){
			//Create the child actors according to 'childElement'. If failed, skip it.
			auto childActors = createActorAndChildren(childElement->Attribute("File"));
			if (childActors.empty())
				continue;

			//The child actors is created. Attach the first one to the parent actor, and add child actors to the returning vector.
			parentActor->addChild(*childActors[0]);
			for (auto && childActor : childActors)
				actorVector.emplace_back(std::move(childActor));
		}
	}

	//Modify the actor using data in the overrides and then post-init it.
	modifyActor(parentActor, overrides);
	pimpl->postInitActor(parentActor);

	return actorVector;
}

void ActorFactory::modifyActor(const std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides)
{
	if (!actor || !overrides)
		return;

	// Loop through each child element and load the component
	if (auto componentsElement = overrides->FirstChildElement("Components")){
		for (auto componentElement = componentsElement->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()){
			auto & component = actor->getComponent(componentElement->Value());

			//If there is a component of the same type already, re-initialize it.
			if (component){
				component->vInit(componentElement);
				component->vOnChanged();
			}
			else {
				//Else, create a new component and attach to actor.
				auto newComponent = pimpl->createComponent(componentElement);
				if (newComponent){
					component->setOwner(actor);
					actor->addComponent(std::move(newComponent));
				}
			}
		}
	}
}
