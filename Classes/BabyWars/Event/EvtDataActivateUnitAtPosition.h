#ifndef __EVENT_DATA_ACTIVATE_UNIT_AT_POSITION__
#define __EVENT_DATA_ACTIVATE_UNIT_AT_POSITION__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "cocos2d.h"

class EvtDataActivateUnitAtPosition : public BaseEventData
{
public:
	EvtDataActivateUnitAtPosition(const cocos2d::Vec2 & pos) : m_Position{ pos }{}
	virtual ~EvtDataActivateUnitAtPosition() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const cocos2d::Vec2 & getPosition() const;

	//Disable copy/move constructor and operator=.
	EvtDataActivateUnitAtPosition(const EvtDataActivateUnitAtPosition &) = delete;
	EvtDataActivateUnitAtPosition(EvtDataActivateUnitAtPosition &&) = delete;
	EvtDataActivateUnitAtPosition & operator=(const EvtDataActivateUnitAtPosition &) = delete;
	EvtDataActivateUnitAtPosition & operator=(EvtDataActivateUnitAtPosition &&) = delete;

private:
	cocos2d::Vec2 m_Position;
};

#endif // !__EVENT_DATA_ACTIVATE_UNIT_AT_POSITION__
