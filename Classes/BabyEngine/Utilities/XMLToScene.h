#ifndef __XML_TO_SCENE__
#define __XML_TO_SCENE__

//Forward declaration.
namespace cocos2d
{
	class Scene;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Scene * XMLToScene(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SCENE__
