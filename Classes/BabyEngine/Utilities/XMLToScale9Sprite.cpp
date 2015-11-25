#include "ui/UIScale9Sprite.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToScale9Sprite.h"
#include "XMLToRect.h"

cocos2d::ui::Scale9Sprite * utilities::XMLToScale9Sprite(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToScale9Sprite() the xml element is nullptr.");

	auto capInsets = cocos2d::Rect();
	if (const auto capInsetsElement = xmlElement->FirstChildElement("CapInsets"))
		capInsets = XMLToRect(capInsetsElement);

	//#TODO: Add more conditions here to meet the ctors of Scale9Sprite.

	if (const auto spriteFrameElement = xmlElement->FirstChildElement("SpriteFrameName"))
		return cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(spriteFrameElement->Attribute("Value"), capInsets);

	return cocos2d::ui::Scale9Sprite::create();
}
