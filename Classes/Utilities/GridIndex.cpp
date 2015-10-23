#include "cocos2d.h"

#include "GridIndex.h"
#include "SingletonContainer.h"
#include "../Resource/ResourceLoader.h"

GridIndex::GridIndex(float positionX, float positionY)
{
	if (positionX < 0. || positionY < 0.){
		rowIndex = -1;
		colIndex = -1;

		return;
	}

	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	rowIndex = static_cast<int>(positionY / gridSize.height);
	colIndex = static_cast<int>(positionX / gridSize.width);
}

cocos2d::Vec2 GridIndex::toPosition() const
{
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();

	return{ (static_cast<float>(colIndex)+0.5f) * gridSize.width, (static_cast<float>(rowIndex)+0.5f) * gridSize.height };
}
