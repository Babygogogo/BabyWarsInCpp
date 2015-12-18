#include "EvtDataGameCommandGenerated.h"
#include "../Utilities/GameCommand.h"

const std::vector<GameCommand> & EvtDataGameCommandGenerated::getGameCommands() const
{
	return m_GameCommands;
}

const EventType EvtDataGameCommandGenerated::s_EventType{ "EvtDataGameCommandGenerated" };

const EventType & EvtDataGameCommandGenerated::vGetType() const
{
	return s_EventType;
}
