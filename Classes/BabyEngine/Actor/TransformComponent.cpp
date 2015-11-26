#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../Utilities/XMLToVec2.h"
#include "TransformComponent.h"
#include "BaseRenderComponent.h"
#include "Actor.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TransfromComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct TransformComponent::TransformComponentImpl
{
	TransformComponentImpl() = default;
	~TransformComponentImpl() = default;

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;

	std::vector<std::function<void()>> m_CachedOperations;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of TransformComponent.
//////////////////////////////////////////////////////////////////////////
TransformComponent::TransformComponent() : pimpl{ std::make_unique<TransformComponentImpl>() }
{
}

TransformComponent::~TransformComponent()
{
}

const cocos2d::Size & TransformComponent::getContentSize() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getContentSize();
}

void TransformComponent::setContentSize(const cocos2d::Size & size)
{
	pimpl->m_RenderComponent->getSceneNode()->setContentSize(size);
}

int TransformComponent::getLocalZOrder() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getLocalZOrder();
}

void TransformComponent::setLocalZOrder(int order)
{
	pimpl->m_RenderComponent->getSceneNode()->setLocalZOrder(order);
}

const cocos2d::Vec2 & TransformComponent::getPosition() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getPosition();
}

void TransformComponent::setPosition(const cocos2d::Vec2 & position)
{
	pimpl->m_RenderComponent->getSceneNode()->setPosition(position);
}

cocos2d::Vec2 TransformComponent::getPositionInWindow() const
{
	auto sceneNode = pimpl->m_RenderComponent->getSceneNode();

	return sceneNode->convertToWorldSpace(sceneNode->getPosition());
}

void TransformComponent::setPositionInWindow(const cocos2d::Vec2 & positionInWorld)
{
	auto sceneNode = pimpl->m_RenderComponent->getSceneNode();

	sceneNode->setPosition(sceneNode->convertToNodeSpace(positionInWorld));
}

float TransformComponent::getScale() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getScale();
}

float TransformComponent::getScaleX() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getScaleX();
}

float TransformComponent::getScaleY() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getScaleY();
}

void TransformComponent::setScaleToSize(const cocos2d::Size & size)
{
	auto sceneNode = pimpl->m_RenderComponent->getSceneNode();
	auto contentSize = sceneNode->getContentSize();
	if (contentSize.width == 0.0f || contentSize.height == 0.0f) {
		cocos2d::log("TransformComponent::setScaleToSize() the scene node has no content size therefore can't be scaled to size.");
		return;
	}

	sceneNode->setScaleX(size.width / contentSize.width);
	sceneNode->setScaleY(size.height / contentSize.height);
}

float TransformComponent::getRotation() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getRotation();
}

void TransformComponent::setRotation(float degree)
{
	pimpl->m_RenderComponent->getSceneNode()->setRotation(degree);
}

cocos2d::Vec2 TransformComponent::convertToLocalSpace(const cocos2d::Vec2 & positionInWorld) const
{
	return pimpl->m_RenderComponent->getSceneNode()->convertToNodeSpace(positionInWorld);
}

bool TransformComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	if (const auto localZOrderElement = xmlElement->FirstChildElement("LocalZOrder")) {
		pimpl->m_CachedOperations.emplace_back([localZOrder = localZOrderElement->IntAttribute("Value"), this]() {
			setLocalZOrder(localZOrder);
		});
	}

	if (const auto positionElement = xmlElement->FirstChildElement("Position")) {
		pimpl->m_CachedOperations.emplace_back([position = utilities::XMLToVec2(positionElement), this](){
			setPosition(position);
		});
	}

	return true;
}

void TransformComponent::vPostInit()
{
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
	assert(pimpl->m_RenderComponent && "TransformComponent::vPostInit() the actor has no render component.");

	for (const auto & operation : pimpl->m_CachedOperations)
		operation();
	pimpl->m_CachedOperations.clear();
}

const std::string TransformComponent::Type{ "TransformComponent" };

const std::string & TransformComponent::getType() const
{
	return Type;
}
