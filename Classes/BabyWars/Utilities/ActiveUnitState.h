#ifndef __ACTIVE_UNIT_STATE__
#define __ACTIVE_UNIT_STATE__

#include "UnitState.h"

class ActiveUnitState : public UnitState
{
public:
	ActiveUnitState() = default;
	~ActiveUnitState() = default;

	//Disable copy/move constructor and operator=.
	ActiveUnitState(const ActiveUnitState &) = delete;
	ActiveUnitState(ActiveUnitState &&) = delete;
	ActiveUnitState & operator=(const ActiveUnitState &) = delete;
	ActiveUnitState & operator=(ActiveUnitState &&) = delete;

private:
	virtual UnitStateTypeCode vGetStateTypeCode() const override;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const override;
	virtual void vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const override;
	virtual void vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const override;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit) const override;
	virtual void vShowUnitAppearanceInState(UnitScript & unit) const override;
	virtual void vClearUnitAppearanceInState(UnitScript & unit) const override;

	virtual bool vCanMoveAlongPath() const override;
	virtual bool vCanUndoMove() const override;

	virtual std::vector<GameCommand> vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const override;
};

#endif // !__ACTIVE_UNIT_STATE__
