#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/UnitData.h"
#include "../Utilities/SingletonContainer.h"
#include "../Utilities/GridIndex.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitScript::UnitScriptImpl
{
	UnitScriptImpl(){};
	~UnitScriptImpl(){};

	GridIndex m_GridIndex;
	std::shared_ptr<UnitData> m_UnitData;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
UnitScript::UnitScript() : pimpl{ std::make_unique<UnitScriptImpl>() }
{
}

UnitScript::~UnitScript()
{
}

bool UnitScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void UnitScript::vPostInit()
{
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
}

void UnitScript::loadUnit(tinyxml2::XMLElement * xmlElement)
{
	//////////////////////////////////////////////////////////////////////////
	//Load and set the unit data.
	auto unitDataID = xmlElement->IntAttribute("UnitDataID");
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto unitData = resourceLoader->getUnitData(unitDataID);
	assert(unitData && "UnitScript::setUnitData() with nullptr.");

	auto ownerActor = m_OwnerActor.lock();
	auto underlyingSprite = ownerActor->getRenderComponent()->getSceneNode<cocos2d::Sprite>();
	//#TODO: This only shows the first first frame of the animation. Update the code to show the whole animation.
	underlyingSprite->setSpriteFrame(unitData->getAnimation()->getFrames().at(0)->getSpriteFrame());

	//Scale the sprite so that it meets the real game grid size.
	auto designGridSize = resourceLoader->getDesignGridSize();
	auto realGameGridSize = resourceLoader->getRealGameGridSize();
	auto designScaleFactor = unitData->getDesignScaleFactor();
	underlyingSprite->setScaleX(realGameGridSize.width / designGridSize.width * designScaleFactor);
	underlyingSprite->setScaleY(realGameGridSize.height / designGridSize.height * designScaleFactor);

	pimpl->m_UnitData = std::move(unitData);

	//////////////////////////////////////////////////////////////////////////
	//#TODO: Load more data, such as the hp, level and so on, from the xml.
}

const std::shared_ptr<UnitData> & UnitScript::getUnitData() const
{
	return pimpl->m_UnitData;
}

void UnitScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes.
	pimpl->m_GridIndex = gridIndex;

	//Set the position of the node according to indexes.
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	pimpl->m_RenderComponent.lock()->getSceneNode()->setPosition(gridIndex.toPosition(gridSize));
}

GridIndex UnitScript::getGridIndex() const
{
	return pimpl->m_GridIndex;
}

void UnitScript::setActive(bool active)
{
	auto underlyingNode = pimpl->m_RenderComponent.lock()->getSceneNode();
	if (!active){
		underlyingNode->stopAllActions();
		underlyingNode->setRotation(0);
		return;
	}

	auto sequence = cocos2d::Sequence::create(cocos2d::RotateTo::create(0.2f, 30, 30), cocos2d::RotateTo::create(0.4f, -30, -30), cocos2d::RotateTo::create(0.2f, 0, 0), nullptr);
	underlyingNode->runAction(cocos2d::RepeatForever::create(sequence));
}

void UnitScript::moveTo(const GridIndex & gridIndex)
{
	pimpl->m_GridIndex = gridIndex;

	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto moveTo = cocos2d::MoveTo::create(0.5, gridIndex.toPosition(gridSize));
	pimpl->m_RenderComponent.lock()->getSceneNode()->runAction(moveTo);
}

const std::string UnitScript::Type = "UnitScript";

const std::string & UnitScript::getType() const
{
	return Type;
}
