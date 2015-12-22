#ifndef __WAITING_UNIT_STATE__
#define __WAITING_UNIT_STATE__

#include "UnitState.h"

class WaitingUnitState : public UnitState
{
public:
	WaitingUnitState();
	~WaitingUnitState();

	//Disable copy/move constructor and operator=.
	WaitingUnitState(const WaitingUnitState &) = delete;
	WaitingUnitState(WaitingUnitState &&) = delete;
	WaitingUnitState & operator=(const WaitingUnitState &) = delete;
	WaitingUnitState & operator=(WaitingUnitState &&) = delete;

private:
	virtual UnitStateTypeCode vGetStateTypeCode() const override;

	virtual void vOnEnterState(UnitScript & unit) const override;
	virtual void vOnExitState(UnitScript & unit) const override;

	virtual bool vIsNeedFocusForUnitMap() const override;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const override;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const override;

	virtual bool vCanMoveAlongPath() const override;
	virtual bool vCanUndoMove() const override;

	virtual std::vector<GameCommand> vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const override;

	//Implementation stuff.
	struct WaitingUnitStateImpl;
	std::unique_ptr<WaitingUnitStateImpl> pimpl;
};

#endif // !__WAITING_UNIT_STATE__
