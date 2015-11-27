#ifndef __EVENT_DATA_SHOW_MOVING_AREA_END__
#define __EVENT_DATA_SHOW_MOVING_AREA_END__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class MovingArea;

class EvtDataShowMovingAreaEnd : public BaseEventData
{
public:
	EvtDataShowMovingAreaEnd(std::weak_ptr<const MovingArea> && movingArea) : m_MovingArea{ std::move(movingArea) } {}
	virtual ~EvtDataShowMovingAreaEnd() = default;

	std::weak_ptr<const MovingArea> getMovingArea() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataShowMovingAreaEnd(const EvtDataShowMovingAreaEnd &) = delete;
	EvtDataShowMovingAreaEnd(EvtDataShowMovingAreaEnd &&) = delete;
	EvtDataShowMovingAreaEnd & operator=(const EvtDataShowMovingAreaEnd &) = delete;
	EvtDataShowMovingAreaEnd & operator=(EvtDataShowMovingAreaEnd &&) = delete;

private:
	std::weak_ptr<const MovingArea> m_MovingArea;
};

#endif // !__EVENT_DATA_SHOW_MOVING_AREA_END__
