#include <list>
#include <unordered_set>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataMakeMovingPathEnd.h"
#include "../Event/EvtDataShowMovingAreaEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/AdjacentDirection.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/MovingArea.h"
#include "../Utilities/MovingPath.h"
#include "MovingPathGridScript.h"
#include "MovingPathScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingPathScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingPathScript::MovingPathScriptImpl
{
	MovingPathScriptImpl() = default;
	~MovingPathScriptImpl() = default;

	void onShowMovingAreaEnd(const EvtDataShowMovingAreaEnd & e);

	bool canBeginMakingMovingPath(const EvtDataInputDrag & drag) const;
	void beginMakingMovingPath(const EvtDataInputDrag & drag);

	bool canContinueMakingMovingPath(const EvtDataInputDrag & drag) const;
	void continueMakingMovingPath(const EvtDataInputDrag & drag);

	bool canEndMakingMovingPath(const EvtDataInputDrag & drag) const;
	void endMakingMovingPath(const EvtDataInputDrag & drag);

	void updateAndShowPath(const GridIndex & destination);
	void clearPath();

	MovingPath createPath(const MovingPath & oldPath, const GridIndex & destination, const MovingArea & area) const;
	void setChildrenGridActors(const MovingPath & path, Actor & self);

	bool isBackIndex(const GridIndex & index) const;
	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	std::string m_MovingPathGridActorPath;

	bool m_IsMakingPath{ false };
	std::unordered_set<ActorID> m_ChildrenGridActorIDs;
	MovingPath m_MovingPath;
	std::weak_ptr<const MovingArea> m_MovingArea;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<TransformComponent> m_TransformComponent;
};

void MovingPathScript::MovingPathScriptImpl::onShowMovingAreaEnd(const EvtDataShowMovingAreaEnd & e)
{
	m_MovingArea = e.getMovingArea();
}

bool MovingPathScript::MovingPathScriptImpl::canBeginMakingMovingPath(const EvtDataInputDrag & drag) const
{
	if (m_IsMakingPath)
		return false;
	if (drag.getState() != EvtDataInputDrag::DragState::Begin)
		return false;
	if (m_MovingArea.expired())
		return false;
	if (toGridIndex(drag.getPreviousPositionInWorld()) != m_MovingArea.lock()->getStartingIndex())
		return false;

	return true;
}

void MovingPathScript::MovingPathScriptImpl::beginMakingMovingPath(const EvtDataInputDrag & drag)
{
	assert(!m_IsMakingPath && "MovingPathScriptImpl::beginMakingMovingPath() the flag for making path is already on.");
	m_IsMakingPath = true;

	updateAndShowPath(toGridIndex(drag.getPositionInWorld()));
}

bool MovingPathScript::MovingPathScriptImpl::canContinueMakingMovingPath(const EvtDataInputDrag & drag) const
{
	if (!m_IsMakingPath)
		return false;
	if (drag.getState() != EvtDataInputDrag::DragState::Dragging)
		return false;
	if (m_MovingArea.expired())
		return false;

	return true;
}

void MovingPathScript::MovingPathScriptImpl::continueMakingMovingPath(const EvtDataInputDrag & drag)
{
	assert(m_IsMakingPath && "MovingPathScriptImpl::continueMakingMovingPath() the flag for making path is off.");

	updateAndShowPath(toGridIndex(drag.getPositionInWorld()));
}

bool MovingPathScript::MovingPathScriptImpl::canEndMakingMovingPath(const EvtDataInputDrag & drag) const
{
	if (!m_IsMakingPath)
		return false;
	if (drag.getState() != EvtDataInputDrag::DragState::End)
		return false;
	if (m_MovingArea.expired())
		return false;

	return true;
}

void MovingPathScript::MovingPathScriptImpl::endMakingMovingPath(const EvtDataInputDrag & drag)
{
	assert(m_IsMakingPath && "MovingPathScriptImpl::endMakingMovingPath() the flag for making path is off.");

	auto isPathValid = isBackIndex(toGridIndex(drag.getPositionInWorld()));
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataMakeMovingPathEnd>(m_MovingPath, isPathValid));

	clearPath();
	m_IsMakingPath = false;
}

void MovingPathScript::MovingPathScriptImpl::updateAndShowPath(const GridIndex & destination)
{
	//If the area is expired, or the destination is not in the area, just do nothing and return.
	if (m_MovingArea.expired())
		return;
	auto area = m_MovingArea.lock();
	if (!area->hasIndex(destination))
		return;

	auto newPath = createPath(m_MovingPath, destination, *area);
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

bool MovingPathScript::MovingPathScriptImpl::isBackIndex(const GridIndex & index) const
{
	if (m_MovingPath.isEmpty())
		return false;

	return m_MovingPath.getBackNode().m_GridIndex == index;
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
	if (pimpl->canBeginMakingMovingPath(drag)) {
		pimpl->beginMakingMovingPath(drag);
		return true;
	}

	if (pimpl->canContinueMakingMovingPath(drag)) {
		pimpl->continueMakingMovingPath(drag);
		return true;
	}

	if (pimpl->canEndMakingMovingPath(drag)) {
		pimpl->endMakingMovingPath(drag);
		return true;
	}

	return false;
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
	eventDispatcher->vAddListener(EvtDataShowMovingAreaEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onShowMovingAreaEnd(static_cast<const EvtDataShowMovingAreaEnd &>(e));
	});
}

const std::string MovingPathScript::Type = "MovingPathScript";

const std::string & MovingPathScript::getType() const
{
	return Type;
}
