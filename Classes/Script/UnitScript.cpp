#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/UnitData.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitScript::UnitScriptImpl
{
	UnitScriptImpl(){};
	~UnitScriptImpl(){};

	int m_RowIndex{ 0 }, m_ColIndex{ 0 };
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

void UnitScript::setRowAndColIndex(int rowIndex, int colIndex)
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

int UnitScript::getRowIndex() const
{
	return pimpl->m_RowIndex;
}

int UnitScript::getColIndex() const
{
	return pimpl->m_ColIndex;
}

void UnitScript::onSingleTouch()
{
	auto renderComponent = pimpl->m_RenderComponent.lock();
	auto underlyingNode = renderComponent->getSceneNode();

	auto rotateAction = cocos2d::RotateBy::create(0.2, 45, 45);
	underlyingNode->runAction(rotateAction);
}

const std::string UnitScript::Type = "UnitScript";

const std::string & UnitScript::getType() const
{
	return Type;
}
