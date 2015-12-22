#ifndef __UNIT_STATE__
#define __UNIT_STATE__

#include <vector>
#include <memory>

//Forward declaration.
class MovingAreaScript;
class UnitScript;
class GameCommand;
enum class UnitStateTypeCode;

class UnitState
{
public:
	virtual ~UnitState() = default;

	virtual UnitStateTypeCode vGetStateTypeCode() const = 0;

	virtual void vOnEnterState(UnitScript & unit) const = 0;
	virtual void vOnExitState(UnitScript & unit) const = 0;

	virtual bool vIsNeedFocusForUnitMap() const = 0;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const = 0;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const = 0;

	virtual bool vCanMoveAlongPath() const = 0;
	virtual bool vCanUndoMove() const = 0;

	virtual std::vector<GameCommand> vGenerateGameCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const = 0;
};

#endif // !__UNIT_STATE__
