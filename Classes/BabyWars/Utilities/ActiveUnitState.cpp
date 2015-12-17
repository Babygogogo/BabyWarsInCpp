#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Actor/ActionComponent.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "ActiveUnitState.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActiveUnitStateImpl.
//////////////////////////////////////////////////////////////////////////
struct ActiveUnitState::ActiveUnitStateImpl
{
	ActiveUnitStateImpl();
	~ActiveUnitStateImpl();

	cocos2d::Action * m_Action{ nullptr };
};

ActiveUnitState::ActiveUnitStateImpl::ActiveUnitStateImpl()
{
	using namespace cocos2d;
	m_Action = RepeatForever::create(Sequence::create(RotateTo::create(0.2f, 30, 30), RotateTo::create(0.4f, -30, -30), RotateTo::create(0.2f, 0, 0), nullptr));
	m_Action->retain();
}

ActiveUnitState::ActiveUnitStateImpl::~ActiveUnitStateImpl()
{
	CC_SAFE_RELEASE_NULL(m_Action);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ActiveUnitScript.
//////////////////////////////////////////////////////////////////////////
ActiveUnitState::ActiveUnitState() : pimpl{ std::make_unique<ActiveUnitStateImpl>() }
{
}

ActiveUnitState::~ActiveUnitState()
{
}

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
		unitMap.undoMoveAndSetToIdleStateForFocusedUnit();
		unitMap.setFocusedUnit(unit);
	}
}

bool ActiveUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	if (touchedUnit.get() == &unit) {
		unit.moveInPlace();
	}
	else {
		unit.undoMoveAndSetToIdleState();
	}

	return true;
}

void ActiveUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->runAction(pimpl->m_Action);
}

void ActiveUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->stopAction(pimpl->m_Action);
	unit.getComponent<TransformComponent>()->setRotation(0);
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
