#ifndef __XML_TO_TRANSITION_SCENE__
#define __XML_TO_TRANSITION_SCENE__

//Forward declaration.
namespace cocos2d
{
	class Scene;
	class TransitionScene;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::TransitionScene * XMLToTransitionScene(const tinyxml2::XMLElement * xmlElement, cocos2d::Scene * transiteTo);
}

#endif // !__XML_TO_TRANSITION_SCENE__
