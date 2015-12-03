#ifndef __XML_TO_BUTTON__
#define __XML_TO_BUTTON__

//Forward declaration.
namespace cocos2d
{
	namespace ui
	{
		class Button;
	}
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::ui::Button * XMLToButton(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_BUTTON__
