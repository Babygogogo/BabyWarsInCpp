#include <cassert>

#include "cocos2d.h"

#include "BaseRenderComponent.h"

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}

void BaseRenderComponent::setVisible(bool visible)
{
	assert(m_Node && "BaseRenderComponent::setVisible() while the node is not initialized.");

	m_Node->setVisible(visible);
}

void BaseRenderComponent::runAction(cocos2d::Action * action)
{
	assert(m_Node && "BaseRenderComponent::runAction() while the node is not initialized.");

	m_Node->runAction(action);
}

void BaseRenderComponent::stopAllActions()
{
	assert(m_Node && "BaseRenderComponent::stopAllActions() while the node is not initialized.");

	m_Node->stopAllActions();
}

void BaseRenderComponent::addChild(const BaseRenderComponent & child)
{
	assert(m_Node && child.m_Node && "BaseRenderComponent::addChild() parent or child is not initialized!");

	this->m_Node->addChild(child.m_Node);
}

void BaseRenderComponent::removeFromParent()
{
	assert(m_Node && "BaseRenderComponent::removeFromParent() the node is not initialized!");

	m_Node->removeFromParent();
}
