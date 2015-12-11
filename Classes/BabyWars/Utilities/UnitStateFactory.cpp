#include <cassert>

#include "UnitStateTypeCode.h"
#include "InvalidUnitState.h"
#include "IdleUnitState.h"
#include "ActiveUnitState.h"
#include "MovingUnitState.h"
#include "MovingEndUnitState.h"
#include "WaitingUnitState.h"
#include "UnitStateFactory.h"

std::shared_ptr<UnitState> utilities::createUnitState(UnitStateTypeCode stateCode)
{
	//#NOTE: If you change the type of the unit states, reflect the changes here.
	switch (stateCode)
	{
	case UnitStateTypeCode::Invalid:
		return std::make_shared<InvalidUnitState>();
		break;

	case UnitStateTypeCode::Idle:
		return std::make_shared<IdleUnitState>();
		break;

	case UnitStateTypeCode::Active:
		return std::make_shared<ActiveUnitState>();
		break;

	case UnitStateTypeCode::Moving:
		return std::make_shared<MovingUnitState>();
		break;

	case UnitStateTypeCode::MovingEnd:
		return std::make_shared<MovingEndUnitState>();
		break;

	case UnitStateTypeCode::Waiting:
		return std::make_shared<WaitingUnitState>();
		break;

	default:
		assert("utilities::createUnitState() falls to default in switch (stateCode).This should not happen.");
		return nullptr;
		break;
	}
}
