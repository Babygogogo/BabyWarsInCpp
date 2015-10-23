#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Resource/TileData.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/SingletonContainer.h"
#include "../Utilities/GridIndex.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileScript::TileScriptImpl
{
	TileScriptImpl(){};
	~TileScriptImpl(){};

	GridIndex m_GridIndex;
	std::shared_ptr<TileData> m_TileData;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
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
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
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

void TileScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes.
	pimpl->m_GridIndex = gridIndex;

	//Set the position of the node according to indexes.
	pimpl->m_RenderComponent.lock()->getSceneNode()->setPosition(gridIndex.toPosition());
}

GridIndex TileScript::getGridIndex() const
{
	return pimpl->m_GridIndex;
}

const std::string TileScript::Type = "TileScript";

const std::string & TileScript::getType() const
{
	return Type;
}
