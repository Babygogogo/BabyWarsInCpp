#ifndef __EVENT_DATA_DRAG_SCENE__
#define __EVENT_DATA_DRAG_SCENE__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataDragScene : public BaseEventData
{
public:
	EvtDataDragScene(const cocos2d::Vec2 & offset) : m_Offset{ offset }{}
	virtual ~EvtDataDragScene() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getOffset() const;

	//Disable copy/move constructor and operator=.
	EvtDataDragScene(const EvtDataDragScene &) = delete;
	EvtDataDragScene(EvtDataDragScene &&) = delete;
	EvtDataDragScene & operator=(const EvtDataDragScene &) = delete;
	EvtDataDragScene & operator=(EvtDataDragScene &&) = delete;

private:
	cocos2d::Vec2 m_Offset;
};

#endif // !__EVENT_DATA_DRAG_SCENE__
