#include "EvtDataMakeMovePath.h"

const EventType EvtDataMakeMovePath::s_EventType{ "EvtDataMakeMovePath" };

const EventType & EvtDataMakeMovePath::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataMakeMovePath::getPosition() const
{
	return m_Position;
}
