#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToMenuItemImage.h"

cocos2d::MenuItemImage * utilities::XMLToMenuItemImage(const tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")) {
		auto menuItemImage = cocos2d::MenuItemImage::create();
		auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();

		if (auto normalName = createWith->Attribute("NormalSpriteFrameName"))
			menuItemImage->setNormalSpriteFrame(spriteFrameCache->getSpriteFrameByName(normalName));
		if (auto selectedName = createWith->Attribute("SelectedSpriteFrameName"))
			menuItemImage->setSelectedSpriteFrame(spriteFrameCache->getSpriteFrameByName(selectedName));

		return menuItemImage;
	}

	return cocos2d::MenuItemImage::create();
}
