#ifndef __EVENT_DATA_ACTIVATE_UNIT_END__
#define __EVENT_DATA_ACTIVATE_UNIT_END__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class UnitScript;

class EvtDataActivateUnitEnd : public BaseEventData
{
public:
	EvtDataActivateUnitEnd(std::weak_ptr<const UnitScript> && unit) : m_Unit{ std::move(unit) } {}
	virtual ~EvtDataActivateUnitEnd() = default;

	std::shared_ptr<const UnitScript> getUnit() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataActivateUnitEnd(const EvtDataActivateUnitEnd &) = delete;
	EvtDataActivateUnitEnd(EvtDataActivateUnitEnd &&) = delete;
	EvtDataActivateUnitEnd & operator=(const EvtDataActivateUnitEnd &) = delete;
	EvtDataActivateUnitEnd & operator=(EvtDataActivateUnitEnd &&) = delete;

private:
	std::weak_ptr<const UnitScript> m_Unit;
};

#endif // !__EVENT_DATA_ACTIVATE_UNIT_END__
