#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToFadeTo.h"
#include "XMLToFiniteTimeAction.h"

cocos2d::FiniteTimeAction * utilities::XMLToFiniteTimeAction(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToFiniteTimeAction() the xml element is nullptr.");

	const auto actionType = std::string(xmlElement->Value());
	if (actionType == "Sequence") {
	}
	else if (actionType == "FadeTo") {
		return XMLToFadeTo(xmlElement);
	}

	return nullptr;
}
