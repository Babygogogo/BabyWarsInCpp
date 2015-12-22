#include "EvtDataRequestChangeTurnPhase.h"

TurnPhaseTypeCode EvtDataRequestChangeTurnPhase::getTurnPhaseTypeCode() const
{
	return m_TypeCode;
}

const EventType EvtDataRequestChangeTurnPhase::s_EventType{ "EvtDataRequestChangeTurnPhase" };

const EventType & EvtDataRequestChangeTurnPhase::vGetType() const
{
	return s_EventType;
}
