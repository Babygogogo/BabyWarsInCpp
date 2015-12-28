#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToSize.h"

cocos2d::Size utilities::XMLToSize(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToSize() the xml element is nullptr.");

	return cocos2d::Size(xmlElement->FloatAttribute("Width"), xmlElement->FloatAttribute("Height"));
}
