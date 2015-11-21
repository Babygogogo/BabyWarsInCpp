#include "cocos2d.h"

#include "EvtDataInputDrag.h"

//////////////////////////////////////////////////////////////////////////
//Definition of EvtDataInputDragImpl.
//////////////////////////////////////////////////////////////////////////
struct EvtDataInputDrag::EvtDataInputDragImpl
{
	EvtDataInputDragImpl(ActorID actorID, const cocos2d::Vec2 & positionInWorld, const cocos2d::Vec2 & previousPosition, DragState dragState)
		: m_ActorID{ actorID }, m_PositionInWorld{ positionInWorld }, m_PreviousPositionInWorld{ previousPosition }, m_DragState{ dragState } {}
	~EvtDataInputDragImpl() = default;

	ActorID m_ActorID{ INVALID_ACTOR_ID };
	cocos2d::Vec2 m_PositionInWorld;
	cocos2d::Vec2 m_PreviousPositionInWorld;
	DragState m_DragState{ DragState::Begin };
};

//////////////////////////////////////////////////////////////////////////
//Implementation of EvtDataInputDrag.
//////////////////////////////////////////////////////////////////////////
EvtDataInputDrag::EvtDataInputDrag(ActorID actorID, const cocos2d::Vec2 & positionInWorld, const cocos2d::Vec2 & previousPositionInWorld, DragState dragState)
	: pimpl{ std::make_unique<EvtDataInputDragImpl>(actorID, positionInWorld, previousPositionInWorld, dragState) }
{
}

EvtDataInputDrag::~EvtDataInputDrag()
{
}

ActorID EvtDataInputDrag::getActorID() const
{
	return pimpl->m_ActorID;
}

const cocos2d::Vec2 & EvtDataInputDrag::getPositionInWorld() const
{
	return pimpl->m_PositionInWorld;
}

const cocos2d::Vec2 & EvtDataInputDrag::getPreviousPositionInWorld() const
{
	return pimpl->m_PreviousPositionInWorld;
}

EvtDataInputDrag::DragState EvtDataInputDrag::getState() const
{
	return pimpl->m_DragState;
}

const EventType EvtDataInputDrag::s_EventType{ "EvtDataInputDrag" };

const EventType & EvtDataInputDrag::vGetType() const
{
	return s_EventType;
}
