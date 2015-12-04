#ifndef __EVENT_DATA_REQUEST_CHANGE_UNIT_STATE__
#define __EVENT_DATA_REQUEST_CHANGE_UNIT_STATE__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/UnitState.h"

class UnitScript;

class EvtDataRequestChangeUnitState : public BaseEventData
{
public:
	EvtDataRequestChangeUnitState(std::weak_ptr<UnitScript> unitScript, UnitState newState) : m_UnitScript{ std::move(unitScript) }, m_NewState{ newState } {}
	virtual ~EvtDataRequestChangeUnitState() = default;

	std::shared_ptr<UnitScript> getUnitScript() const;
	UnitState getNewState() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataRequestChangeUnitState(const EvtDataRequestChangeUnitState &) = delete;
	EvtDataRequestChangeUnitState(EvtDataRequestChangeUnitState &&) = delete;
	EvtDataRequestChangeUnitState & operator=(const EvtDataRequestChangeUnitState &) = delete;
	EvtDataRequestChangeUnitState & operator=(EvtDataRequestChangeUnitState &&) = delete;

private:
	std::weak_ptr<UnitScript> m_UnitScript;
	UnitState m_NewState;
};

#endif // !__EVENT_DATA_REQUEST_CHANGE_UNIT_STATE__
