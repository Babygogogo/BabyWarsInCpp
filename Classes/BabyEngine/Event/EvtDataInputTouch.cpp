#include "cocos2d.h"

#include "EvtDataInputTouch.h"

//////////////////////////////////////////////////////////////////////////
//Definition of EvtDataInputTouchImpl.
//////////////////////////////////////////////////////////////////////////
struct EvtDataInputTouch::EvtDataInputTouchImpl
{
	EvtDataInputTouchImpl(ActorID actorID, const cocos2d::Vec2 & positionInWorld) : m_ActorID{ actorID }, m_PositionInWorld{ positionInWorld } {}
	~EvtDataInputTouchImpl() = default;

	ActorID m_ActorID{ INVALID_ACTOR_ID };
	cocos2d::Vec2 m_PositionInWorld;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of EvtDataInputTouch.
//////////////////////////////////////////////////////////////////////////
EvtDataInputTouch::EvtDataInputTouch(ActorID actorID, const cocos2d::Vec2 & positionInWorld) : pimpl{ std::make_unique<EvtDataInputTouchImpl>(actorID, positionInWorld) }
{
}

EvtDataInputTouch::~EvtDataInputTouch()
{
}

ActorID EvtDataInputTouch::getActorID() const
{
	return pimpl->m_ActorID;
}

const cocos2d::Vec2 & EvtDataInputTouch::getPositionInWorld() const
{
	return pimpl->m_PositionInWorld;
}

const EventType EvtDataInputTouch::s_EventType{ "EvtDataInputTouch" };

const EventType & EvtDataInputTouch::vGetType() const
{
	return s_EventType;
}
