#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "GridIndex.h"
#include "AdjacentDirection.h"

const std::vector<std::pair<AdjacentDirection, GridIndex>> s_Adjacents{
	std::make_pair(AdjacentDirection::BELOW, GridIndex(-1, 0)),
	std::make_pair(AdjacentDirection::UPPER, GridIndex(1, 0)),
	std::make_pair(AdjacentDirection::LEFT, GridIndex(0, -1)),
	std::make_pair(AdjacentDirection::RIGHT, GridIndex(0, 1))
};

GridIndex::GridIndex(int rowIndex, int colIndex) : rowIndex{ rowIndex }, colIndex{ colIndex }
{
}

GridIndex::GridIndex(const cocos2d::Vec2 & position, const cocos2d::Size & gridSize)
{
	assert(gridSize.height > 0. && gridSize.width >= 0. && "GridIndex::GridIndex() gridSize is non-positive.");

	if (position.x < 0. || position.y < 0.) {
		rowIndex = -1;
		colIndex = -1;

		return;
	}

	rowIndex = static_cast<int>(position.y / gridSize.height);
	colIndex = static_cast<int>(position.x / gridSize.width);
}

GridIndex::GridIndex(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "GridIndex::GridIndex() the xml element is nullptr.");
	if (xmlElement) {
		rowIndex = xmlElement->IntAttribute("Row");
		colIndex = xmlElement->IntAttribute("Column");
	}
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
	auto adjacents = std::vector<GridIndex>();
	for (auto & adjacentPair : s_Adjacents)
		adjacents.emplace_back(*this + adjacentPair.second);

	return adjacents;
}

bool GridIndex::isAdjacentTo(const GridIndex & rhs) const
{
	return getAdjacentDirectionOf(rhs) != AdjacentDirection::INVALID;
}

AdjacentDirection GridIndex::getAdjacentDirectionOf(const GridIndex & rhs) const
{
	auto adjacent = *this - rhs;
	for (const auto & adjacentPair : s_Adjacents)
		if (adjacent == adjacentPair.second)
			return adjacentPair.first;

	return AdjacentDirection::INVALID;
}

cocos2d::Vec2 GridIndex::toPosition(const cocos2d::Size & gridSize) const
{
	assert(gridSize.height > 0. && gridSize.width >= 0. && "GridIndex::toPosition() gridSize is non-positive.");

	return{ (static_cast<float>(colIndex) + 0.5f) * gridSize.width, (static_cast<float>(rowIndex) + 0.5f) * gridSize.height };
}
