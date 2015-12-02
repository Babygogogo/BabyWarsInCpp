#ifndef __EVENT_DATA_UNIT_INDEX_CHANGE_END__
#define __EVENT_DATA_UNIT_INDEX_CHANGE_END__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class UnitScript;

class EvtDataUnitIndexChangeEnd : public BaseEventData
{
public:
	EvtDataUnitIndexChangeEnd(std::weak_ptr<const UnitScript> && unit) : m_Unit{ std::move(unit) } {}
	virtual ~EvtDataUnitIndexChangeEnd() = default;

	std::shared_ptr<const UnitScript> getUnit() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataUnitIndexChangeEnd(const EvtDataUnitIndexChangeEnd &) = delete;
	EvtDataUnitIndexChangeEnd(EvtDataUnitIndexChangeEnd &&) = delete;
	EvtDataUnitIndexChangeEnd & operator=(const EvtDataUnitIndexChangeEnd &) = delete;
	EvtDataUnitIndexChangeEnd & operator=(EvtDataUnitIndexChangeEnd &&) = delete;

private:
	std::weak_ptr<const UnitScript> m_Unit;
};

#endif // !__EVENT_DATA_UNIT_INDEX_CHANGE_END__
