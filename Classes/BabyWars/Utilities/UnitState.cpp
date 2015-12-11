#include <cassert>

#include "../Script/MovingAreaScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/GameCommand.h"
#include "UnitState.h"

UnitState::UnitState()
{
}

UnitState::UnitState(State state) : m_State{ state }
{
}

UnitState::~UnitState()
{
}

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

bool UnitState::updateUnitOnTouch(UnitScript & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::updateUnit() the unit state is invalid.");
		return false;
		break;

	case UnitState::State::Idle:
		unit.setStateAndAppearanceAndQueueEvent(UnitState(State::Active));
		return true;
		break;

	case UnitState::State::Active:
		unit.moveInPlace();
		return true;
		break;

	case UnitState::State::Moving:
		return false;
		break;

	case UnitState::State::MovingEnd:
		return false;
		break;

	case UnitState::State::Waiting:
		return false;
		break;

	default:
		assert("UnitState::updateUnitOnTouch() falls to default in switch(m_State). This should not happen.");
		return false;
		break;
	}
}

void UnitState::showUnitAppearanceInState(UnitScript & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::showUnitAppearanceInState() the unit state is invalid.");
		break;

	case UnitState::State::Idle:
		break;

	case UnitState::State::Active:
		unit.showAppearanceInActiveState();
		break;

	case UnitState::State::Moving:
		unit.showAppearanceInMovingState();
		break;

	case UnitState::State::MovingEnd:
		unit.showAppearanceInMovingEndState();
		break;

	case UnitState::State::Waiting:
		unit.showAppearanceInWaitingState();
		break;

	default:
		assert("UnitState::showUnitAppearanceInState() falls to default in switch(m_State). This should not happen.");
		break;
	}
}

void UnitState::clearUnitAppearanceInState(UnitScript & unit) const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::clearUnitAppearanceInState() the unit state is invalid.");
		break;

	case UnitState::State::Idle:
		break;

	case UnitState::State::Active:
		unit.clearAppearanceInActiveState();
		break;

	case UnitState::State::Moving:
		unit.clearAppearanceInMovingState();
		break;

	case UnitState::State::MovingEnd:
		unit.clearAppearanceInMovingEndState();
		break;

	case UnitState::State::Waiting:
		unit.clearAppearanceInWaitingState();
		break;

	default:
		assert("UnitState::clearUnitAppearanceInState() falls to default in switch(m_State). This should not happen.");
		break;
	}
}

bool UnitState::canMoveAlongPath() const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::canMoveAlongPath() the unit state is invalid.");
		return false;
		break;

	case UnitState::State::Idle:
		return false;
		break;

	case UnitState::State::Active:
		return true;
		break;

	case UnitState::State::Moving:
		return false;
		break;

	case UnitState::State::MovingEnd:
		return false;
		break;

	case UnitState::State::Waiting:
		return false;
		break;

	default:
		assert("UnitState::canMoveAlongPath() falls to default in switch(m_State).This should not happen.");
		return false;
		break;
	}
}

bool UnitState::canUndoMove() const
{
	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::canUndoMove() the unit state is invalid.");
		return false;
		break;

	case UnitState::State::Idle:
		return false;
		break;

	case UnitState::State::Active:
		return true;
		break;

	case UnitState::State::Moving:
		return true;
		break;

	case UnitState::State::MovingEnd:
		return true;
		break;

	case UnitState::State::Waiting:
		return false;
		break;

	default:
		assert("UnitState::canUndoMove() falls to default in switch(m_State). This should not happen.");
		return false;
		break;
	}
}

std::vector<GameCommand> UnitState::getCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	auto commands = std::vector<GameCommand>{};
	auto unitScript = std::weak_ptr<UnitScript>(targetUnit);

	switch (m_State)
	{
	case UnitState::State::Invalid:
		assert("UnitState::getCommandsForUnit() the unit state is invalid.");
		return commands;
		break;

	case UnitState::State::Idle:
		return commands;
		break;

	case UnitState::State::Active:
		return commands;
		break;

	case UnitState::State::Moving:
		return commands;
		break;

	case UnitState::State::MovingEnd:
		assert(targetUnit && "UnitState::getCommandsForUnit() the target unit is expired.");
		commands.emplace_back("Wait", [unitScript = unitScript]() {
			if (unitScript.expired()) {
				return;
			}

			auto strongUnit = unitScript.lock();
			strongUnit->setStateAndAppearanceAndQueueEvent(UnitState(State::Waiting));
			strongUnit->setGridIndexAndPosition(strongUnit->getGridIndex());
		});
		return commands;
		break;

	case UnitState::State::Waiting:
		return commands;
		break;

	default:
		assert("UnitState::getCommandsForUnit() falls to default in switch(m_State). This should not happen.");
		return commands;
		break;
	}
}
