#ifndef __XML_TO_SCENE_NODE__
#define __XML_TO_SCENE_NODE__

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
	cocos2d::Node * XMLToSceneNode(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SCENE_NODE__
