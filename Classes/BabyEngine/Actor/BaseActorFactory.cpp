#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "BaseActorFactory.h"
#include "Actor.h"
#include "ActorComponent.h"

#include "../Utilities/GenericFactory.h"
#include "FiniteTimeActionComponent.h"
#include "GeneralRenderComponent.h"
#include "SceneRenderComponent.h"
#include "ActionComponent.h"
#include "TransformComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActorFactory::BaseActorFactoryImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseActorFactory::BaseActorFactoryImpl
{
	BaseActorFactoryImpl() = default;
	~BaseActorFactoryImpl() = default;

	std::shared_ptr<Actor> createSingleActorWithComponents(const tinyxml2::XMLElement * actorElement);
	bool addChildrenActors(Actor & parent, std::vector<std::shared_ptr<Actor>> & parentActors, std::vector<std::shared_ptr<Actor>> && childrenActors) const;

	std::shared_ptr<Actor> _createEmptyActor(const tinyxml2::XMLElement * actorElement);
	bool _createAndAttachAllComponentsToActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentsElement) const;
	bool __createAndAttachOneComponentToActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentElement) const;
	std::shared_ptr<ActorComponent> ___createComponent(const tinyxml2::XMLElement * componentElement) const;

	void modifyActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * overrides) const;
	void _modifyComponentsForActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentsElement) const;
	void postInitActor(Actor & actor);

	ActorID getAvaliableActorID() const;
	void updateActorID();

	ActorID m_currentID{ INVALID_ACTOR_ID };
	GenericFactory<ActorComponent> m_ComponentFactory;
};

std::shared_ptr<Actor> BaseActorFactory::BaseActorFactoryImpl::createSingleActorWithComponents(const tinyxml2::XMLElement * actorElement)
{
	assert(actorElement && "BaseActorFactoryImpl::createSingleActorWithComponents() the actor element is nullptr.");

	auto actor = _createEmptyActor(actorElement);
	if (!actor) {
		return nullptr;
	}

	if (!_createAndAttachAllComponentsToActor(actor, actorElement->FirstChildElement("Components"))) {
		return nullptr;
	}

	return actor;
}

bool BaseActorFactory::BaseActorFactoryImpl::addChildrenActors(Actor & parent, std::vector<std::shared_ptr<Actor>> & parentActors, std::vector<std::shared_ptr<Actor>> && childrenActors) const
{
	if (childrenActors.empty()) {
		return false;
	}

	parent.addChild(*childrenActors[0]);
	for (auto && childActor : std::move(childrenActors)) {
		parentActors.emplace_back(std::move(childActor));
	}

	return true;
}

std::shared_ptr<Actor> BaseActorFactory::BaseActorFactoryImpl::_createEmptyActor(const tinyxml2::XMLElement * actorElement)
{
	auto actor = std::make_shared<Actor>();
	if (!actor || !actor->init(getAvaliableActorID(), actor, actorElement)) {
		cocos2d::log("BaseActorFactoryImpl::createSingleActorWithComponents() failed to create or init the actor itself (not the components).");
		return nullptr;
	}

	updateActorID();
	return actor;
}

bool BaseActorFactory::BaseActorFactoryImpl::_createAndAttachAllComponentsToActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentsElement) const
{
	assert(actor && "BaseActorFactoryImpl::_createAndAttachAllComponentsToActor() the actor is nullptr.");

	if (!componentsElement) {
		cocos2d::log("BaseActorFactoryImpl::_createAndAttachAllComponentsToActor() the components element is nullptr, possibly because it's not specified in the actor xml.");
		return true;
	}

	for (auto singleComponentElement = componentsElement->FirstChildElement(); singleComponentElement; singleComponentElement = singleComponentElement->NextSiblingElement()) {
		if (!__createAndAttachOneComponentToActor(actor, singleComponentElement)) {
			return false;
		}
	}

	return true;
}

bool BaseActorFactory::BaseActorFactoryImpl::__createAndAttachOneComponentToActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentElement) const
{
	assert(actor && "BaseActorFactoryImpl::__createAndAttachOneComponentToActor() the actor is nullptr.");
	assert(componentElement && "BaseActorFactoryImpl::__createAndAttachOneComponentToActor() the component element is nullptr.");

	if (auto component = ___createComponent(componentElement)) {
		component->setOwner(actor);
		actor->addComponent(std::move(component));

		return true;
	}

	return false;
}

std::shared_ptr<ActorComponent> BaseActorFactory::BaseActorFactoryImpl::___createComponent(const tinyxml2::XMLElement * componentElement) const
{
	auto componentType = componentElement->Value();
	auto component = m_ComponentFactory.createShared(componentType);

	assert(component && "BaseActorFactoryImpl::___createComponent() can't find or create the ActorComponent as the xml indicated, possibly because the component is not registered to the factory.");

	if (!component->vInit(componentElement)) {
		cocos2d::log("BaseActorFactoryImpl::___createComponent failed to initialize a(n) %s", componentType);
		return nullptr;
	}

	return component;
}

