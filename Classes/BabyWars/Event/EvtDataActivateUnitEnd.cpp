#include "EvtDataActivateUnitEnd.h"

std::shared_ptr<const UnitScript> EvtDataActivateUnitEnd::getUnit() const
{
	if (m_Unit.expired())
		return nullptr;

	return m_Unit.lock();
}

const EventType EvtDataActivateUnitEnd::s_EventType{ "EvtDataActivateUnitEnd" };

const EventType & EvtDataActivateUnitEnd::vGetType() const
{
	return s_EventType;
}
