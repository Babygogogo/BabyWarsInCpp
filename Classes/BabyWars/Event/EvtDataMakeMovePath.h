#ifndef __EVENT_DATA_MAKE_MOVE_PATH__
#define __EVENT_DATA_MAKE_MOVE_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataMakeMovePath : public BaseEventData
{
public:
	EvtDataMakeMovePath(const cocos2d::Vec2 & position) : m_Position{ position }{}
	virtual ~EvtDataMakeMovePath() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getPosition() const;

	//Disable copy/move constructor and operator=.
	EvtDataMakeMovePath(const EvtDataMakeMovePath &) = delete;
	EvtDataMakeMovePath(EvtDataMakeMovePath &&) = delete;
	EvtDataMakeMovePath & operator=(const EvtDataMakeMovePath &) = delete;
	EvtDataMakeMovePath & operator=(EvtDataMakeMovePath &&) = delete;

private:
	cocos2d::Vec2 m_Position;
};

#endif // !__EVENT_DATA_MAKE_MOVE_PATH__
