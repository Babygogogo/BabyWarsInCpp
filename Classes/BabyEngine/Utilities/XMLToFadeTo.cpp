#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToFadeTo.h"

cocos2d::FadeTo * utilities::XMLToFadeTo(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToFadeIn() the xml element is nullptr.");

	return cocos2d::FadeTo::create(xmlElement->FloatAttribute("Duration"), xmlElement->IntAttribute("Opacity"));
}
