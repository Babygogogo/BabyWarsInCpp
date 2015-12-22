#ifndef __EVENT_DATA_TURN_PHASE_CHANGED__
#define __EVENT_DATA_TURN_PHASE_CHANGED__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class TurnManagerScript;
class TurnPhase;

class EvtDataTurnPhaseChanged : public BaseEventData
{
public:
	EvtDataTurnPhaseChanged(std::weak_ptr<TurnManagerScript> turnManagerScript, std::weak_ptr<TurnPhase> turnPhase)
		: m_TurnManagerScript{ std::move(turnManagerScript) }, m_CurrentTurnPhase{ std::move(turnPhase) } {}
	virtual ~EvtDataTurnPhaseChanged() = default;

	std::shared_ptr<TurnManagerScript> getTurnManagerScript() const;
	std::shared_ptr<TurnPhase> getCurrentTurnPhase() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataTurnPhaseChanged(const EvtDataTurnPhaseChanged &) = delete;
	EvtDataTurnPhaseChanged(EvtDataTurnPhaseChanged &&) = delete;
	EvtDataTurnPhaseChanged & operator=(const EvtDataTurnPhaseChanged &) = delete;
	EvtDataTurnPhaseChanged & operator=(EvtDataTurnPhaseChanged &&) = delete;

private:
	std::weak_ptr<TurnManagerScript> m_TurnManagerScript;
	std::weak_ptr<TurnPhase> m_CurrentTurnPhase;
};

#endif // !__EVENT_DATA_TURN_PHASE_CHANGED__
