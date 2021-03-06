#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "SetSceneNodePropertiesWithXML.h"
#include "XMLToSize.h"
#include "XMLToColor3B.h"

void utilities::setSceneNodePropertiesWithXML(cocos2d::Node * sceneNode, const tinyxml2::XMLElement * xmlElement)
{
	assert(sceneNode && "utilities::setSceneNodePropertiesWithXML() the scene node is nullptr.");
	assert(xmlElement && "utilities::setSceneNodePropertiesWithXML() the xml element is nullptr.");

	if (const auto opacityElement = xmlElement->FirstChildElement("Opacity")) {
		sceneNode->setOpacity(opacityElement->IntAttribute("Value"));
		sceneNode->setCascadeOpacityEnabled(opacityElement->BoolAttribute("IsCascadeEnabled"));
	}

	if (const auto contentSizeElement = xmlElement->FirstChildElement("ContentSize")) {
		sceneNode->setContentSize(XMLToSize(contentSizeElement));
	}

	if (const auto colorElement = xmlElement->FirstChildElement("Color")) {
		sceneNode->setColor(XMLToColor3B(colorElement));
	}
	//#TODO: Load and set other properties.
}
