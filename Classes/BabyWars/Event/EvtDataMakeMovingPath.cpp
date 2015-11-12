#include "EvtDataMakeMovingPath.h"

const EventType EvtDataMakeMovingPath::s_EventType{ "EvtDataMakeMovePath" };

const EventType & EvtDataMakeMovingPath::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataMakeMovingPath::getPosition() const
{
	return m_Position;
}
