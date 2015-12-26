#include <cassert>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "ColorTypeCode.h"
#include "XMLToColorTypeCode.h"

ColorTypeCode utilities::XMLToColorTypeCode(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToColorTypeCode() the xml element is nullptr.");
	return StringToColorTypeCode(xmlElement->Attribute("Value"));
}

ColorTypeCode utilities::StringToColorTypeCode(const std::string & codeString)
{
	if (codeString == "Invalid") {
		return ColorTypeCode::Invalid;
	}
	else if (codeString == "Orange") {
		return ColorTypeCode::Orange;
	}
	else if (codeString == "Blue") {
		return ColorTypeCode::Blue;
	}
	else if (codeString == "Green") {
		return ColorTypeCode::Green;
	}
	else if (codeString == "Yellow") {
		return ColorTypeCode::Yellow;
	}
	else if (codeString == "Grey") {
		return ColorTypeCode::Grey;
	}

	assert("utilities::StringToColorTypeCode() the code string is unrecognized.");
	return ColorTypeCode::Invalid;
}
