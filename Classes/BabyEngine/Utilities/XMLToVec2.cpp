#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToVec2.h"

cocos2d::Vec2 utilities::XMLToVec2(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToVec2() the xml element is nullptr.");

	return cocos2d::Vec2(xmlElement->FloatAttribute("X"), xmlElement->FloatAttribute("Y"));
}
