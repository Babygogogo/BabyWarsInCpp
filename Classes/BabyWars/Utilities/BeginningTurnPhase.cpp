#include "../Script/TurnManagerScript.h"
#include "../Script/UnitScript.h"
#include "TurnPhaseTypeCode.h"
#include "BeginningTurnPhase.h"
#include "GameCommand.h"

TurnPhaseTypeCode BeginningTurnPhase::vGetTypeCode() const
{
	return TurnPhaseTypeCode::Beginning;
}

bool BeginningTurnPhase::vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const
{
	return newPhaseTypeCode == TurnPhaseTypeCode::Standby;
}

void BeginningTurnPhase::vOnEnterPhase(TurnManagerScript & turnManagerScript) const
{
	turnManagerScript.updateTurnIndexAndPlayerIdForNewTurn();
}

std::vector<GameCommand> BeginningTurnPhase::vGenerateGameCommands() const
{
	return{};
}

void BeginningTurnPhase::vUpdateUnit(UnitScript & unitScript) const
{
	unitScript.setCanRespondToTouch(false);
}
