#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

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

int TransformComponent::getLocalZOrder() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getLocalZOrder();
}

void TransformComponent::setLocalZOrder(int order)
{
	pimpl->m_RenderComponent->getSceneNode()->setLocalZOrder(order);
}

cocos2d::Vec2 TransformComponent::getPosition() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getPosition();
}

void TransformComponent::setPosition(const cocos2d::Vec2 & position)
{
	pimpl->m_RenderComponent->getSceneNode()->setPosition(position);
}

float TransformComponent::getScale() const
{
	return pimpl->m_RenderComponent->getSceneNode()->getScale();
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

bool TransformComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	if (auto localZOrderElement = xmlElement->FirstChildElement("LocalZOrder")) {
		auto localZOrder = localZOrderElement->IntAttribute("Value");
		pimpl->m_CachedOperations.emplace_back([localZOrder, this]() {
			setLocalZOrder(localZOrder);
		});
	}

	return true;
}

void TransformComponent::vPostInit()
{
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getBaseRenderComponent();
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
