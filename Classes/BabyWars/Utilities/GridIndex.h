#ifndef __GRID_INDEX__
#define __GRID_INDEX__

#include <vector>

//Forward declaration.
namespace cocos2d
{
	class Vec2;
	class Size;
}
namespace tinyxml2
{
	class XMLElement;
}
enum class AdjacentDirection;

struct GridIndex
{
	GridIndex() = default;
	~GridIndex() = default;

	GridIndex(const GridIndex &) = default;
	GridIndex & operator=(const GridIndex &) = default;
	GridIndex(GridIndex &&) = default;
	GridIndex & operator=(GridIndex &&) = default;

	GridIndex(int rowIndex, int colIndex);
	GridIndex(const cocos2d::Vec2 & position, const cocos2d::Size & gridSize);
	GridIndex(const tinyxml2::XMLElement * xmlElement);

	//////////////////////////////////////////////////////////////////////////
	//Operators.
	bool operator==(const GridIndex & rhs) const;
	bool operator!=(const GridIndex & rhs) const;

	GridIndex operator-(const GridIndex & rhs) const;
	GridIndex operator+(const GridIndex & rhs) const;
	GridIndex & operator+=(const GridIndex & rhs);

	//This is for std::map and doesn't mean much.
	bool operator<(const GridIndex & rhs) const;

	//////////////////////////////////////////////////////////////////////////
	//Other functions.
	std::vector<GridIndex> getAdjacentIndexes() const;
	bool isAdjacentTo(const GridIndex & rhs) const;
	AdjacentDirection getAdjacentDirectionOf(const GridIndex & rhs) const;

	cocos2d::Vec2 toPosition(const cocos2d::Size & gridSize) const;

	//////////////////////////////////////////////////////////////////////////
	//Data members.
	int rowIndex{};
	int colIndex{};
};

#endif // !__GRID_INDEX__
