#ifndef __XML_TO_FADE_TO__
#define __XML_TO_FADE_TO__

//Forward declaration.
namespace cocos2d
{
	class FadeTo;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::FadeTo * XMLToFadeTo(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_FADE_TO__
