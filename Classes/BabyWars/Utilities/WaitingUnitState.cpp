#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "GameCommand.h"
#include "UnitStateTypeCode.h"
#include "WaitingUnitState.h"

UnitStateTypeCode WaitingUnitState::vGetStateTypeCode() const
{
	return UnitStateTypeCode::Waiting;
}

void WaitingUnitState::vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const
{
	movingArea.clearAreaForUnit(unit);
}

void WaitingUnitState::vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const
{
	commandList.clearListForUnit(unit);
}

void WaitingUnitState::vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const
{
	assert(unit && "UnitState::vUpdateUnitMap() the unit is expired.");
	if (unitMap.isUnitFocused(*unit)) {
		unitMap.setFocusedUnit(nullptr);
	}
}

bool WaitingUnitState::vUpdateUnitOnTouch(UnitScript & unit) const
{
	return false;
}

void WaitingUnitState::vShowUnitAppearanceInState(UnitScript & unit) const
{
	unit.getRenderComponent()->getSceneNode()->setColor(cocos2d::Color3B(150, 150, 150));
}

void WaitingUnitState::vClearUnitAppearanceInState(UnitScript & unit) const
{
	unit.getRenderComponent()->getSceneNode()->setColor(cocos2d::Color3B(255, 255, 255));
}

bool WaitingUnitState::vCanMoveAlongPath() const
{
	return false;
}

bool WaitingUnitState::vCanUndoMove() const
{
	return false;
}

std::vector<GameCommand> WaitingUnitState::vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const
{
	return{};
}
