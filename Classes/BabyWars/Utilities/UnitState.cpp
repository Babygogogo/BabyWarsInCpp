#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "UnitState.h"

void UnitState::updateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::updateMovingArea() the unit state is invalid.");
		break;

	case UnitState::State::Idle:
		movingArea.clearAreaForUnit(unit);
		break;

	case UnitState::State::Active:
		movingArea.showAreaForUnit(unit);
		break;

	case UnitState::State::Moving:
		movingArea.clearAreaForUnit(unit);
		break;

	case UnitState::State::MovingEnd:
		movingArea.clearAreaForUnit(unit);
		break;

	case UnitState::State::Waiting:
		movingArea.clearAreaForUnit(unit);
		break;

	default:
		assert("UnitState::updateMovingArea() falls to default in switch(m_State). This should not happen.");
		break;
	}
}

void UnitState::updateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::updateCommandList() the unit state is invalid.");
		break;

	case UnitState::State::Idle:
		commandList.clearListForUnit(unit);
		break;

	case UnitState::State::Active:
		commandList.clearListForUnit(unit);
		break;

	case UnitState::State::Moving:
		commandList.clearListForUnit(unit);
		break;

	case UnitState::State::MovingEnd:
		commandList.showListForUnit(unit);
		break;

	case UnitState::State::Waiting:
		commandList.clearListForUnit(unit);
		break;

	default:
		assert("UnitState::updateCommandList() falls to default in switch(m_State). This should not happen.");
		break;
	}
}

void UnitState::updateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::updateUnitMap() the unit state is invalid.");
		break;

	case UnitState::State::Idle:
		assert(unit && "UnitState::updateUnitMap() the unit is expired.");
		if (unitMap.isUnitFocused(*unit)) {
			unitMap.undoMoveForFocusedUnit();
			unitMap.setFocusedUnit(nullptr);
		}
		break;

	case UnitState::State::Active:
		assert(unit && "UnitState::updateUnitMap() the unit is expired.");
		if (!unitMap.isUnitFocused(*unit)) {
			unitMap.undoMoveForFocusedUnit();
			unitMap.setFocusedUnit(unit);
		}
		break;

	case UnitState::State::Moving:
		assert(unit && "UnitState::updateUnitMap() the unit is expired.");
		assert(unitMap.isUnitFocused(*unit) && "UnitState::updateUnitMap() the moving unit is not the focused unit in map.");

		unitMap.removeFocusedUnitIndexFromMap();
		break;

	case UnitState::State::MovingEnd:
		break;

	case UnitState::State::Waiting:
		assert(unit && "UnitState::updateUnitMap() the unit is expired.");
		if (unitMap.isUnitFocused(*unit)) {
			unitMap.setFocusedUnit(nullptr);
		}
		break;

	default:
		assert("UnitState::updateUnitMap() falls to default in switch(m_State). This should not happen.");
		break;
	}
}
