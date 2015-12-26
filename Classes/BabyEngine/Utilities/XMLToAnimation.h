#ifndef __XML_TO_ANIMATION__
#define __XML_TO_ANIMATION__

//Forward declaration.
namespace cocos2d
{
	class Animation;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Animation * XMLToAnimation(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_ANIMATION__
