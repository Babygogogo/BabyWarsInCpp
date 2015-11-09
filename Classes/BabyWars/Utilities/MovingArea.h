#ifndef __MOVING_AREA__
#define __MOVING_AREA__

#include "../../BabyEngine/Utilities/GridIndex.h"

class MovingArea
{
public:
	MovingArea() = default;
	~MovingArea() = default;

	//Disable copy/move constructor and operator=.
	MovingArea(const MovingArea &) = delete;
	MovingArea(MovingArea &&) = delete;
	MovingArea & operator=(const MovingArea &) = delete;
	MovingArea & operator=(MovingArea &&) = delete;

private:
	struct MovingInfo
	{
	};

	//std::map < GridIndex,
};

#endif // !__MOVING_AREA__
