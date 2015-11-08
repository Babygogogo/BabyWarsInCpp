#ifndef __EVENT_DATA_REQUEST_DESTROY_ACTOR__
#define __EVENT_DATA_REQUEST_DESTROY_ACTOR__

#include "BaseEventData.h"

#include "../Actor/ActorID.h"

class EvtDataRequestDestroyActor final : public BaseEventData
{
public:
	EvtDataRequestDestroyActor(ActorID actorID, bool alsoDestroyChildren = true) : m_ActorID{ actorID }, m_IsAlsoDestroyChildren{ alsoDestroyChildren }{}
	virtual ~EvtDataRequestDestroyActor() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	ActorID getActorID() const;
	bool isAlsoDestroyChildren() const;

	//Disable copy/move constructor and operator=.
	EvtDataRequestDestroyActor(const EvtDataRequestDestroyActor &) = delete;
	EvtDataRequestDestroyActor(EvtDataRequestDestroyActor &&) = delete;
	EvtDataRequestDestroyActor & operator=(const EvtDataRequestDestroyActor &) = delete;
	EvtDataRequestDestroyActor & operator=(EvtDataRequestDestroyActor &&) = delete;

private:
	ActorID m_ActorID{ INVALID_ACTOR_ID };
	bool m_IsAlsoDestroyChildren{ true };
};

#endif // !__EVENT_DATA_REQUEST_DESTROY_ACTOR__
