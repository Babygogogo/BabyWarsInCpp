#include "EvtDataTurnPhaseChanged.h"

std::shared_ptr<TurnManagerScript> EvtDataTurnPhaseChanged::getTurnManagerScript() const
{
	if (m_TurnManagerScript.expired()) {
		return nullptr;
	}

	return m_TurnManagerScript.lock();
}

std::shared_ptr<TurnPhase> EvtDataTurnPhaseChanged::getCurrentTurnPhase() const
{
	if (m_CurrentTurnPhase.expired()) {
		return nullptr;
	}

	return m_CurrentTurnPhase.lock();
}

const EventType EvtDataTurnPhaseChanged::s_EventType{ "EvtDataTurnPhaseChanged" };

const EventType & EvtDataTurnPhaseChanged::vGetType() const
{
	return s_EventType;
}
