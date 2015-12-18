#ifndef __EVENT_DATA__ALL_UNITS_UNFOCUSED__
#define __EVENT_DATA__ALL_UNITS_UNFOCUSED__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/PlayerID.h"

class EvtDataAllUnitsUnfocused : public BaseEventData
{
public:
	EvtDataAllUnitsUnfocused() = default;
	virtual ~EvtDataAllUnitsUnfocused() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataAllUnitsUnfocused(const EvtDataAllUnitsUnfocused &) = delete;
	EvtDataAllUnitsUnfocused(EvtDataAllUnitsUnfocused &&) = delete;
	EvtDataAllUnitsUnfocused & operator=(const EvtDataAllUnitsUnfocused &) = delete;
	EvtDataAllUnitsUnfocused & operator=(EvtDataAllUnitsUnfocused &&) = delete;
};

#endif // !__EVENT_DATA__ALL_UNITS_UNFOCUSED__
