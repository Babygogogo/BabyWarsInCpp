#include <cassert>

#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToMoney.h"

Money utilities::XMLToMoney(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToMoney() the xml element is nullptr.");

	return xmlElement->IntAttribute("Value");
}
