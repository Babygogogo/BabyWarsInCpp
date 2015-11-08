#include "EvtDataRequestDestroyActor.h"

const EventType EvtDataRequestDestroyActor::s_EventType{ "RequestDestroyActor" };

const EventType & EvtDataRequestDestroyActor::vGetType() const
{
	return s_EventType;
}

ActorID EvtDataRequestDestroyActor::getActorID() const
{
	return m_ActorID;
}

bool EvtDataRequestDestroyActor::isAlsoDestroyChildren() const
{
	return m_IsAlsoDestroyChildren;
}
