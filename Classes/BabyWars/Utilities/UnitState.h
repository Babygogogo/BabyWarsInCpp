#ifndef __UNIT_STATE__
#define __UNIT_STATE__

//Forward declaration.
class MovingAreaScript;
class CommandListScript;
class UnitMapScript;
class UnitScript;

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

	UnitState() = default;
	UnitState(State state) : m_State{ state } {}
	~UnitState() = default;

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

private:
	State m_State{ State::Invalid };
};

#endif // !__UNIT_STATE__
