#include "EvtDataActivateUnitAtPosition.h"

const EventType EvtDataActivateUnitAtPosition::s_EventType{ "EvtDataActivateUnitAtPosition" };

const EventType & EvtDataActivateUnitAtPosition::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataActivateUnitAtPosition::getPosition() const
{
	return m_Position;
}
