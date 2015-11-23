#include <list>
#include <unordered_set>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataMakeMovingPathBegin.h"
#include "../Event/EvtDataMakeMovingPathEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/AdjacentDirection.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/MovingArea.h"
#include "../Utilities/MovingPath.h"
#include "MovingAreaScript.h"
#include "MovingPathGridScript.h"
#include "MovingPathScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingPathScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingPathScript::MovingPathScriptImpl
{
	MovingPathScriptImpl() = default;
	~MovingPathScriptImpl() = default;

	void onMakeMovingPathBegin(const EvtDataMakeMovingPathBegin & e);

	void showPath(const GridIndex & destination);
	void clearPath();

	MovingPath createPath(const MovingPath & oldPath, const GridIndex & destination, const MovingArea & area) const;
	void setChildrenGridActors(const MovingPath & path, Actor & self);

	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	std::string m_MovingPathGridActorPath;

	bool m_IsMakingPath{ false };
	std::unordered_set<ActorID> m_ChildrenGridActorIDs;
	MovingPath m_MovingPath;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<const MovingAreaScript> m_MovingAreaScript;
};

void MovingPathScript::MovingPathScriptImpl::onMakeMovingPathBegin(const EvtDataMakeMovingPathBegin & e)
{
	assert(!m_IsMakingPath && "MovingPathScriptImpl::onMakeMovingPathBegin() the flag for making path is already on.");
	m_IsMakingPath = true;

	showPath(e.getGridIndex());
}

void MovingPathScript::MovingPathScriptImpl::showPath(const GridIndex & destination)
{
	//If the destination is not in the area, just do nothing and return.
	const auto & area = m_MovingAreaScript.lock()->getUnderlyingArea();
	if (!area.hasIndex(destination))
		return;

	auto newPath = createPath(m_MovingPath, destination, area);
	if (newPath != m_MovingPath) {
		clearPath();

		m_MovingPath = std::move(newPath);
		setChildrenGridActors(m_MovingPath, *m_OwnerActor.lock());
	}
}

void MovingPathScript::MovingPathScriptImpl::clearPath()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));

	m_MovingPath.clear();
	m_ChildrenGridActorIDs.clear();
}

MovingPath MovingPathScript::MovingPathScriptImpl::createPath(const MovingPath & oldPath, const GridIndex & destination, const MovingArea & area) const
{
	auto newPath = oldPath;

	//Init the path if it's empty.
	if (newPath.isEmpty()) {
		auto startingIndex = area.getStartingIndex();
		auto startingInfo = area.getMovingInfo(startingIndex);
		newPath.init(MovingPath::PathNode(startingIndex, startingInfo.m_MaxRemainingMovementRange));
	}

	//If the destination is in the path already, cut the path and return.
	if (newPath.hasIndex(destination)) {
		newPath.tryFindAndCut(destination);
		return newPath;
	}

	//The destination is not in the path. Try extending the path to destination. Return the new path if succeed.
	if (newPath.tryExtend(destination, area.getMovingInfo(destination).m_MovingCost))
		return newPath;

	//The path can't be extended (because the remaining movement range is not big enough).
	//Generate a new shortest path.
	auto pathNodeList = std::list<MovingPath::PathNode>();
	auto currentIndex = destination;
	auto currentInfo = area.getMovingInfo(currentIndex);
	auto previousIndex = currentInfo.m_PreviousIndex;
	auto startingIndex = area.getStartingIndex();

	while (currentIndex != startingIndex) {
		pathNodeList.emplace_front(MovingPath::PathNode(currentIndex, currentInfo.m_MaxRemainingMovementRange));
		currentIndex = previousIndex;
		currentInfo = area.getMovingInfo(currentIndex);
		previousIndex = currentInfo.m_PreviousIndex;
	}
	pathNodeList.emplace_front(MovingPath::PathNode(startingIndex, area.getMovingInfo(startingIndex).m_MaxRemainingMovementRange));

	newPath.init(std::move(pathNodeList));
	return newPath;
}

