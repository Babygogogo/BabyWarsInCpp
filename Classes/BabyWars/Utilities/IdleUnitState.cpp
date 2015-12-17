#include <cassert>

#include "../Script/UnitMapScript.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "IdleUnitState.h"

UnitStateTypeCode IdleUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Idle;
}

void IdleUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

void IdleUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	commandList.clearListForUnit(unit);
}

void IdleUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert(unit && "UnitState::vUpdateUnitMap() the unit is expired.");
	if (unitMap.isUnitFocused(*unit)) {
		unitMap.undoMoveAndSetToIdleStateForFocusedUnit();
		unitMap.setFocusedUnit(nullptr);
	}
}

bool IdleUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	if (touchedUnit.get() == &unit) {
		unit.setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Active);
		return true;
	}

	return false;
}

void IdleUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
}

void IdleUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
}

bool IdleUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool IdleUnitState::vCanUndoMove() const
{
	return false;
}

std::vector<GameCommand> IdleUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
