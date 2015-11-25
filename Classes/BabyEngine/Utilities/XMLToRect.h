#ifndef __XML_TO_NODE__
#define __XML_TO_NODE__

//Forward declaration.
namespace cocos2d
{
	class Rect;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Rect XMLToRect(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_NODE__
