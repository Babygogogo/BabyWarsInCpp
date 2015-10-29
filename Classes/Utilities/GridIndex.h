#ifndef __GRID_INDEX__
#define __GRID_INDEX__

//Forward declaration.
namespace cocos2d{
	class Vec2;
	class Size;
}

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

	bool operator==(const GridIndex & rhs) const{ return rowIndex == rhs.rowIndex && colIndex == rhs.colIndex; }

	cocos2d::Vec2 toPosition(const cocos2d::Size & gridSize) const;

	int rowIndex{};
	int colIndex{};
};

#endif // !__GRID_INDEX__
