#include <cassert>
#include <string>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToSceneNode.h"
#include "XMLToButton.h"
#include "XMLToLabel.h"
#include "XMLToLayer.h"
#include "XMLToListView.h"
#include "XMLToMenu.h"
#include "XMLToMenuItemImage.h"
#include "XMLToNode.h"
#include "XMLToParticleExplosion.h"
#include "XMLToScale9Sprite.h"
#include "XMLToSprite.h"

cocos2d::Node * utilities::XMLToSceneNode(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToSceneNode() the xml element is nullptr.");

	//#TODO: Complete the if statements.
	const auto nodeType = std::string(xmlElement->Attribute("Type"));
	if (nodeType == "Sprite")					return XMLToSprite(xmlElement);
	else if (nodeType == "Layer")				return XMLToLayer(xmlElement);
	else if (nodeType == "Node")				return XMLToNode(xmlElement);
	else if (nodeType == "Label")				return XMLToLabel(xmlElement);
	else if (nodeType == "Menu")				return XMLToMenu(xmlElement);
	else if (nodeType == "MenuItemImage")		return XMLToMenuItemImage(xmlElement);
	else if (nodeType == "ParticleExplosion")	return XMLToParticleExplosion(xmlElement);
	else if (nodeType == "Scale9Sprite")		return XMLToScale9Sprite(xmlElement);
	else if (nodeType == "Button")				return XMLToButton(xmlElement);
	else if (nodeType == "ListView")			return XMLToListView(xmlElement);

	return nullptr;
}
