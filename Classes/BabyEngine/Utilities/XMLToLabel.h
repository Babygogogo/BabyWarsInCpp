#ifndef __XML_TO_LABEL__
#define __XML_TO_LABEL__

//Forward declaration.
namespace cocos2d
{
	class Label;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Label * XMLToLabel(tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_LABEL__
