#ifndef __SET_SCENE_NODE_PROPERTIES_WITH_XML__
#define __SET_SCENE_NODE_PROPERTIES_WITH_XML__

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
	void setSceneNodePropertiesWithXML(cocos2d::Node * sceneNode, const tinyxml2::XMLElement * xmlElement);
}

#endif // !__SET_SCENE_NODE_PROPERTIES_WITH_XML__
