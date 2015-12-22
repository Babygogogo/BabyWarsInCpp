#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Actor/ActionComponent.h"
#include "../Script/MovingAreaScript.h"
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

	void showUnitAppearanceInState(UnitScript & unit) const;
	void clearUnitAppearanceInState(UnitScript & unit) const;

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

void ActiveUnitState::ActiveUnitStateImpl::showUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->runAction(m_Action);
}

void ActiveUnitState::ActiveUnitStateImpl::clearUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->stopAction(m_Action);
	unit.getComponent<TransformComponent>()->setRotation(0);
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

void ActiveUnitState::vOnEnterState(UnitScript & unit) const
{
	pimpl->showUnitAppearanceInState(unit);
}

void ActiveUnitState::vOnExitState(UnitScript & unit) const
{
	pimpl->clearUnitAppearanceInState(unit);
}

bool ActiveUnitState::vIsNeedFocusForUnitMap() const
{
	return true;
}

void ActiveUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.showAreaForUnit(unit);
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

bool ActiveUnitState::vCanMoveAlongPath() const
{
	return true;
}

bool ActiveUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> ActiveUnitState::vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
