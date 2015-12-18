#ifndef __MOVING_UNIT_STATE__
#define __MOVING_UNIT_STATE__

#include "UnitState.h"

class MovingUnitState : public UnitState
{
public:
	MovingUnitState() = default;
	~MovingUnitState() = default;

	//Disable copy/move constructor and operator=.
	MovingUnitState(const MovingUnitState &) = delete;
	MovingUnitState(MovingUnitState &&) = delete;
	MovingUnitState & operator=(const MovingUnitState &) = delete;
	MovingUnitState & operator=(MovingUnitState &&) = delete;

private:
	virtual UnitStateTypeCode vGetStateTypeCode() const override;

	virtual void onUnitEnterState(UnitScript & unit) const override;
	virtual void onUnitExitState(UnitScript & unit) const override;

	virtual bool vIsNeedFocusForUnitMap() const override;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const override;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const override;

	virtual bool vCanMoveAlongPath() const override;
	virtual bool vCanUndoMove() const override;

	virtual std::vector<GameCommand> vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const override;
};

#endif // !__MOVING_UNIT_STATE__
