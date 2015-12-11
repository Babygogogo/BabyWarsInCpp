#include <cassert>

#include "../Script/CommandListScript.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "GridIndex.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "MovingEndUnitState.h"

UnitStateTypeCode MovingEndUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::MovingEnd;
}

void MovingEndUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

void MovingEndUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	commandList.showListForUnit(unit);
}

void MovingEndUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
}

bool MovingEndUnitState::vUpdateUnitOnTouch(UnitScript & unit) const
{
	return false;
}

void MovingEndUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	unit.showAppearanceInMovingEndState();
}

void MovingEndUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	unit.clearAppearanceInMovingEndState();
}

bool MovingEndUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool MovingEndUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> MovingEndUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	assert(targetUnit && "UnitState::vGetCommandsForUnit() the target unit is expired.");
	auto commands = std::vector<GameCommand>{};
	auto unitScript = std::weak_ptr<UnitScript>(targetUnit);

	commands.emplace_back("Wait", [unitScript = unitScript]() {
		if (unitScript.expired()) {
			return;
		}

		auto strongUnit = unitScript.lock();
		strongUnit->setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Waiting);
		strongUnit->setGridIndexAndPosition(strongUnit->getGridIndex());
	});
	return commands;
}
