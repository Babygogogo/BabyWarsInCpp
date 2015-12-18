#include "EvtDataTurnEnded.h"

int EvtDataTurnEnded::getTurnIndex() const
{
	return m_TurnIndex;
}

PlayerID EvtDataTurnEnded::getPlayerID() const
{
	return m_PlayerID;
}

const EventType EvtDataTurnEnded::s_EventType{ "EvtDataTurnEnded" };

const EventType & EvtDataTurnEnded::vGetType() const
{
	return s_EventType;
}
