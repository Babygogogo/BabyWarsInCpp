#ifndef __EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__
#define __EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataDeactivateActiveUnit : public BaseEventData
{
public:
	EvtDataDeactivateActiveUnit() = default;
	virtual ~EvtDataDeactivateActiveUnit() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataDeactivateActiveUnit(const EvtDataDeactivateActiveUnit &) = delete;
	EvtDataDeactivateActiveUnit(EvtDataDeactivateActiveUnit &&) = delete;
	EvtDataDeactivateActiveUnit & operator=(const EvtDataDeactivateActiveUnit &) = delete;
	EvtDataDeactivateActiveUnit & operator=(EvtDataDeactivateActiveUnit &&) = delete;

private:
};

#endif // !__EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__
