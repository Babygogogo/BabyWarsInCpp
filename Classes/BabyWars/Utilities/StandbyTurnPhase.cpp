#include <memory>

#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../Event/EvtDataRequestChangeTurnPhase.h"
#include "../Script/UnitScript.h"
#include "TurnPhaseTypeCode.h"
#include "StandbyTurnPhase.h"
#include "GameCommand.h"

TurnPhaseTypeCode StandbyTurnPhase::vGetTypeCode() const
{
	return TurnPhaseTypeCode::Standby;
}

bool StandbyTurnPhase::vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const
{
	return newPhaseTypeCode == TurnPhaseTypeCode::Main;
}

void StandbyTurnPhase::vOnEnterPhase(TurnManagerScript & turnManagerScript) const
{
	//#TODO: This is a hack that jumps over the Standby phase. I'm doing this because I haven't implemented the stuff for Standby phase.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestChangeTurnPhase>(TurnPhaseTypeCode::Main));
}

std::vector<GameCommand> StandbyTurnPhase::vGenerateGameCommands() const
{
	return{};
}

void StandbyTurnPhase::vUpdateUnit(UnitScript & unitScript) const
{
	unitScript.setCanRespondToTouch(false);
}
