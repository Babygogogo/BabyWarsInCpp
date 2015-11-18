#ifndef __XML_TO_MENU_ITEM_IMAGE__
#define __XML_TO_MENU_ITEM_IMAGE__

//Forward declaration.
namespace cocos2d
{
	class MenuItemImage;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::MenuItemImage * XMLToMenuItemImage(tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_MENU_ITEM_IMAGE__
