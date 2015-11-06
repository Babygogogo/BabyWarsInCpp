#include "EvtDataFinishMakeMovePath.h"

const EventType EvtDataFinishMakeMovePath::s_EventType{ "EvtDataFinishMakeMovePath" };

const EventType & EvtDataFinishMakeMovePath::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataFinishMakeMovePath::getPosition() const
{
	return m_Position;
}
