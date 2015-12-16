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

void BaseRenderComponent::onOwnerActorAddChild(Actor & child)
{
	assert(m_Node && "BaseRenderComponent::onOwnerActorAddChild() the underlying scene node is not initialized.");
	if (!child.getRenderComponent()) {
		return;
	}

	auto listView = dynamic_cast<cocos2d::ui::ListView*>(m_Node);
	auto childNode = child.getRenderComponent()->getSceneNode();
	auto childWidget = dynamic_cast<cocos2d::ui::Widget*>(childNode);

	if (listView && childWidget) {
		listView->pushBackCustomItem(childWidget);
	}
	else {
		m_Node->addChild(childNode);
	}
}

void BaseRenderComponent::onOwnerActorRemoveFromParent()
{
	assert(m_Node && "BaseRenderComponent::onOwnerActorAddChild() the underlying scene node is not initialized.");
	m_Node->removeFromParent();
}
