#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Resource/TileData.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileScript::TileScriptImpl
{
	TileScriptImpl(){};
	~TileScriptImpl(){};

	int m_RowIndex{ 0 }, m_ColIndex{ 0 };
	std::shared_ptr<TileData> m_TileData;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
TileScript::TileScript() : pimpl{ std::make_unique<TileScriptImpl>() }
{
}

TileScript::~TileScript()
{
}

bool TileScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void TileScript::vPostInit()
{
}

void TileScript::LoadTile(tinyxml2::XMLElement * xmlElement)
{
	//////////////////////////////////////////////////////////////////////////
	//Load and set the tile data.
	auto tileDataID = xmlElement->IntAttribute("TileDataID");
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto tileData = resourceLoader->getTileData(tileDataID);
	assert(tileData && "TileScript::setTileData() with nullptr.");

	auto ownerActor = m_OwnerActor.lock();
	auto underlyingSprite = ownerActor->getRenderComponent()->getSceneNode<cocos2d::Sprite>();
	//#TODO: This only shows the first first frame of the animation. Update the code to show the whole animation.
	underlyingSprite->setSpriteFrame(tileData->getAnimation()->getFrames().at(0)->getSpriteFrame());

	//Scale the sprite so that it meets the real game grid size.
	auto designGridSize = resourceLoader->getDesignGridSize();
	auto realGameGridSize = resourceLoader->getRealGameGridSize();
	auto designScaleFactor = tileData->getDesignScaleFactor();
	underlyingSprite->setScaleX(realGameGridSize.width / designGridSize.width * designScaleFactor);
	underlyingSprite->setScaleY(realGameGridSize.height / designGridSize.height * designScaleFactor);

	pimpl->m_TileData = std::move(tileData);

	//////////////////////////////////////////////////////////////////////////
	//#TODO: Load more data, such as the hp, level and so on, from the xml.
}

const std::shared_ptr<TileData> & TileScript::getTileData() const
{
	return pimpl->m_TileData;
}

void TileScript::setRowAndColIndex(int rowIndex, int colIndex)
{
	//Set the indexes.
	pimpl->m_RowIndex = rowIndex;
	pimpl->m_ColIndex = colIndex;

	//Set the position of the node according to indexes.
	auto strongActor = m_OwnerActor.lock();
	auto underlyingNode = strongActor->getRenderComponent()->getSceneNode();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	underlyingNode->setPosition((static_cast<float>(colIndex)+0.5f) * gridSize.width, (static_cast<float>(rowIndex)+0.5f) * gridSize.height);
}

int TileScript::getRowIndex() const
{
	return pimpl->m_RowIndex;
}

int TileScript::getColIndex() const
{
	return pimpl->m_ColIndex;
}

const std::string TileScript::Type = "TileScript";

const std::string & TileScript::getType() const
{
	return Type;
}
