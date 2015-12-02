#include "EvtDataMakeMovingPathEnd.h"

const MovingPath & EvtDataMakeMovingPathEnd::getMovingPath() const
{
	return m_MovingPath;
}

const EventType EvtDataMakeMovingPathEnd::s_EventType{ "EvtDataMakeMovingPathEnd" };

const EventType & EvtDataMakeMovingPathEnd::vGetType() const
{
	return s_EventType;
}
