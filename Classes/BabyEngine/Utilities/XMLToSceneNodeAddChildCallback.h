#ifndef __XML_TO_SCENE_NODE_ADD_CHILD_CALLBACK__
#define __XML_TO_SCENE_NODE_ADD_CHILD_CALLBACK__

#include <functional>

//Forward declaration.
namespace cocos2d
{
	class Node;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	std::function<void(cocos2d::Node*, cocos2d::Node*)> XMLToSceneNodeAddChildCallback(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SCENE_NODE_ADD_CHILD_CALLBACK__
