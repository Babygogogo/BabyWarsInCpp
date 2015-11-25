#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToRect.h"

cocos2d::Rect utilities::XMLToRect(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToRect() the xml element is nullptr.");

	return cocos2d::Rect(xmlElement->FloatAttribute("X"), xmlElement->FloatAttribute("Y"), xmlElement->FloatAttribute("Width"), xmlElement->FloatAttribute("Height"));
}
