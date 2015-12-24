#include <string>
#include <cassert>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitStateTypeCode.h"
#include "XMLToUnitStateTypeCode.h"

UnitStateTypeCode utilities::XMLToUnitStateTypeCode(const tinyxml2::XMLElement * xmlElement)
{
	const auto codeString = std::string(xmlElement->Attribute("Value"));
	if (codeString == "Invalid") {
		return UnitStateTypeCode::Invalid;
	}
	else if (codeString == "Idle") {
		return UnitStateTypeCode::Idle;
	}
	else if (codeString == "Active") {
		return UnitStateTypeCode::Active;
	}
	else if (codeString == "Moving") {
		return UnitStateTypeCode::Moving;
	}
	else if (codeString == "MovingEnd") {
		return UnitStateTypeCode::MovingEnd;
	}
	else if (codeString == "Waiting") {
		return UnitStateTypeCode::Waiting;
	}

	assert("utilities::XMLToUnitStateTypeCode() the type code is unrecognized.");
	return UnitStateTypeCode::Invalid;
}