void BaseActorFactory::BaseActorFactoryImpl::modifyActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * overrides) const
{
	if (!overrides) {
		return;
	}

	assert(actor && "BaseActorFactoryImpl::modifyActor() the actor is nullptr.");
	_modifyComponentsForActor(actor, overrides->FirstChildElement("Components"));
}

void BaseActorFactory::BaseActorFactoryImpl::_modifyComponentsForActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement * componentsElement) const
{
	if (!actor || !componentsElement) {
		return;
	}

	// Loop through each child element and load the component
	for (auto componentElement = componentsElement->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()) {
		if (auto existingComponent = actor->getComponent(componentElement->Value())) {
			//If there is a component of the same type already, re-initialize it.
			existingComponent->vInit(componentElement);
			existingComponent->vOnChanged();
		}
		else {
			//Else, create a new component and attach to actor.
			__createAndAttachOneComponentToActor(actor, componentElement);
		}
	}
}

void BaseActorFactory::BaseActorFactoryImpl::postInitActor(Actor & actor)
{
	//If you want to post-init the components in some order, do it here.
	actor.postInit();
}

ActorID BaseActorFactory::BaseActorFactoryImpl::getAvaliableActorID() const
{
	return m_currentID + 1;
}

void BaseActorFactory::BaseActorFactoryImpl::updateActorID()
{
	++m_currentID;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ActorFactory.
//////////////////////////////////////////////////////////////////////////
BaseActorFactory::BaseActorFactory() : pimpl{ std::make_unique<BaseActorFactoryImpl>() }
{
}

BaseActorFactory::~BaseActorFactory()
{
}

void BaseActorFactory::init()
{
	registerGeneralComponents();
	vRegisterSpecificComponents();
}

std::vector<std::shared_ptr<Actor>> BaseActorFactory::createActorAndChildren(const char *resourceFile, const tinyxml2::XMLElement *overrides /*= nullptr*/)
{
	//Load the resource file. If failed, log and return an empty vector.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(resourceFile);
	const auto rootElement = xmlDoc.RootElement();
	if (!rootElement) {
		cocos2d::log("BaseActorFactory::createActorAndChildren() failed to load resource file %s", resourceFile);
		return{};
	}

	return createActorAndChildren(rootElement, overrides);
}

std::vector<std::shared_ptr<Actor>> BaseActorFactory::createActorAndChildren(const tinyxml2::XMLElement * actorElement, const tinyxml2::XMLElement * overrides /*= nullptr*/)
{
	assert(actorElement && "BaseActorFactory::createActorAndChildren() the resource element is nullptr.");

	auto parentActor = pimpl->createSingleActorWithComponents(actorElement);
	if (!parentActor) {
		return{};
	}

	auto actorVector = std::vector<std::shared_ptr<Actor>>{ parentActor };
	//Loop through each child actor element, create children actors and attach them to the parent actor.
	if (auto childrenElement = actorElement->FirstChildElement("ChildrenActors")) {
		for (auto childResourceElement = childrenElement->FirstChildElement("Resource"); childResourceElement; childResourceElement = childResourceElement->NextSiblingElement("Resource")) {
			if (!pimpl->addChildrenActors(*parentActor, actorVector, createActorAndChildren(childResourceElement->Attribute("File")))) {
				return{};
			}
		}

		for (auto childActorElement = childrenElement->FirstChildElement("Actor"); childActorElement; childActorElement = childActorElement->NextSiblingElement("Actor")) {
			if (!pimpl->addChildrenActors(*parentActor, actorVector, createActorAndChildren(childActorElement))) {
				return{};
			}
		}
	}

	pimpl->modifyActor(parentActor, overrides);
	pimpl->postInitActor(*parentActor);

	return actorVector;
}

void BaseActorFactory::modifyActor(const std::shared_ptr<Actor> & actor, const tinyxml2::XMLElement *overrides)
{
	pimpl->modifyActor(actor, overrides);
}

void BaseActorFactory::registerGeneralComponents()
{
	//#TODO: Modify the register calls if the general components are changed.
	registerComponent<GeneralRenderComponent>();
	registerComponent<SceneRenderComponent>();
	registerComponent<TransformComponent>();
	registerComponent<ActionComponent>();
	registerComponent<FiniteTimeActionComponent>();
}

void BaseActorFactory::registerComponentHelper(const std::string & typeName, std::function<std::unique_ptr<ActorComponent>()> makeUnique, std::function < std::shared_ptr<ActorComponent>()> makeShared)
{
	pimpl->m_ComponentFactory.registerType(typeName, std::move(makeUnique), std::move(makeShared));
}
