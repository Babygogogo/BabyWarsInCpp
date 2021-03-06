#include "EvtDataBeginTurn.h"

int EvtDataBeginTurn::getTurnIndex() const
{
	return m_TurnIndex;
}

PlayerID EvtDataBeginTurn::getPlayerID() const
{
	return m_PlayerID;
}

const EventType EvtDataBeginTurn::s_EventType{ "EvtDataBeginTurn" };

const EventType & EvtDataBeginTurn::vGetType() const
{
	return s_EventType;
}
