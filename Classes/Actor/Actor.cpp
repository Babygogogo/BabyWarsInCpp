#include <cassert>
#include <set>
#include <unordered_map>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "Actor.h"
#include "BaseRenderComponent.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
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
	std::weak_ptr<Actor> m_Parent;
	std::weak_ptr<Actor> m_Self;
	std::set<ActorID> m_ChildrenID;

	std::string m_Type;
	std::string m_ResourceFile;

	std::unordered_map<std::string, std::unique_ptr<ActorComponent>> m_Components;
	BaseRenderComponent * m_RenderComponent{ nullptr };
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

const std::weak_ptr<Actor> & Actor::getParent() const
{
	return pimpl->m_Parent;
}

ActorComponent * Actor::getComponent(const std::string & type) const
{
	auto findIter = pimpl->m_Components.find(type);
	if (findIter == pimpl->m_Components.end())
		return nullptr;

	return findIter->second.get();
}

BaseRenderComponent * Actor::getRenderComponent() const
{
	return pimpl->m_RenderComponent;
}

bool Actor::hasParent() const
{
	return !pimpl->m_Parent.expired();
}

bool Actor::isAncestorOf(const Actor & child) const
{
	auto weakAncestor = child.pimpl->m_Parent;
	while (!weakAncestor.expired()){
		auto strongAncestor = weakAncestor.lock();
		if (strongAncestor->pimpl->m_ID == pimpl->m_ID)
			return true;

		weakAncestor = strongAncestor->pimpl->m_Parent;
	}

	return false;
}

void Actor::addChild(Actor & child)
{
	//If the child is not valid or has parent already, simply return.
	if (child.hasParent())
		return;

	child.pimpl->m_Parent = pimpl->m_Self;
	pimpl->m_ChildrenID.emplace(child.getID());

	//Deal with child's render component if present
	if (auto childRenderComponent = child.pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->addChild(*childRenderComponent);
}

void Actor::removeFromParent()
{
	if (!hasParent())
		return;

	//Remove this from the parent's children list.
	pimpl->m_Parent.lock()->pimpl->m_ChildrenID.erase(pimpl->m_ID);
	pimpl->m_Parent.reset();

	//Deal with child's render component if present.
	if (pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->removeFromParent();
}

bool Actor::init(ActorID id, const std::shared_ptr<Actor> & selfPtr, tinyxml2::XMLElement *xmlElement)
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
	pimpl->m_Self = selfPtr;
	pimpl->m_Type = actorType;
	pimpl->m_ResourceFile = resourceFile;

	return true;
}

void Actor::postInit()
{
	for (auto & componentIter : pimpl->m_Components)
		componentIter.second->vPostInit();
}

void Actor::addComponent(std::unique_ptr<ActorComponent> && component)
{
	//Ensure that the component is alive.
	assert(component);

	//Check if the component is an render component, and make sure that the actor can have no more than one render component.
	if (auto renderComponent = dynamic_cast<BaseRenderComponent*>(component.get())){
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
