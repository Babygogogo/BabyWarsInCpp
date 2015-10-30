#include "cocos2d.h"

#include "GridIndex.h"

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

cocos2d::Vec2 GridIndex::toPosition(const cocos2d::Size & gridSize) const
{
	assert(gridSize.height > 0. && gridSize.width >= 0. && "GridIndex::toPosition() gridSize is non-positive.");

	return{ (static_cast<float>(colIndex)+0.5f) * gridSize.width, (static_cast<float>(rowIndex)+0.5f) * gridSize.height };
}
