#include "cocos2d.h"

#include "GridIndex.h"

const std::vector<GridIndex> s_Adjacents{ { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

GridIndex::GridIndex(int rowIndex, int colIndex) : rowIndex{ rowIndex }, colIndex{ colIndex }
{
}

GridIndex::GridIndex(const cocos2d::Vec2 & position, const cocos2d::Size & gridSize)
{
	assert(gridSize.height > 0. && gridSize.width >= 0. && "GridIndex::GridIndex() gridSize is non-positive.");

	if (position.x < 0. || position.y < 0.){
		rowIndex = -1;
		colIndex = -1;

		return;
	}

	rowIndex = static_cast<int>(position.y / gridSize.height);
	colIndex = static_cast<int>(position.x / gridSize.width);
}

bool GridIndex::operator==(const GridIndex & rhs) const
{
	return rowIndex == rhs.rowIndex && colIndex == rhs.colIndex;
}

bool GridIndex::operator!=(const GridIndex & rhs) const
{
	return !(*this == rhs);
}

GridIndex GridIndex::operator-(const GridIndex & rhs) const
{
	return GridIndex(rowIndex - rhs.rowIndex, colIndex - rhs.colIndex);
}

GridIndex GridIndex::operator+(const GridIndex & rhs) const
{
	return GridIndex(rowIndex + rhs.rowIndex, colIndex + rhs.colIndex);
}

GridIndex & GridIndex::operator+=(const GridIndex & rhs)
{
	rowIndex += rhs.rowIndex;
	colIndex += rhs.colIndex;

	return *this;
}

bool GridIndex::operator<(const GridIndex & rhs) const
{
	if (rowIndex < rhs.rowIndex)
		return true;
	if (rowIndex == rhs.rowIndex && colIndex < rhs.colIndex)
		return true;
	return false;
}

std::vector<GridIndex> GridIndex::getAdjacentIndexes() const
{
	auto adjacents = s_Adjacents;
	for (auto & adjacent : adjacents)
		adjacent += *this;

	return adjacents;
}

bool GridIndex::isAdjacentTo(const GridIndex & rhs) const
{
	auto adjacent = *this - rhs;
	for (const auto & adj : s_Adjacents)
		if (adjacent == adj)
			return true;

	return false;
}

cocos2d::Vec2 GridIndex::toPosition(const cocos2d::Size & gridSize) const
{
	assert(gridSize.height > 0. && gridSize.width >= 0. && "GridIndex::toPosition() gridSize is non-positive.");

	return{ (static_cast<float>(colIndex)+0.5f) * gridSize.width, (static_cast<float>(rowIndex)+0.5f) * gridSize.height };
}
