#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToSprite.h"

cocos2d::Sprite * utilities::XMLToSprite(const tinyxml2::XMLElement * xmlElement)
{
	if (auto fileNameElement = xmlElement->FirstChildElement("FileName"))
		return cocos2d::Sprite::create(fileNameElement->Attribute("Value"));

	if (auto spriteFrameNameElement = xmlElement->FirstChildElement("SpriteFrameName"))
		return cocos2d::Sprite::createWithSpriteFrameName(spriteFrameNameElement->Attribute("Value"));

	//#TODO: Add conditions to fill the create() functions of cocos2d::Sprite.

	return cocos2d::Sprite::create();
}
