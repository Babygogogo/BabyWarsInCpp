#ifndef __XML_TO_COLOR_3B__
#define __XML_TO_COLOR_3B__

//Forward declaration.
namespace cocos2d
{
	struct Color3B;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Color3B XMLToColor3B(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_COLOR_3B__
