#ifndef __MOVING_AREA__
#define __MOVING_AREA__

#include <map>
#include <vector>

#include "../../BabyEngine/Utilities/GridIndex.h"

class MovingArea
{
public:
	struct MovingInfo
	{
		MovingInfo() = default;
		MovingInfo(int remainingMovementRange, const GridIndex & previousIndex);

		bool m_CanPassThrough{ false };
		bool m_CanStay{ false };
		int m_MaxRemainingMovementRange{ -1 };
		GridIndex m_PreviousIndex;
	};

	MovingArea() = default;
	~MovingArea() = default;

	MovingArea(int remainingMovementRange, const GridIndex & index);

	//Get MovingInfo within the area. The bool in the returned value indicates if the MovingInfo is valid.
	std::pair<MovingInfo, bool> getMovingInfo(const GridIndex & index) const;

	bool hasIndex(const GridIndex & index) const;
	std::vector<GridIndex> getAllIndexesInArea() const;

	void clear();

	bool tryUpdateIndex(const GridIndex & index, MovingInfo && movingInfo);

private:
	std::map<GridIndex, MovingInfo> m_Map;
};

#endif // !__MOVING_AREA__
