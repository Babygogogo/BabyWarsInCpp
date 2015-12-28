#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
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

	std::weak_ptr<TransformComponent> m_TransformComponent;
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
	pimpl->m_TransformComponent.lock()->setPosition(index.toPosition(SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize()));
}

bool MovingAreaGridScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void MovingAreaGridScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "MovingAreaGridScript::vPostInit() the actor has no render component.");
	pimpl->m_RenderComponent = renderComponent;

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "MovingAreaGridScript::vPostInit() the actor has no tranform component.");
	pimpl->m_TransformComponent = transformComponent;
}

const std::string MovingAreaGridScript::Type = "MovingAreaGridScript";

const std::string & MovingAreaGridScript::getType() const
{
	return Type;
}
