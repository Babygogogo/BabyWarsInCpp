#include <cassert>

#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "Actor.h"
#include "BaseRenderComponent.h"

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
	std::unordered_map<ActorID, std::weak_ptr<Actor>> m_Children;

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
	removeAllChildren();
}

ActorID Actor::getID() const
{
	return pimpl->m_ID;
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
	return !pimpl->m_Parent.expired();
}

std::shared_ptr<Actor> Actor::getParent() const
{
	if (pimpl->m_Parent.expired())
		return nullptr;

	return pimpl->m_Parent.lock();
}

bool Actor::isAncestorOf(const Actor & child) const
{
	auto weakAncestor = child.pimpl->m_Parent;
	while (!weakAncestor.expired()) {
		auto strongAncestor = weakAncestor.lock();
		if (strongAncestor->pimpl->m_ID == pimpl->m_ID)
			return true;

		weakAncestor = strongAncestor->pimpl->m_Parent;
	}

	return false;
}

const std::unordered_map<ActorID, std::weak_ptr<Actor>> & Actor::getChildren() const
{
	return pimpl->m_Children;
}

void Actor::addChild(Actor & child)
{
	assert(!child.hasParent() && "Actor::addChild() the child actor already has a parent.");

	child.pimpl->m_Parent = pimpl->m_Self;
	pimpl->m_Children.emplace(child.getID(), child.pimpl->m_Self);

	//Deal with render component.
	if (pimpl->m_RenderComponent) {
		pimpl->m_RenderComponent->onOwnerActorAddChild(child);
	}
}

void Actor::removeChild(Actor & child)
{
	if (!child.hasParent() || child.getParent().get() != this) {
		return;
	}

	child.pimpl->m_Parent.reset();
	pimpl->m_Children.erase(child.getID());

	//Deal with render component.
	if (pimpl->m_RenderComponent) {
		pimpl->m_RenderComponent->onOwnerActorRemoveChild(child);
	}
}

void Actor::removeFromParent()
{
	if (!hasParent()) {
		return;
	}

	getParent()->removeChild(*this);
}

void Actor::removeAllChildren()
{
	for (const auto & idChildPair : pimpl->m_Children) {
		if (!idChildPair.second.expired()) {
			auto child = idChildPair.second.lock();
			child->pimpl->m_Parent.reset();

			//Deal with render component.
			if (pimpl->m_RenderComponent) {
				pimpl->m_RenderComponent->onOwnerActorRemoveChild(*child);
			}
		}
	}

	pimpl->m_Children.clear();
}

bool Actor::init(ActorID id, const std::shared_ptr<Actor> & selfPtr, const tinyxml2::XMLElement * xmlElement)
{
	if (id == 0 || !xmlElement)
		return false;

	auto actorType = xmlElement->Attribute("Type");
	if (!actorType) {
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	auto resourceFile = xmlElement->Attribute("Resource");
	if (!resourceFile) {
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	pimpl->m_ID = std::move(id);
	pimpl->m_Self = selfPtr;
	pimpl->m_Type = actorType;
	pimpl->m_ResourceFile = resourceFile;

	return true;
}

void Actor::addComponent(std::shared_ptr<ActorComponent> && component)
{
	assert(component && "Actor::addComponent() the component is nullptr.");

	//Check if the component is an render component, and make sure that the actor can have no more than one render component.
	if (auto renderComponent = std::dynamic_pointer_cast<BaseRenderComponent>(component)) {
		assert(!pimpl->m_RenderComponent && "Actor::addComponent() trying to add more than one render component.");
		pimpl->m_RenderComponent = std::move(renderComponent);
	}

	auto emplaceResult = pimpl->m_Components.emplace(std::make_pair(component->getType(), std::move(component)));
	assert(emplaceResult.second && "Actor::addComponent() can't emplace the component possibly because a component of the same type already exists.");
}

void Actor::postInit()
{
	for (auto & componentIter : pimpl->m_Components)
		componentIter.second->vPostInit();
}

void Actor::update(const std::chrono::milliseconds & delteTimeMs)
{
	//#TODO: call update() on all components here.
}
