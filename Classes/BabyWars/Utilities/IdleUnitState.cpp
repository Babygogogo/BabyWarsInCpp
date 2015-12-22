#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "IdleUnitState.h"

UnitStateTypeCode IdleUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Idle;
}

void IdleUnitState::vOnEnterState(UnitScript & unit) const
{
}

void IdleUnitState::vOnExitState(UnitScript & unit) const
{
}

bool IdleUnitState::vIsNeedFocusForUnitMap() const
{
	return false;
}

void IdleUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

bool IdleUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	if (touchedUnit.get() == &unit) {
		unit.setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Active);
		return true;
	}

	return false;
}

bool IdleUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool IdleUnitState::vCanUndoMove() const
{
	return false;
}

std::vector<GameCommand> IdleUnitState::vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
