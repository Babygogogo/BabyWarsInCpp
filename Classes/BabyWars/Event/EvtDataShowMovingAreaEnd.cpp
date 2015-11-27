#include "EvtDataShowMovingAreaEnd.h"

std::weak_ptr<const MovingArea> EvtDataShowMovingAreaEnd::getMovingArea() const
{
	return m_MovingArea;
}

const EventType EvtDataShowMovingAreaEnd::s_EventType{ "EvtDataShowMovingAreaEnd" };

const EventType & EvtDataShowMovingAreaEnd::vGetType() const
{
	return s_EventType;
}
