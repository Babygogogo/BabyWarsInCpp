#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
#define __EVENT_DATA_FINISH_MAKE_MOVING_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataFinishMakeMovingPath : public BaseEventData
{
public:
	EvtDataFinishMakeMovingPath(const cocos2d::Vec2 & position) : m_Position{ position } {}
	virtual ~EvtDataFinishMakeMovingPath() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getPosition() const;

	//Disable copy/move constructor and operator=.
	EvtDataFinishMakeMovingPath(const EvtDataFinishMakeMovingPath &) = delete;
	EvtDataFinishMakeMovingPath(EvtDataFinishMakeMovingPath &&) = delete;
	EvtDataFinishMakeMovingPath & operator=(const EvtDataFinishMakeMovingPath &) = delete;
	EvtDataFinishMakeMovingPath & operator=(EvtDataFinishMakeMovingPath &&) = delete;

private:
	cocos2d::Vec2 m_Position;
};

#endif // !#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
