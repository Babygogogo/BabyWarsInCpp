#include <cassert>
#include <string>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "TurnPhaseTypeCode.h"
#include "StandbyTurnPhase.h"
#include "MainTurnPhase.h"
#include "EndTurnPhase.h"
#include "InvalidTurnPhase.h"
#include "BeginningTurnPhase.h"
#include "TurnPhaseFactory.h"

namespace utilities
{
	TurnPhaseTypeCode toTurnPhaseTypeCode(const std::string & typeString)
	{
		if (typeString == "Invalid") {
			return TurnPhaseTypeCode::Invalid;
		}
		else if (typeString == "Beginning") {
			return TurnPhaseTypeCode::Beginning;
		}
		else if (typeString == "Standby") {
			return TurnPhaseTypeCode::Standby;
		}
		else if (typeString == "Main") {
			return TurnPhaseTypeCode::Main;
		}
		else if (typeString == "End") {
			return TurnPhaseTypeCode::End;
		}
		else {
			assert("utilities::toTurnPhaseTypeCode() the type string is unrecognized.");
			return TurnPhaseTypeCode::Invalid;
		}
	}
}

std::shared_ptr<TurnPhase> utilities::createTurnPhaseWithTypeCode(TurnPhaseTypeCode typeCode)
{
	switch (typeCode)
	{
	case TurnPhaseTypeCode::Invalid:
		return std::make_shared<InvalidTurnPhase>();
		break;

	case TurnPhaseTypeCode::Beginning:
		return std::make_shared<BeginningTurnPhase>();
		break;

	case TurnPhaseTypeCode::Standby:
		return std::make_shared<StandbyTurnPhase>();
		break;

	case TurnPhaseTypeCode::Main:
		return std::make_shared<MainTurnPhase>();
		break;

	case TurnPhaseTypeCode::End:
		return std::make_shared<EndTurnPhase>();
		break;

	default:
		assert("utilities::createTurnPhaseWithTypeCode() falls to default, which should not happen.");
		return nullptr;
		break;
	}
}

std::shared_ptr<TurnPhase> utilities::createTurnPhaseWithXML(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::createTurnPhaseWithXML() the xml element is nullptr.");

	const auto typeString = std::string(xmlElement->Attribute("TurnPhase"));
	return createTurnPhaseWithTypeCode(toTurnPhaseTypeCode(typeString));
}
