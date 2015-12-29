#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToPlayerID.h"

PlayerID utilities::XMLToPlayerID(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToPlayerID() the xml element is nullptr.");

	return xmlElement->Attribute("Value");
}
