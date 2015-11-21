#include <list>
#include <unordered_set>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
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

	MovingPath createPath(const MovingPath & oldPath, const GridIndex & destination, const MovingArea & area) const;
	void setChildrenGridActors(const MovingPath & path, Actor & self);

	std::string m_MovingPathGridActorPath;

	std::unordered_set<ActorID> m_ChildrenGridActorIDs;
	MovingPath m_MovingPath;
};

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

//////////////////////////////////////////////////////////////////////////
//Implementation of MovingPathScript.
//////////////////////////////////////////////////////////////////////////
MovingPathScript::MovingPathScript() : pimpl{ std::make_unique<MovingPathScriptImpl>() }
{
}

MovingPathScript::~MovingPathScript()
{
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
}

const std::string MovingPathScript::Type = "MovingPathScript";

const std::string & MovingPathScript::getType() const
{
	return Type;
}
