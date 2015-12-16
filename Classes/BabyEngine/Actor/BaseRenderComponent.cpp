#include <cassert>

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Actor.h"
#include "BaseRenderComponent.h"

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	assert(m_Node && "BaseRenderComponent::onOwnerActorAddChild() the underlying scene node is not initialized.");
	return m_Node;
}

void BaseRenderComponent::setOnOwnerActorAddChildCallback(OwnerActorAddChildCallback && callback)
{
	m_OnOwnerActorAddChildCallback = std::move(callback);
}

void BaseRenderComponent::onOwnerActorAddChild(Actor & child)
{
	assert(m_Node && "BaseRenderComponent::onOwnerActorAddChild() the underlying scene node is not initialized.");
	if (!child.getRenderComponent()) {
		return;
	}

	if (m_OnOwnerActorAddChildCallback) {
		m_OnOwnerActorAddChildCallback(m_Node, child.getRenderComponent()->getSceneNode());
	}
	else {
		m_Node->addChild(child.getRenderComponent()->getSceneNode());
	}
}

void BaseRenderComponent::onOwnerActorRemoveChild(Actor & child)
{
	assert(m_Node && "BaseRenderComponent::onOwnerActorAddChild() the underlying scene node is not initialized.");
	if (const auto childRenderComponent = child.getRenderComponent()) {
		m_Node->removeChild(childRenderComponent->getSceneNode());
	}
}
