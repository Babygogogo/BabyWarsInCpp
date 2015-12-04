#include "EvtDataRequestChangeUnitState.h"

std::shared_ptr<UnitScript> EvtDataRequestChangeUnitState::getUnitScript() const
{
	if (m_UnitScript.expired())
		return nullptr;

	return m_UnitScript.lock();
}

UnitState EvtDataRequestChangeUnitState::getNewState() const
{
	return m_NewState;
}

const EventType EvtDataRequestChangeUnitState::s_EventType{ "EvtDataRequestChangeUnitState" };

const EventType & EvtDataRequestChangeUnitState::vGetType() const
{
	return s_EventType;
}
