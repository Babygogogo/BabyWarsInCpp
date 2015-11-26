#ifndef __XML_TO_SIZE__
#define __XML_TO_SIZE__

//Forward declaration.
namespace cocos2d
{
	class Size;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Size XMLToSize(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SIZE__
