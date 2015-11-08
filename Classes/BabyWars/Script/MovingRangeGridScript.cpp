#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingRangeGridScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyWars/Resource/ResourceLoader.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingRangeGridScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingRangeGridScript::MovingRangeGridScriptImpl
{
	MovingRangeGridScriptImpl(){};
	~MovingRangeGridScriptImpl(){};

	GridIndex m_GridIndex;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingRangeGridScript::MovingRangeGridScript() : pimpl{ std::make_unique<MovingRangeGridScriptImpl>() }
{
}

MovingRangeGridScript::~MovingRangeGridScript()
{
}

void MovingRangeGridScript::setGridIndexAndPosition(const GridIndex & index)
{
	pimpl->m_GridIndex = index;

	auto renderComponent = pimpl->m_RenderComponent.lock();
	renderComponent->getSceneNode()->setPosition(index.toPosition(SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize()));
}

void MovingRangeGridScript::setVisible(bool visible)
{
	pimpl->m_RenderComponent.lock()->getSceneNode()->setVisible(visible);
}

bool MovingRangeGridScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void MovingRangeGridScript::vPostInit()
{
	auto renderComponent = m_OwnerActor.lock()->getRenderComponent();
	pimpl->m_RenderComponent = renderComponent;

	auto sceneNode = renderComponent->getSceneNode();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	sceneNode->setScaleX(gridSize.width / sceneNode->getContentSize().width);
	sceneNode->setScaleY(gridSize.height / sceneNode->getContentSize().height);
}

const std::string MovingRangeGridScript::Type = "MovingRangeGridScript";

const std::string & MovingRangeGridScript::getType() const
{
	return Type;
}
