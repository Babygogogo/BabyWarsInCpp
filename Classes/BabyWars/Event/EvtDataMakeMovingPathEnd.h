#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
#define __EVENT_DATA_FINISH_MAKE_MOVING_PATH__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "../Utilities/MovingPath.h"

class EvtDataMakeMovingPathEnd : public BaseEventData
{
public:
	EvtDataMakeMovingPathEnd(const MovingPath & movingPath, bool isPathValid) : m_MovingPath{ movingPath }, m_IsPathValid{ isPathValid } {}
	virtual ~EvtDataMakeMovingPathEnd() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	const MovingPath & getMovingPath() const;
	bool isPathValid() const;

	//Disable copy/move constructor and operator=.
	EvtDataMakeMovingPathEnd(const EvtDataMakeMovingPathEnd &) = delete;
	EvtDataMakeMovingPathEnd(EvtDataMakeMovingPathEnd &&) = delete;
	EvtDataMakeMovingPathEnd & operator=(const EvtDataMakeMovingPathEnd &) = delete;
	EvtDataMakeMovingPathEnd & operator=(EvtDataMakeMovingPathEnd &&) = delete;

private:
	MovingPath m_MovingPath;
	bool m_IsPathValid{ false };
};

#endif // !#ifndef __EVENT_DATA_FINISH_MAKE_MOVING_PATH__
