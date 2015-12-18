#ifndef __INVALID_UNIT_STATE__
#define __INVALID_UNIT_STATE__

#include "UnitState.h"

class InvalidUnitState : public UnitState
{
public:
	InvalidUnitState() = default;
	~InvalidUnitState() = default;

	//Disable copy/move constructor and operator=.
	InvalidUnitState(const InvalidUnitState &) = delete;
	InvalidUnitState(InvalidUnitState &&) = delete;
	InvalidUnitState & operator=(const InvalidUnitState &) = delete;
	InvalidUnitState & operator=(InvalidUnitState &&) = delete;

private:
	virtual UnitStateTypeCode vGetStateTypeCode() const override;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const override;
	virtual void vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const override;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const override;
	virtual void vShowUnitAppearanceInState(UnitScript & unit) const override;
	virtual void vClearUnitAppearanceInState(UnitScript & unit) const override;

	virtual bool vCanMoveAlongPath() const override;
	virtual bool vCanUndoMove() const override;

	virtual std::vector<GameCommand> vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const override;
};

#endif // !__INVALID_UNIT_STATE__
