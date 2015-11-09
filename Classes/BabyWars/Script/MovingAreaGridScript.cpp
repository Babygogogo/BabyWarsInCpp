#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingAreaGridScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyWars/Resource/ResourceLoader.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingRangeGridScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingAreaGridScript::MovingRangeGridScriptImpl
{
	MovingRangeGridScriptImpl(){};
	~MovingRangeGridScriptImpl(){};

	GridIndex m_GridIndex;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingAreaGridScript::MovingAreaGridScript() : pimpl{ std::make_unique<MovingRangeGridScriptImpl>() }
{
}

MovingAreaGridScript::~MovingAreaGridScript()
{
}

void MovingAreaGridScript::setGridIndexAndPosition(const GridIndex & index)
{
	pimpl->m_GridIndex = index;

	auto renderComponent = pimpl->m_RenderComponent.lock();
	renderComponent->getSceneNode()->setPosition(index.toPosition(SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize()));
}

void MovingAreaGridScript::setVisible(bool visible)
{
	pimpl->m_RenderComponent.lock()->getSceneNode()->setVisible(visible);
}

bool MovingAreaGridScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void MovingAreaGridScript::vPostInit()
{
	auto renderComponent = m_OwnerActor.lock()->getRenderComponent();
	pimpl->m_RenderComponent = renderComponent;

	auto sceneNode = renderComponent->getSceneNode();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	sceneNode->setScaleX(gridSize.width / sceneNode->getContentSize().width);
	sceneNode->setScaleY(gridSize.height / sceneNode->getContentSize().height);
}

const std::string MovingAreaGridScript::Type = "MovingAreaGridScript";

const std::string & MovingAreaGridScript::getType() const
{
	return Type;
}
