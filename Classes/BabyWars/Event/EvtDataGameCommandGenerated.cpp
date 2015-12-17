#include "EvtDataGameCommandGenerated.h"

std::shared_ptr<const std::vector<GameCommand>> EvtDataGameCommandGenerated::getGameCommands() const
{
	if (m_GameCommands.expired()) {
		return nullptr;
	}

	return m_GameCommands.lock();
}

const EventType EvtDataGameCommandGenerated::s_EventType{ "EvtDataGameCommandGenerated" };

const EventType & EvtDataGameCommandGenerated::vGetType() const
{
	return s_EventType;
}
