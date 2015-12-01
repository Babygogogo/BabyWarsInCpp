#include "EvtDataUnitStateChangeEnd.h"

const std::weak_ptr<const UnitScript> & EvtDataUnitStateChangeEnd::getUnitScript() const
{
	return m_UnitScript;
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
