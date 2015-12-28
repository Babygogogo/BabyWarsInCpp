#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/TileData.h"
#include "../Utilities/GridIndex.h"
#include "TileScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileScript::TileScriptImpl
{
	TileScriptImpl() = default;
	~TileScriptImpl() = default;

	GridIndex m_GridIndex;
	std::shared_ptr<TileData> m_TileData;

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TransformComponent> m_TransformComponent;
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

void TileScript::LoadTile(tinyxml2::XMLElement * xmlElement)
{
	//////////////////////////////////////////////////////////////////////////
	//Load and set the tile data.
	auto tileDataID = xmlElement->IntAttribute("TileDataID");
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto tileData = resourceLoader->getTileData(tileDataID);
	assert(tileData && "TileScript::setTileData() with nullptr.");

	auto ownerActor = m_OwnerActor.lock();
	//#TODO: This only shows the first first frame of the animation. Update the code and resources to show the whole animation.
	auto sceneNode = static_cast<cocos2d::Sprite*>(pimpl->m_RenderComponent->getSceneNode());
	sceneNode->setSpriteFrame(tileData->getAnimation()->getFrames().at(0)->getSpriteFrame());

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
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_TransformComponent.lock()->setPosition(gridIndex.toPosition(gridSize));
}

GridIndex TileScript::getGridIndex() const
{
	return pimpl->m_GridIndex;
}

bool TileScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void TileScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();

	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "TileScript::vPostInit() the actor has no sprite render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "TileScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);
}

const std::string TileScript::Type = "TileScript";

const std::string & TileScript::getType() const
{
	return Type;
}
