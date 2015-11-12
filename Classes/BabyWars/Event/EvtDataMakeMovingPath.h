#ifndef __EVENT_DATA_MAKE_MOVING_PATH__
#define __EVENT_DATA_MAKE_MOVING_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataMakeMovingPath : public BaseEventData
{
public:
	EvtDataMakeMovingPath(const cocos2d::Vec2 & position) : m_Position{ position } {}
	virtual ~EvtDataMakeMovingPath() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getPosition() const;

	//Disable copy/move constructor and operator=.
	EvtDataMakeMovingPath(const EvtDataMakeMovingPath &) = delete;
	EvtDataMakeMovingPath(EvtDataMakeMovingPath &&) = delete;
	EvtDataMakeMovingPath & operator=(const EvtDataMakeMovingPath &) = delete;
	EvtDataMakeMovingPath & operator=(EvtDataMakeMovingPath &&) = delete;

private:
	cocos2d::Vec2 m_Position;
};

#endif // !__EVENT_DATA_MAKE_MOVING_PATH__
