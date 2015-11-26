#ifndef __XML_TO_VEC_2__
#define __XML_TO_VEC_2__

//Forward declaration.
namespace cocos2d
{
	class Vec2;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Vec2 XMLToVec2(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_VEC_2__
