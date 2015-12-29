#include <memory>

#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../Event/EvtDataRequestChangeTurnPhase.h"
#include "../Script/UnitScript.h"
#include "TurnPhaseTypeCode.h"
#include "MainTurnPhase.h"
#include "GameCommand.h"

TurnPhaseTypeCode MainTurnPhase::vGetTypeCode() const
{
	return TurnPhaseTypeCode::Main;
}

bool MainTurnPhase::vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const
{
	return newPhaseTypeCode == TurnPhaseTypeCode::End;
}

void MainTurnPhase::vOnEnterPhase(TurnManagerScript & turnManagerScript) const
{
}

std::vector<GameCommand> MainTurnPhase::vGenerateGameCommands() const
{
	auto commands = std::vector<GameCommand>{};

	commands.emplace_back("End Turn", []() {
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestChangeTurnPhase>(TurnPhaseTypeCode::End));
	});

	return commands;
}

void MainTurnPhase::vUpdateUnit(UnitScript & unitScript) const
{
	unitScript.setCanRespondToTouch(true);
}
