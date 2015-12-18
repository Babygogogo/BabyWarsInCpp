#ifndef __MOVING_END_UNIT_STATE__
#define __MOVING_END_UNIT_STATE__

#include "UnitState.h"

class MovingEndUnitState : public UnitState
{
public:
	MovingEndUnitState();
	~MovingEndUnitState();

	//Disable copy/move constructor and operator=.
	MovingEndUnitState(const MovingEndUnitState &) = delete;
	MovingEndUnitState(MovingEndUnitState &&) = delete;
	MovingEndUnitState & operator=(const MovingEndUnitState &) = delete;
	MovingEndUnitState & operator=(MovingEndUnitState &&) = delete;

private:
	virtual UnitStateTypeCode vGetStateTypeCode() const override;

	virtual void onUnitEnterState(UnitScript & unit) const override;
	virtual void onUnitExitState(UnitScript & unit) const override;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const override;
	virtual void vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const override;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const override;

	virtual bool vCanMoveAlongPath() const override;
	virtual bool vCanUndoMove() const override;

	virtual std::vector<GameCommand> vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const override;

	//Implementation stuff.
	struct MovingEndUnitStateImpl;
	std::unique_ptr<MovingEndUnitStateImpl> pimpl;
};

#endif // !__MOVING_END_UNIT_STATE__
