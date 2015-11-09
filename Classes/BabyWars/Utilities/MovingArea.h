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
		MovingInfo(int movingCost, int remainingMovementRange, bool canStay, const GridIndex & previousIndex);

		GridIndex m_PreviousIndex;
		int m_MovingCost{ 0 };
		int m_MaxRemainingMovementRange{ -1 };
		bool m_CanStay{ false };
	};

	MovingArea() = default;
	~MovingArea() = default;

	MovingArea(int remainingMovementRange, const GridIndex & startingIndex);

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
