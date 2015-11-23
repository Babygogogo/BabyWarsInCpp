#include "EvtDataDeactivateUnitEnd.h"

std::shared_ptr<const UnitScript> EvtDataDeactivateUnitEnd::getUnit() const
{
	if (m_Unit.expired())
		return nullptr;

	return m_Unit.lock();
}

const EventType EvtDataDeactivateUnitEnd::s_EventType{ "EvtDataDectivateUnitEnd" };

const EventType & EvtDataDeactivateUnitEnd::vGetType() const
{
	return s_EventType;
}
