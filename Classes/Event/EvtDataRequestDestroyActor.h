#ifndef __EVENT_REQUEST_DESTORY_ACTOR__
#define __EVENT_REQUEST_DESTORY_ACTOR__

#include "BaseEventData.h"
#include "EventType.h"
#include "../Actor/ActorID.h"

class EvtDataRequestDestoryActor final : public BaseEventData
{
public:
	EvtDataRequestDestoryActor(ActorID actorID) : m_ActorID{ actorID }
	{
	}

	virtual ~EvtDataRequestDestoryActor() = default;

	static const EventType EvtType = EventType::RequestDestoryActor;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getActorID() const
	{
		return m_ActorID;
	}

private:
	int m_ActorID{ 0 };
};

#endif // !__EVENT_REQUEST_DESTORY_ACTOR__
