#ifndef __GRID_INDEX__
#define __GRID_INDEX__

//Forward declaration.
namespace cocos2d{
	class Vec2;
}

struct GridIndex
{
	GridIndex() = default;
	GridIndex(int rowIndex, int colIndex) : rowIndex{ rowIndex }, colIndex{ colIndex }{}
	GridIndex(float positionX, float positionY);
	~GridIndex() = default;

	bool operator==(const GridIndex & rhs){ return rowIndex == rhs.rowIndex && colIndex == rhs.colIndex; }

	cocos2d::Vec2 toPosition() const;

	int rowIndex{};
	int colIndex{};
};

#endif // !__GRID_INDEX__
