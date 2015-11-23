#ifndef __EVENT_DATA_MAKE_MOVING_PATH_BEGIN__
#define __EVENT_DATA_MAKE_MOVING_PATH_BEGIN__

#include "../../BabyEngine/Event/BaseEventData.h"

#include "../Utilities/GridIndex.h"

class EvtDataMakeMovingPathBegin : public BaseEventData
{
public:
	EvtDataMakeMovingPathBegin(const GridIndex & gridIndex) : m_GridIndex{ gridIndex } {}
	virtual ~EvtDataMakeMovingPathBegin() = default;

	const GridIndex & getGridIndex() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataMakeMovingPathBegin(const EvtDataMakeMovingPathBegin &) = delete;
	EvtDataMakeMovingPathBegin(EvtDataMakeMovingPathBegin &&) = delete;
	EvtDataMakeMovingPathBegin & operator=(const EvtDataMakeMovingPathBegin &) = delete;
	EvtDataMakeMovingPathBegin & operator=(EvtDataMakeMovingPathBegin &&) = delete;

private:
	GridIndex m_GridIndex;
};

#endif // !__EVENT_DATA_MAKE_MOVING_PATH_BEGIN__
