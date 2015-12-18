#ifndef __IDLE_UNIT_STATE__
#define __IDLE_UNIT_STATE__

#include "UnitState.h"

class IdleUnitState : public UnitState
{
public:
	IdleUnitState() = default;
	~IdleUnitState() = default;

	//Disable copy/move constructor and operator=.
	IdleUnitState(const IdleUnitState &) = delete;
	IdleUnitState(IdleUnitState &&) = delete;
	IdleUnitState & operator=(const IdleUnitState &) = delete;
	IdleUnitState & operator=(IdleUnitState &&) = delete;

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

#endif // !__IDLE_UNIT_STATE__