void MovingPathScript::MovingPathScriptImpl::setChildrenGridActors(const MovingPath & path, Actor & self)
{
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & pathNode : path.getUnderlyingPath()) {
		const auto & index = pathNode.m_GridIndex;

		auto previousDirection = AdjacentDirection::INVALID;
		if (path.hasPreviousOf(index))
			previousDirection = path.getPreviousNodeOf(index).m_GridIndex.getAdjacentDirectionOf(index);

		auto nextDirection = AdjacentDirection::INVALID;
		if (path.hasNextOf(index))
			nextDirection = path.getNextNodeOf(index).m_GridIndex.getAdjacentDirectionOf(index);

		//Create the grid actor and attach it to self.
		auto gridActor = gameLogic->createActor(m_MovingPathGridActorPath.c_str());
		auto gridScript = gridActor->getComponent<MovingPathGridScript>();
		gridScript->setAppearanceAndPosition(index, previousDirection, nextDirection);

		self.addChild(*gridActor);
		m_ChildrenGridActorIDs.emplace(gridActor->getID());
	}
}

GridIndex MovingPathScript::MovingPathScriptImpl::toGridIndex(const cocos2d::Vec2 & positionInWindow) const
{
	auto positionInMap = m_TransformComponent.lock()->convertToLocalSpace(positionInWindow);
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	return GridIndex(positionInMap, gridSize);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MovingPathScript.
//////////////////////////////////////////////////////////////////////////
MovingPathScript::MovingPathScript() : pimpl{ std::make_shared<MovingPathScriptImpl>() }
{
}

MovingPathScript::~MovingPathScript()
{
}

bool MovingPathScript::onInputDrag(const EvtDataInputDrag & drag)
{
	if (!pimpl->m_IsMakingPath)
		return false;

	const auto dragState = drag.getState();
	assert(dragState != EvtDataInputDrag::DragState::Begin && "MovingPathScript::onInputDrag() the drag is in begin state while it's already making path.");

	if (dragState == EvtDataInputDrag::DragState::Dragging) {
		pimpl->showPath(pimpl->toGridIndex(drag.getPositionInWorld()));
		return true;
	}

	if (dragState == EvtDataInputDrag::DragState::End) {
		auto isPathValid = isBackIndex(pimpl->toGridIndex(drag.getPositionInWorld()));
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataMakeMovingPathEnd>(pimpl->m_MovingPath, isPathValid));

		pimpl->clearPath();
		pimpl->m_IsMakingPath = false;

		return true;
	}

	return false;
}

void MovingPathScript::setMovingAreaScript(std::weak_ptr<const MovingAreaScript> && movingAreaScript)
{
	pimpl->m_MovingAreaScript = std::move(movingAreaScript);
}

void MovingPathScript::showPath(const GridIndex & destination, const MovingArea & area)
{
	//If the destination is not in the area, just do nothing and return.
	if (!area.hasIndex(destination))
		return;

	auto newPath = pimpl->createPath(pimpl->m_MovingPath, destination, area);
	if (newPath != pimpl->m_MovingPath) {
		clearPath();

		pimpl->m_MovingPath = std::move(newPath);
		pimpl->setChildrenGridActors(pimpl->m_MovingPath, *m_OwnerActor.lock());
	}
}

void MovingPathScript::clearPath()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : pimpl->m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));

	pimpl->m_MovingPath.clear();
	pimpl->m_ChildrenGridActorIDs.clear();
}

bool MovingPathScript::isBackIndex(const GridIndex & index) const
{
	if (pimpl->m_MovingPath.isEmpty())
		return false;

	return pimpl->m_MovingPath.getBackNode().m_GridIndex == index;
}

const MovingPath & MovingPathScript::getUnderlyingPath() const
{
	return pimpl->m_MovingPath;
}

bool MovingPathScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	auto relatedActorsPath = xmlElement->FirstChildElement("RelatedActorsPath");
	pimpl->m_MovingPathGridActorPath = relatedActorsPath->Attribute("MovingPathGrid");

	return true;
}

void MovingPathScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;

	auto transformComponent = m_OwnerActor.lock()->getComponent<TransformComponent>();
	assert(transformComponent && "MovingPathScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataMakeMovingPathBegin::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onMakeMovingPathBegin(static_cast<const EvtDataMakeMovingPathBegin &>(e));
	});
}

const std::string MovingPathScript::Type = "MovingPathScript";

const std::string & MovingPathScript::getType() const
{
	return Type;
}
