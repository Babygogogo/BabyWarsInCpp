#include <memory>

#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../Event/EvtDataRequestChangeTurnPhase.h"
#include "TurnPhaseTypeCode.h"
#include "EndTurnPhase.h"
#include "GameCommand.h"

TurnPhaseTypeCode EndTurnPhase::vGetTypeCode() const
{
	return TurnPhaseTypeCode::End;
}

bool EndTurnPhase::vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const
{
	return newPhaseTypeCode == TurnPhaseTypeCode::Beginning;
}

void EndTurnPhase::vOnEnterPhase(TurnManagerScript & turnManagerScript) const
{
	//#TODO: This is a hack that jumps over the End phase. I'm doing this because I haven't implemented the stuff for End phase.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestChangeTurnPhase>(TurnPhaseTypeCode::Beginning));
}

std::vector<GameCommand> EndTurnPhase::vGenerateGameCommands() const
{
	return{};
}
