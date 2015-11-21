#ifndef __XML_TO_MENU__
#define __XML_TO_MENU__

//Forward declaration.
namespace cocos2d
{
	class Menu;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Menu * XMLToMenu(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_MENU__
