#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "SetSceneNodePropertiesWithXML.h"

void utilities::setSceneNodePropertiesWithXML(cocos2d::Node * sceneNode, const tinyxml2::XMLElement * xmlElement)
{
	assert(sceneNode && "utilities::setSceneNodePropertiesWithXML() the scene node is nullptr.");
	assert(xmlElement && "utilities::setSceneNodePropertiesWithXML() the xml element is nullptr.");

	if (auto opacityElement = xmlElement->FirstChildElement("Opacity"))
		sceneNode->setOpacity(opacityElement->IntAttribute("Value"));

	//#TODO: Load and set other properties.
}
