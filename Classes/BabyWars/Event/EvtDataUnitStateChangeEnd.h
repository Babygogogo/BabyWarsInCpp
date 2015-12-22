#ifndef __EVENT_DATA_UNIT_STATE_CHANGE_END__
#define __EVENT_DATA_UNIT_STATE_CHANGE_END__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class UnitScript;
class UnitState;

class EvtDataUnitStateChangeEnd : public BaseEventData
{
public:
	EvtDataUnitStateChangeEnd(std::weak_ptr<UnitScript> unitScript, std::weak_ptr<UnitState> currentState)
		: m_UnitScript{ std::move(unitScript) }, m_CurrentState{ std::move(currentState) } {}
	virtual ~EvtDataUnitStateChangeEnd() = default;

	std::shared_ptr<UnitScript> getUnitScript() const;
	std::shared_ptr<UnitState> getCurrentState() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataUnitStateChangeEnd(const EvtDataUnitStateChangeEnd &) = delete;
	EvtDataUnitStateChangeEnd(EvtDataUnitStateChangeEnd &&) = delete;
	EvtDataUnitStateChangeEnd & operator=(const EvtDataUnitStateChangeEnd &) = delete;
	EvtDataUnitStateChangeEnd & operator=(EvtDataUnitStateChangeEnd &&) = delete;

private:
	std::weak_ptr<UnitScript> m_UnitScript;
	std::weak_ptr<UnitState> m_CurrentState;
};

#endif // !__EVENT_DATA_UNIT_STATE_CHANGE_END__
