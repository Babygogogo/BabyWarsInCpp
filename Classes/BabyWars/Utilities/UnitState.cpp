#include <cassert>

#include "../Script/MovingAreaScript.h"
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
