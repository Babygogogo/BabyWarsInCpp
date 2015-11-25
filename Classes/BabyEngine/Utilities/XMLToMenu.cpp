#include "cocos2d.h"

#include "XMLToMenu.h"

cocos2d::Menu * utilities::XMLToMenu(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToMenu() the xml element is nullptr.");

	return cocos2d::Menu::create();
}
