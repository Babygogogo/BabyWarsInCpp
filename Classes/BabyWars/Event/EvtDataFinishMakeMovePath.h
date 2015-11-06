#ifndef __EVENT_DATA_FINISH_MAKE_MOVE_PATH__
#define __EVENT_DATA_FINISH_MAKE_MOVE_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataFinishMakeMovePath : public BaseEventData
{
public:
	EvtDataFinishMakeMovePath(const cocos2d::Vec2 & position) : m_Position{ position }{}
	virtual ~EvtDataFinishMakeMovePath() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getPosition() const;

	//Disable copy/move constructor and operator=.
	EvtDataFinishMakeMovePath(const EvtDataFinishMakeMovePath &) = delete;
	EvtDataFinishMakeMovePath(EvtDataFinishMakeMovePath &&) = delete;
	EvtDataFinishMakeMovePath & operator=(const EvtDataFinishMakeMovePath &) = delete;
	EvtDataFinishMakeMovePath & operator=(EvtDataFinishMakeMovePath &&) = delete;

private:
	cocos2d::Vec2 m_Position;
};

#endif // !#ifndef __EVENT_DATA_FINISH_MAKE_MOVE_PATH__
