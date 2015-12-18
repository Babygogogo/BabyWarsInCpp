#ifndef __EVENT_DATA_BEGIN_TURN_EFFECT_DISAPPEARED__
#define __EVENT_DATA_BEGIN_TURN_EFFECT_DISAPPEARED__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/PlayerID.h"

class EvtDataBeginTurnEffectDisappeared : public BaseEventData
{
public:
	EvtDataBeginTurnEffectDisappeared() = default;
	virtual ~EvtDataBeginTurnEffectDisappeared() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataBeginTurnEffectDisappeared(const EvtDataBeginTurnEffectDisappeared &) = delete;
	EvtDataBeginTurnEffectDisappeared(EvtDataBeginTurnEffectDisappeared &&) = delete;
	EvtDataBeginTurnEffectDisappeared & operator=(const EvtDataBeginTurnEffectDisappeared &) = delete;
	EvtDataBeginTurnEffectDisappeared & operator=(EvtDataBeginTurnEffectDisappeared &&) = delete;
};

#endif // !__EVENT_DATA_BEGIN_TURN_EFFECT_DISAPPEARED__
