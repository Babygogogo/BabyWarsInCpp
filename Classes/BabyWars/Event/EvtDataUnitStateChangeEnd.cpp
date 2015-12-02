#include "EvtDataUnitStateChangeEnd.h"

std::shared_ptr<UnitScript> EvtDataUnitStateChangeEnd::getUnitScript() const
{
	return m_UnitScript.lock();
}

UnitState EvtDataUnitStateChangeEnd::getPreviousState() const
{
	return m_PreviousState;
}

UnitState EvtDataUnitStateChangeEnd::getCurrentState() const
{
	return m_CurrentState;
}

const EventType EvtDataUnitStateChangeEnd::s_EventType{ "EvtDataUnitStateChangeEnd" };

const EventType & EvtDataUnitStateChangeEnd::vGetType() const
{
	return s_EventType;
}
