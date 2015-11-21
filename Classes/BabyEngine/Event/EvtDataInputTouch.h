#ifndef __EVENT_DATA_INPUT_TOUCH__
#define __EVENT_DATA_INPUT_TOUCH__

#include <memory>
#include "BaseEventData.h"
#include "../Actor/ActorID.h"

//Forward declaration.
namespace cocos2d
{
	class Vec2;
}

class EvtDataInputTouch final : public BaseEventData
{
public:
	EvtDataInputTouch(ActorID actorID, const cocos2d::Vec2 & positionInWorld);
	virtual ~EvtDataInputTouch();

	ActorID getActorID() const;
	const cocos2d::Vec2 & getPositionInWorld() const;

	//Type name of this event data.
	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataInputTouch(const EvtDataInputTouch &) = delete;
	EvtDataInputTouch(EvtDataInputTouch &&) = delete;
	EvtDataInputTouch & operator=(const EvtDataInputTouch &) = delete;
	EvtDataInputTouch & operator=(EvtDataInputTouch &&) = delete;

private:
	//Implementation stuff.
	struct EvtDataInputTouchImpl;
	std::unique_ptr<EvtDataInputTouchImpl> pimpl;
};

#endif // !__EVENT_DATA_INPUT_TOUCH__
