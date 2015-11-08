#ifndef __GRID_INDEX__
#define __GRID_INDEX__

//Forward declaration.
namespace cocos2d{
	class Vec2;
	class Size;
}

#include <list>

struct GridIndex
{
	GridIndex() = default;
	~GridIndex() = default;

	GridIndex(const GridIndex &) = default;
	GridIndex & operator=(const GridIndex &) = default;
	//GridIndex(GridIndex &&) = default;
	//GridIndex & operator=(GridIndex &&) = default;

	GridIndex(int rowIndex, int colIndex) : rowIndex{ rowIndex }, colIndex{ colIndex }{}
	GridIndex(const cocos2d::Vec2 & position, const cocos2d::Size & gridSize);

	std::list<GridIndex> getNeighbors() const{
		auto neighbors = std::list<GridIndex>();

		neighbors.emplace_back(GridIndex(rowIndex - 1, colIndex));
		neighbors.emplace_back(GridIndex(rowIndex + 1, colIndex));
		neighbors.emplace_back(GridIndex(rowIndex, colIndex - 1));
		neighbors.emplace_back(GridIndex(rowIndex, colIndex + 1));

		return neighbors;
	}

	bool operator==(const GridIndex & rhs) const{ return rowIndex == rhs.rowIndex && colIndex == rhs.colIndex; }

	//This is for std::map and doesn't mean much.
	bool operator<(const GridIndex & rhs) const
	{
		if (rowIndex < rhs.rowIndex)
			return true;
		if (rowIndex == rhs.rowIndex && colIndex < rhs.colIndex)
			return true;
		return false;
	}

	cocos2d::Vec2 toPosition(const cocos2d::Size & gridSize) const;

	int rowIndex{};
	int colIndex{};
};

#endif // !__GRID_INDEX__
