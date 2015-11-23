#include "EvtDataMakeMovingPathBegin.h"

const GridIndex & EvtDataMakeMovingPathBegin::getGridIndex() const
{
	return m_GridIndex;
}

const EventType EvtDataMakeMovingPathBegin::s_EventType{ "EvtDataMakeMovingPathBegin" };

const EventType & EvtDataMakeMovingPathBegin::vGetType() const
{
	return s_EventType;
}
