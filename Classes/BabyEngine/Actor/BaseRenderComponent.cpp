#include <cassert>

#include "cocos2d.h"

#include "BaseRenderComponent.h"

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}

void BaseRenderComponent::runAction(cocos2d::Action * action)
{
	assert(m_Node && "BaseRenderComponent::runAction() while the node is not initialized.");

	m_Node->runAction(action);
}

void BaseRenderComponent::stopAction(cocos2d::Action * action)
{
	assert(m_Node && "BaseRenderComponent::stopAction() while the node is not initialized.");

	m_Node->stopAction(action);
}

void BaseRenderComponent::stopAllActions()
{
	assert(m_Node && "BaseRenderComponent::stopAllActions() while the node is not initialized.");

	m_Node->stopAllActions();
}
