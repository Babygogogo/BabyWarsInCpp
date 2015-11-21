#ifndef __EVENT_DATA_INPUT_DRAG__
#define __EVENT_DATA_INPUT_DRAG__

#include <memory>
#include "BaseEventData.h"
#include "../Actor/ActorID.h"

//Forward declaration.
namespace cocos2d
{
	class Vec2;
}

class EvtDataInputDrag final : public BaseEventData
{
public:
	enum class DragState
	{
		Begin,
		Dragging,
		End
	};

	EvtDataInputDrag(ActorID actorID, const cocos2d::Vec2 & positionInWorld, const cocos2d::Vec2 & previousPositionInWorld, DragState dragState);
	virtual ~EvtDataInputDrag();

	ActorID getActorID() const;
	const cocos2d::Vec2 & getPositionInWorld() const;
	const cocos2d::Vec2 & getPreviousPositionInWorld() const;
	DragState getState() const;

	//Type name of this event data.
	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataInputDrag(const EvtDataInputDrag &) = delete;
	EvtDataInputDrag(EvtDataInputDrag &&) = delete;
	EvtDataInputDrag & operator=(const EvtDataInputDrag &) = delete;
	EvtDataInputDrag & operator=(EvtDataInputDrag &&) = delete;

private:
	//Implementation stuff.
	struct EvtDataInputDragImpl;
	std::unique_ptr<EvtDataInputDragImpl> pimpl;
};

#endif // !__EVENT_DATA_INPUT_DRAG__
