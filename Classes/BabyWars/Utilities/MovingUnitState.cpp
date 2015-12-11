#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/CommandListScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "MovingUnitState.h"

UnitStateTypeCode MovingUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Moving;
}

void MovingUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

void MovingUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	commandList.clearListForUnit(unit);
}

void MovingUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert(unit && "UnitState::vUpdateUnitMap() the unit is expired.");
	assert(unitMap.isUnitFocused(*unit) && "UnitState::vUpdateUnitMap() the moving unit is not the focused unit in map.");

	unitMap.removeFocusedUnitIndexFromMap();
}

bool MovingUnitState::vUpdateUnitOnTouch(UnitScript & unit) const
{
	return false;
}

void MovingUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	unit.showAppearanceInMovingState();
}

void MovingUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	unit.clearAppearanceInMovingState();
}

bool MovingUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool MovingUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> MovingUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
