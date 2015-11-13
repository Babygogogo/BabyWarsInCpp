#ifndef __MOVING_AREA__
#define __MOVING_AREA__

#include <map>
#include <vector>

#include "../Utilities/GridIndex.h"

class MovingArea
{
public:
	struct MovingInfo
	{
		MovingInfo() = default;
		MovingInfo(int movingCost, int remainingMovementRange, bool canStay, const GridIndex & previousIndex);

		bool operator!=(const MovingInfo & rhs) const;
		bool operator==(const MovingInfo & rhs) const;

		GridIndex m_PreviousIndex;
		int m_MovingCost{ 0 };
		int m_MaxRemainingMovementRange{ -1 };
		bool m_CanStay{ false };
	};

	MovingArea() = default;
	~MovingArea() = default;

	MovingArea(MovingArea &&) = default;
	MovingArea & operator=(MovingArea &&) = default;

	MovingArea(int remainingMovementRange, const GridIndex & startingIndex);

	bool operator!=(const MovingArea & rhs) const;
	bool operator==(const MovingArea & rhs) const;

	bool isInitialized() const;
	void init(int remainingMovementRange, const GridIndex & startingIndex);

	//Get the starting index of the area. Asserts if the area has no starting index.
	GridIndex getStartingIndex() const;

	//Get MovingInfo within the area. Asserts if the index is not in the area.
	MovingInfo getMovingInfo(const GridIndex & index) const;

	bool hasIndex(const GridIndex & index) const;
	std::vector<GridIndex> getAllIndexesInArea() const;

	void clear();

	bool tryUpdateIndex(const GridIndex & index, MovingInfo && movingInfo);

private:
	GridIndex m_StartingIndex;
	std::map<GridIndex, MovingInfo> m_Map;
};

#endif // !__MOVING_AREA__
