#ifndef __UNIT_STATE__
#define __UNIT_STATE__

#include <vector>
#include <memory>

//Forward declaration.
class MovingAreaScript;
class CommandListScript;
class UnitMapScript;
class UnitScript;
class GameCommand;
enum class UnitStateTypeCode;

class UnitState
{
public:
	virtual ~UnitState() = default;

	virtual UnitStateTypeCode vGetStateTypeCode() const = 0;

	void onUnitEnterState(UnitScript & unit) const;
	void onUnitExitState(UnitScript & unit) const;

	virtual void vUpdateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const = 0;
	virtual void vUpdateCommandList(CommandListScript & commandList, const UnitScript & unit) const = 0;
	virtual void vUpdateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const = 0;

	virtual bool vUpdateUnitOnTouch(UnitScript & unit, const std::shared_ptr<UnitScript> & touchedUnit) const = 0;
	virtual void vShowUnitAppearanceInState(UnitScript & unit) const = 0;
	virtual void vClearUnitAppearanceInState(UnitScript & unit) const = 0;

	virtual bool vCanMoveAlongPath() const = 0;
	virtual bool vCanUndoMove() const = 0;

	virtual std::vector<GameCommand> vGetCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const = 0;
};

#endif // !__UNIT_STATE__
