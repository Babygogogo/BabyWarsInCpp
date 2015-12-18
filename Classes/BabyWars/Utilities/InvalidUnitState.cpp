#include <cassert>

#include "UnitStateTypeCode.h"
#include "InvalidUnitState.h"
#include "GameCommand.h"

UnitStateTypeCode InvalidUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Invalid;
}

void InvalidUnitState::onUnitEnterState(UnitScript & unit) const
{
	assert("InvalidUnitState::onUnitEnterState() the state is invalid.");
}

void InvalidUnitState::onUnitExitState(UnitScript & unit) const
{
	assert("InvalidUnitState::onUnitExitState() the state is invalid.");
}

bool InvalidUnitState::vIsNeedFocusForUnitMap() const
{
	assert("InvalidUnitState::vIsNeedFocusForUnitMap() the state is invalid.");
	return false;
}

void InvalidUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	assert("UnitState::vUpdateMovingArea() the unit state is invalid.");
}

bool InvalidUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	assert("UnitState::updateUnit() the unit state is invalid.");
	return false;
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

std::vector<GameCommand> InvalidUnitState::vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	assert("UnitState::vGenerateGameCommandsForUnit() the unit state is invalid.");
	return{};
}
