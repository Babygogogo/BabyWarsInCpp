#ifndef __EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__
#define __EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class UnitScript;

class EvtDataDeactivateUnitEnd : public BaseEventData
{
public:
	EvtDataDeactivateUnitEnd(std::weak_ptr<const UnitScript> && unit) : m_Unit{ std::move(unit) } {}
	virtual ~EvtDataDeactivateUnitEnd() = default;

	std::shared_ptr<const UnitScript> getUnit() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataDeactivateUnitEnd(const EvtDataDeactivateUnitEnd &) = delete;
	EvtDataDeactivateUnitEnd(EvtDataDeactivateUnitEnd &&) = delete;
	EvtDataDeactivateUnitEnd & operator=(const EvtDataDeactivateUnitEnd &) = delete;
	EvtDataDeactivateUnitEnd & operator=(EvtDataDeactivateUnitEnd &&) = delete;

private:
	std::weak_ptr<const UnitScript> m_Unit;
};

#endif // !__EVENT_DATA_DEACTIVATE_ACTIVE_UNIT__
