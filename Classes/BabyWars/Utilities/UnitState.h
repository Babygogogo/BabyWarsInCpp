#ifndef __UNIT_STATE__
#define __UNIT_STATE__

#include <vector>

//Forward declaration.
class MovingAreaScript;
class CommandListScript;
class UnitMapScript;
class UnitScript;
class BaseRenderComponent;
class GameCommand;

class UnitState
{
public:
	enum class State {
		Invalid,
		Idle,
		Active,
		Moving,
		MovingEnd,
		Waiting
	};

	UnitState();
	UnitState(State state);
	~UnitState();

	State getState() const
	{
		return m_State;
	}

	bool operator==(const UnitState & rhs) const
	{
		return m_State == rhs.m_State;
	}

	void updateMovingArea(MovingAreaScript & movingArea, const UnitScript & unit) const;
	void updateCommandList(CommandListScript & commandList, const UnitScript & unit) const;
	void updateUnitMap(UnitMapScript & unitMap, const std::shared_ptr<UnitScript> & unit) const;

	bool updateUnitOnTouch(UnitScript & unit) const;
	void showUnitAppearanceInState(UnitScript & unit) const;
	void clearUnitAppearanceInState(UnitScript & unit) const;

	bool canMoveAlongPath() const;
	bool canUndoMove() const;

	std::vector<GameCommand> getCommandsForUnit(const std::shared_ptr<UnitScript> & targetUnit) const;

private:
	State m_State{ State::Invalid };
};

#endif // !__UNIT_STATE__
