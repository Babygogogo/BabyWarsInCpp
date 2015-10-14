#include <cassert>
#include <set>
#include <unordered_map>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "Actor.h"
#include "BaseRenderComponent.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of Actor::ActorImpl.
//////////////////////////////////////////////////////////////////////////
struct Actor::ActorImpl
{
public:
	ActorImpl();
	~ActorImpl();

	bool m_IsUpdating{ false };

	ActorID m_ID{ INVALID_ACTOR_ID };
	ActorID m_ParentID{ INVALID_ACTOR_ID };
	std::set<ActorID> m_ChildrenID;

	std::string m_Type;
	std::string m_ResourceFile;

	std::unordered_map<std::string, std::shared_ptr<ActorComponent>> m_Components;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

Actor::ActorImpl::ActorImpl()
{
}

Actor::ActorImpl::~ActorImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of Actor.
//////////////////////////////////////////////////////////////////////////
Actor::Actor() : pimpl{ std::make_unique<ActorImpl>() }
{
}

Actor::~Actor()
{
	cocos2d::log("Actor %s destructing.", pimpl->m_Type.c_str());

	removeFromParent();

	auto & singletonContainer = SingletonContainer::getInstance();
	if (!singletonContainer)
		return;

	auto eventDispatcher = singletonContainer->get<IEventDispatcher>();
	for (auto childID : pimpl->m_ChildrenID)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestoryActor>(childID));
}

const ActorID & Actor::getID() const
{
	return pimpl->m_ID;
}

const ActorID & Actor::getParentID() const
{
	return pimpl->m_ParentID;
}

std::shared_ptr<ActorComponent> Actor::getComponent(const std::string & type) const
{
	auto findIter = pimpl->m_Components.find(type);
	if (findIter == pimpl->m_Components.end())
		return nullptr;

	return findIter->second;
}

std::shared_ptr<BaseRenderComponent> Actor::getRenderComponent() const
{
	return pimpl->m_RenderComponent;
}

bool Actor::hasParent() const
{
	return pimpl->m_ParentID != INVALID_ACTOR_ID;
}

bool Actor::isAncestorOf(const Actor & child) const
{
	const auto & gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	auto ancestor = gameLogic->getActor(child.pimpl->m_ParentID);
	while (ancestor){
		if (pimpl->m_ID == ancestor->pimpl->m_ID)
			return true;

		ancestor = gameLogic->getActor(ancestor->pimpl->m_ParentID);
	}

	return false;
}

void Actor::addChild(Actor & child)
{
	//If the child is not valid or has parent already, simply return.
	if (child.hasParent())
		return;

	child.pimpl->m_ParentID = pimpl->m_ID;
	pimpl->m_ChildrenID.emplace(child.getID());

	//Deal with child's render component if present
	if (auto childRenderComponent = child.pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->addChild(*childRenderComponent);
}

void Actor::removeFromParent()
{
	if (!hasParent())
		return;

	//If the parent is alive, remove this from the parent's children list.
	if (auto & singletonContainer = SingletonContainer::getInstance())
		if (auto parent = singletonContainer->get<GameLogic>()->getActor(pimpl->m_ParentID))
			parent->pimpl->m_ChildrenID.erase(pimpl->m_ID);

	pimpl->m_ParentID = INVALID_ACTOR_ID;

	//Deal with child's render component if present.
	if (pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->removeFromParent();
}

bool Actor::init(ActorID id, tinyxml2::XMLElement *xmlElement)
{
	if (id == 0 || !xmlElement)
		return false;

	auto actorType = xmlElement->Attribute("Type");
	if (!actorType){
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	auto resourceFile = xmlElement->Attribute("Resource");
	if (!resourceFile){
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	pimpl->m_ID = std::move(id);
	pimpl->m_Type = actorType;
	pimpl->m_ResourceFile = resourceFile;

	return true;
}

void Actor::postInit()
{
	for (auto & componentIter : pimpl->m_Components)
		componentIter.second->vPostInit();
}

void Actor::addComponent(std::shared_ptr<ActorComponent> && component)
{
	//Ensure that the component is alive.
	assert(component);

	//Check if the component is an render component, and make sure that the actor can have no more than one render component.
	if (auto renderComponent = std::dynamic_pointer_cast<BaseRenderComponent>(component)){
		assert(!pimpl->m_RenderComponent && "Actor::addComponent() trying to add more than one render component.");
		pimpl->m_RenderComponent = std::move(renderComponent);
	}

	auto emplaceResult = pimpl->m_Components.emplace(std::make_pair(component->getType(), std::move(component)));
	assert(emplaceResult.second && "Actor::addComponent() can't emplace the component due to some unknown reasons.");
}

void Actor::update(const std::chrono::milliseconds & delteTimeMs)
{
	//#TODO: call update() on all components here.
}
