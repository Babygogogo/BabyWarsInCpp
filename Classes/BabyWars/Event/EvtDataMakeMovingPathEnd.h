#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
#define __EVENT_DATA_FINISH_MAKE_MOVING_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "../Utilities/MovingPath.h"

class EvtDataMakeMovingPathEnd : public BaseEventData
{
public:
	EvtDataMakeMovingPathEnd(const MovingPath & movingPath) : m_MovingPath{ movingPath } {}
	virtual ~EvtDataMakeMovingPathEnd() = default;

	const MovingPath & getMovingPath() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataMakeMovingPathEnd(const EvtDataMakeMovingPathEnd &) = delete;
	EvtDataMakeMovingPathEnd(EvtDataMakeMovingPathEnd &&) = delete;
	EvtDataMakeMovingPathEnd & operator=(const EvtDataMakeMovingPathEnd &) = delete;
	EvtDataMakeMovingPathEnd & operator=(EvtDataMakeMovingPathEnd &&) = delete;

private:
	MovingPath m_MovingPath;
};

#endif // !#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
