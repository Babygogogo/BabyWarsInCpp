#include <cassert>

#include "cocos2d.h"

#include "BaseRenderComponent.h"

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}
