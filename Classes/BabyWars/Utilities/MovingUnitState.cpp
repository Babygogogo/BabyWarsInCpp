#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "MovingUnitState.h"

UnitStateTypeCode MovingUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Moving;
}

void MovingUnitState::vOnEnterState(UnitScript & unit) const
{
}

void MovingUnitState::vOnExitState(UnitScript & unit) const
{
}

bool MovingUnitState::vIsNeedFocusForUnitMap() const
{
	return true;
}

void MovingUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

bool MovingUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	unit.undoMoveAndSetToIdleState();
	return true;
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
