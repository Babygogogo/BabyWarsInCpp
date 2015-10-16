#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Resource/TileData.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileScript::TileScriptImpl
{
	TileScriptImpl(){};
	~TileScriptImpl(){};

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;

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
	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
}

void TileScript::setTileData(std::shared_ptr<TileData> tileData)
{
	assert(tileData && "TileScript::setTileData() with nullptr.");

	auto underlyingSprite = pimpl->m_RenderComponent->getSceneNode<cocos2d::Sprite>();
	//#TODO: This only shows the first first frame of the animation. Update the code to show the whole animation.
	underlyingSprite->setSpriteFrame(tileData->getAnimation()->getFrames().at(0)->getSpriteFrame());

	//Scale the sprite so that it meets the common size of tile data.
	auto spriteSize = underlyingSprite->getSpriteFrame()->getOriginalSize();
	underlyingSprite->setScaleX(TileData::getCommonWidth() / spriteSize.width);
	underlyingSprite->setScaleY(TileData::getCommonHeight() / spriteSize.height);

	pimpl->m_TileData = std::move(tileData);
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
	auto underlyingNode = pimpl->m_RenderComponent->getSceneNode();
	auto tileHeight = TileData::getCommonHeight();
	auto tileWidth = TileData::getCommonWidth();
	underlyingNode->setPosition((static_cast<float>(colIndex)+0.5f) * tileWidth, (static_cast<float>(rowIndex)+0.5f) * tileHeight);
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
