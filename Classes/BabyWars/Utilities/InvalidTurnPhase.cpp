#include <cassert>

#include "TurnPhaseTypeCode.h"
#include "InvalidTurnPhase.h"
#include "GameCommand.h"

TurnPhaseTypeCode InvalidTurnPhase::vGetTypeCode() const
{
	return TurnPhaseTypeCode::Invalid;
}

bool InvalidTurnPhase::vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const
{
	assert("InvalidTurnPhase::vCanEnterNewPhase() the turn phase is invalid.");
	return false;
}

void InvalidTurnPhase::vOnEnterPhase(TurnManagerScript & turnManagerScript) const
{
	assert("InvalidTurnPhase::vOnEnterPhase() the turn phase is invalid.");
}

std::vector<GameCommand> InvalidTurnPhase::vGenerateGameCommands() const
{
	assert("InvalidTurnPhase::vGenerateGameCommands() the turn phase is invalid.");
	return{};
}

void InvalidTurnPhase::vUpdateUnit(UnitScript & unitScript) const
{
	assert("InvalidTurnPhase::vUpdateUnit() the turn phase is invalid.");
}
