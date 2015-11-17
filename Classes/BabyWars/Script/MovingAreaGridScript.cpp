#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyWars/Resource/ResourceLoader.h"
#include "../Utilities/GridIndex.h"
#include "MovingAreaGridScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingAreaGridScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingAreaGridScript::MovingAreaGridScriptImpl
{
	MovingAreaGridScriptImpl() = default;
	~MovingAreaGridScriptImpl() = default;

	GridIndex m_GridIndex;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingAreaGridScript::MovingAreaGridScript() : pimpl{ std::make_unique<MovingAreaGridScriptImpl>() }
{
}

MovingAreaGridScript::~MovingAreaGridScript()
{
}

void MovingAreaGridScript::setGridIndexAndPosition(const GridIndex & index)
{
	pimpl->m_GridIndex = index;

	auto renderComponent = pimpl->m_RenderComponent.lock();
	renderComponent->getSceneNode()->setPosition(index.toPosition(SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize()));
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
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	sceneNode->setScaleX(gridSize.width / sceneNode->getContentSize().width);
	sceneNode->setScaleY(gridSize.height / sceneNode->getContentSize().height);
}

const std::string MovingAreaGridScript::Type = "MovingAreaGridScript";

const std::string & MovingAreaGridScript::getType() const
{
	return Type;
}
