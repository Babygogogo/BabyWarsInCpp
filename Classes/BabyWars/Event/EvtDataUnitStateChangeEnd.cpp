#include "EvtDataUnitStateChangeEnd.h"

std::shared_ptr<UnitScript> EvtDataUnitStateChangeEnd::getUnitScript() const
{
	if (m_UnitScript.expired())
		return nullptr;

	return m_UnitScript.lock();
}

std::shared_ptr<UnitState> EvtDataUnitStateChangeEnd::getCurrentState() const
{
	if (m_CurrentState.expired()) {
		return nullptr;
	}

	return m_CurrentState.lock();
}

const EventType EvtDataUnitStateChangeEnd::s_EventType{ "EvtDataUnitStateChangeEnd" };

const EventType & EvtDataUnitStateChangeEnd::vGetType() const
{
	return s_EventType;
}
