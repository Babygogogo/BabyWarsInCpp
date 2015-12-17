#ifndef __XML_TO_FINITE_TIME_ACTION__
#define __XML_TO_FINITE_TIME_ACTION__

//Forward declaration.
namespace cocos2d
{
	class FiniteTimeAction;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::FiniteTimeAction * XMLToFiniteTimeAction(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_FINITE_TIME_ACTION__
