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
	cocos2d::TransitionScene * XMLToTransitionScene(tinyxml2::XMLElement * xmlElement, cocos2d::Scene * scene);
}

#endif // !__XML_TO_TRANSITION_SCENE__
