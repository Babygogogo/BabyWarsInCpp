#ifndef __EVENT_DATA_REQUEST_CHANGE_TURN_PHASE__
#define __EVENT_DATA_REQUEST_CHANGE_TURN_PHASE__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/TurnPhaseTypeCode.h"

class EvtDataRequestChangeTurnPhase : public BaseEventData
{
public:
	EvtDataRequestChangeTurnPhase(TurnPhaseTypeCode typeCode) : m_TypeCode{ typeCode } {}
	virtual ~EvtDataRequestChangeTurnPhase() = default;

	TurnPhaseTypeCode getTurnPhaseTypeCode() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataRequestChangeTurnPhase(const EvtDataRequestChangeTurnPhase &) = delete;
	EvtDataRequestChangeTurnPhase(EvtDataRequestChangeTurnPhase &&) = delete;
	EvtDataRequestChangeTurnPhase & operator=(const EvtDataRequestChangeTurnPhase &) = delete;
	EvtDataRequestChangeTurnPhase & operator=(EvtDataRequestChangeTurnPhase &&) = delete;

private:
	TurnPhaseTypeCode m_TypeCode{ TurnPhaseTypeCode::Invalid };
};

#endif // !__EVENT_DATA_REQUEST_CHANGE_TURN_PHASE__
