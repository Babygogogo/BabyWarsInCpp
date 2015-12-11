#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "ActiveUnitState.h"

UnitStateTypeCode ActiveUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Active;
}

void ActiveUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.showAreaForUnit(unit);
}

void ActiveUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	commandList.clearListForUnit(unit);
}

void ActiveUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert(unit && "UnitState::vUpdateUnitMap() the unit is expired.");
	if (!unitMap.isUnitFocused(*unit)) {
		unitMap.undoMoveForFocusedUnit();
		unitMap.setFocusedUnit(unit);
	}
}

bool ActiveUnitState::vUpdateUnitOnTouch(UnitScript & unit) const
{
	unit.moveInPlace();
	return true;
}

void ActiveUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	unit.showAppearanceInActiveState();
}

void ActiveUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	unit.clearAppearanceInActiveState();
}

bool ActiveUnitState::vCanMoveAlongPath() const
{
	return true;
}

bool ActiveUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> ActiveUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
