#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/ActionComponent.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/UnitScript.h"
#include "GridIndex.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "MovingEndUnitState.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingEndUnitStateImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingEndUnitState::MovingEndUnitStateImpl
{
	MovingEndUnitStateImpl();
	~MovingEndUnitStateImpl();

	void showUnitAppearanceInState(UnitScript & unit) const;
	void clearUnitAppearanceInState(UnitScript & unit) const;

	cocos2d::Action * m_Action{ nullptr };
};

MovingEndUnitState::MovingEndUnitStateImpl::MovingEndUnitStateImpl()
{
	using namespace cocos2d;

	m_Action = RepeatForever::create(Sequence::create(FadeTo::create(0.4f, 100), FadeTo::create(0.4f, 255), nullptr));
	m_Action->retain();
}

MovingEndUnitState::MovingEndUnitStateImpl::~MovingEndUnitStateImpl()
{
	CC_SAFE_RELEASE_NULL(m_Action);
}

void MovingEndUnitState::MovingEndUnitStateImpl::showUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->runAction(m_Action);
}

void MovingEndUnitState::MovingEndUnitStateImpl::clearUnitAppearanceInState(UnitScript & unit) const
{
	unit.getComponent<ActionComponent>()->stopAction(m_Action);
	unit.getRenderComponent()->getSceneNode()->setOpacity(255);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MovingEndUnitState.
//////////////////////////////////////////////////////////////////////////
MovingEndUnitState::MovingEndUnitState() : pimpl{ std::make_unique<MovingEndUnitStateImpl>() }
{
}

MovingEndUnitState::~MovingEndUnitState()
{
}

UnitStateTypeCode MovingEndUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::MovingEnd;
}

void MovingEndUnitState::onUnitEnterState(UnitScript & unit) const
{
	pimpl->showUnitAppearanceInState(unit);
}

void MovingEndUnitState::onUnitExitState(UnitScript & unit) const
{
	pimpl->clearUnitAppearanceInState(unit);
}

void MovingEndUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

void MovingEndUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
}

bool MovingEndUnitState::vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const
{
	unit.undoMoveAndSetToIdleState();
	return true;
}

bool MovingEndUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool MovingEndUnitState::vCanUndoMove() const
{
	return true;
}

std::vector<GameCommand> MovingEndUnitState::vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	assert(targetUnit && "UnitState::vGenerateGameCommandsForUnit() the target unit is expired.");
	auto commands = std::vector<GameCommand>{};
	auto unitScript = std::weak_ptr<UnitScript>(targetUnit);

	commands.emplace_back("Wait", [unitScript = unitScript]() {
		if (unitScript.expired()) {
			return;
		}

		auto strongUnit = unitScript.lock();
		strongUnit->setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Waiting);
		strongUnit->setGridIndexAndPosition(strongUnit->getGridIndex());
	});
	return commands;
}
