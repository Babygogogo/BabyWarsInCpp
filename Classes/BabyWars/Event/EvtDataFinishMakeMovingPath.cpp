#include "EvtDataFinishMakeMovingPath.h"

const EventType EvtDataFinishMakeMovingPath::s_EventType{ "EvtDataFinishMakeMovePath" };

const EventType & EvtDataFinishMakeMovingPath::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataFinishMakeMovingPath::getPosition() const
{
	return m_Position;
}
