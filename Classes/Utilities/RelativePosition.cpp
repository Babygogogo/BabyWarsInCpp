#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "RelativePosition.h"

RelativePosition::RelativePosition()
{
}

RelativePosition::RelativePosition(tinyxml2::XMLElement * positionElement)
{
	initialize(positionElement);
}

RelativePosition::~RelativePosition()
{
}

void RelativePosition::initialize(tinyxml2::XMLElement * positionElement)
{
	m_NormalizedScreenOffsetX = positionElement->FloatAttribute("NormalizedScreenOffsetX");
	m_NormalizedScreenOffsetY = positionElement->FloatAttribute("NormalizedScreenOffsetY");

	m_NormalizedNodeOffsetX = positionElement->FloatAttribute("NormalizedNodeOffsetX");
	m_NormalizedNodeOffsetY = positionElement->FloatAttribute("NormalizedNodeOffsetY");

	m_PixelOffsetX = positionElement->FloatAttribute("PixelOffsetX");
	m_PixelOffsetY = positionElement->FloatAttribute("PixelOffsetY");
}

float RelativePosition::getAbsolutePositionX(cocos2d::Node * node /*= nullptr*/) const
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto screenPosX = m_NormalizedScreenOffsetX * visibleSize.width;

	auto nodeSizeX = node ? node->getContentSize().width : 0.0f;
	auto nodePosX = m_NormalizedNodeOffsetX * nodeSizeX;

	return screenPosX + nodePosX + m_PixelOffsetX;
}

float RelativePosition::getAbsolutePositionY(cocos2d::Node * node /*= nullptr*/) const
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto screenPosY = m_NormalizedScreenOffsetY * visibleSize.height;

	auto nodeSizeY = node ? node->getContentSize().height : 0.0f;
	auto nodePosY = m_NormalizedNodeOffsetY * nodeSizeY;

	return screenPosY + nodePosY + m_PixelOffsetY;
}
