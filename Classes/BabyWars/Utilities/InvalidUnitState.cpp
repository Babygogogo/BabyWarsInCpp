#include <cassert>

#include "UnitStateTypeCode.h"
#include "InvalidUnitState.h"
#include "GameCommand.h"

UnitStateTypeCode InvalidUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Invalid;
}

void InvalidUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	assert("UnitState::vUpdateMovingArea() the unit state is invalid.");
}

void InvalidUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	assert("UnitState::vUpdateCommandList() the unit state is invalid.");
}

void InvalidUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert("UnitState::vUpdateUnitMap() the unit state is invalid.");
}

bool InvalidUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	assert("UnitState::updateUnit() the unit state is invalid.");
	return false;
}

void InvalidUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	assert("UnitState::vShowUnitAppearanceInState() the unit state is invalid.");
}

void InvalidUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	assert("UnitState::vClearUnitAppearanceInState() the unit state is invalid.");
}

bool InvalidUnitState::vCanMoveAlongPath() const
{
	assert("UnitState::vCanMoveAlongPath() the unit state is invalid.");
	return false;
}

bool InvalidUnitState::vCanUndoMove() const
{
	assert("UnitState::vCanUndoMove() the unit state is invalid.");
	return false;
}

std::vector<GameCommand> InvalidUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	assert("UnitState::vGetCommandsForUnit() the unit state is invalid.");
	return{};
}
