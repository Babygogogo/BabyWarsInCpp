#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "../Script/UnitMapScript.h"
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

void MovingUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert(unit && "UnitState::vUpdateUnitMap() the unit is expired.");
	assert(unitMap.isUnitFocused(*unit) && "UnitState::vUpdateUnitMap() the moving unit is not the focused unit in map.");

	unitMap.removeFocusedUnitIndexFromMap();
}

bool MovingUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	unit.undoMoveAndSetToIdleState();
	return true;
}

void MovingUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
}

void MovingUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
}

bool MovingUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool MovingUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> MovingUnitState::vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
