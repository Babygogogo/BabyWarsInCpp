#include <cassert>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

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
	std::weak_ptr<BaseHumanView> m_HumanView;

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

std::shared_ptr<BaseRenderComponent> Actor::getBaseRenderComponent() const
{
	return pimpl->m_RenderComponent;
}

bool Actor::isAttachedToHumanView() const
{
	return !pimpl->m_HumanView.expired();
}

std::shared_ptr<BaseHumanView> Actor::getHumanView() const
{
	if (!isAttachedToHumanView())
		return nullptr;

	return pimpl->m_HumanView.lock();
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
	//If the child is not valid or has parent already, simply return.
	if (child.hasParent())
		return;
	if (child.isAttachedToHumanView())
		return;

	child.pimpl->m_Parent = pimpl->m_Self;
	pimpl->m_Children.emplace(child.getID(), child.pimpl->m_Self);

	//Deal with child's render component if present
	if (auto childRenderComponent = child.pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->addChild(*childRenderComponent);
}

void Actor::removeFromParent()
{
	if (!hasParent())
		return;

	//Remove this from the parent's children list.
	pimpl->m_Parent.lock()->pimpl->m_Children.erase(pimpl->m_ID);
	pimpl->m_Parent.reset();

	//Deal with child's render component if present.
	if (pimpl->m_RenderComponent)
		pimpl->m_RenderComponent->removeFromParent();
}

void Actor::removeAllChildren()
{
	for (const auto & idChildPair : pimpl->m_Children) {
		if (idChildPair.second.expired())
			continue;

		auto child = idChildPair.second.lock();
		//Can't call child.removeFromParent() here because it modifies the pimpl->m_Children!
		child->pimpl->m_Parent.reset();
		if (auto childRenderComponent = child->getBaseRenderComponent())
			childRenderComponent->removeFromParent();
	}

	pimpl->m_Children.clear();
}

bool Actor::init(ActorID id, const std::shared_ptr<Actor> & selfPtr, tinyxml2::XMLElement *xmlElement)
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

void Actor::setHumanView(std::weak_ptr<BaseHumanView> humanView)
{
	pimpl->m_HumanView = humanView;
}
