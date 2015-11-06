#include "EvtDataDragScene.h"

const EventType EvtDataDragScene::s_EventType{ "EvtDataDragScene" };

const EventType & EvtDataDragScene::vGetType() const
{
	return s_EventType;
}

const cocos2d::Vec2 & EvtDataDragScene::getOffset() const
{
	return m_Offset;
}
