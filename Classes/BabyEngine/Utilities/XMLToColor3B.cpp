#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToColor3B.h"

cocos2d::Color3B utilities::XMLToColor3B(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToColor3B() the xmlElement is nullptr.");

	return cocos2d::Color3B(xmlElement->IntAttribute("R"), xmlElement->IntAttribute("G"), xmlElement->IntAttribute("B"));
}
