#include "EvtDataRequestChangeUnitState.h"

std::shared_ptr<UnitScript> EvtDataRequestChangeUnitState::getUnitScript() const
{
	if (m_UnitScript.expired()) {
		return nullptr;
	}

	return m_UnitScript.lock();
}

UnitStateTypeCode EvtDataRequestChangeUnitState::getNewStateCode() const
{
	return m_NewStateCode;
}

const EventType EvtDataRequestChangeUnitState::s_EventType{ "EvtDataRequestChangeUnitState" };

const EventType & EvtDataRequestChangeUnitState::vGetType() const
{
	return s_EventType;
}
