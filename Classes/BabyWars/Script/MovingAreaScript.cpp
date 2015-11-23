#include <map>
#include <unordered_set>
#include <list>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataActivateUnitEnd.h"
#include "../Event/EvtDataDeactivateUnitEnd.h"
#include "../Event/EvtDataMakeMovingPathEnd.h"
#include "../Resource/UnitData.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/MovingArea.h"
#include "MovingAreaGridScript.h"
#include "MovingAreaScript.h"
#include "TileMapScript.h"
#include "UnitMapScript.h"
#include "UnitScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingRangeScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingAreaScript::MovingRangeScriptImpl
{
	MovingRangeScriptImpl() = default;
	~MovingRangeScriptImpl() = default;

	void onActivateUnitEnd(const EvtDataActivateUnitEnd & e);
	void onDeactivateUnitEnd(const EvtDataDeactivateUnitEnd & e);
	void onMakeMovingPathEnd(const EvtDataMakeMovingPathEnd & e);

	void clearAndShowArea(const UnitScript & movingUnit);
	void clearArea();

	MovingArea createArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const;
	void setChildrenGridActors(const MovingArea & movingArea, Actor & self);

	MovingArea m_MovingArea;
	std::string m_MovingAreaGridActorPath;
	std::unordered_set<ActorID> m_ChildrenGridActorIDs;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<const TileMapScript> m_TileMapScript;
	std::weak_ptr<const UnitMapScript> m_UnitMapScript;
};

void MovingAreaScript::MovingRangeScriptImpl::onActivateUnitEnd(const EvtDataActivateUnitEnd & e)
{
	clearAndShowArea(*e.getUnit());
}

void MovingAreaScript::MovingRangeScriptImpl::onDeactivateUnitEnd(const EvtDataDeactivateUnitEnd & e)
{
	clearArea();
}

void MovingAreaScript::MovingRangeScriptImpl::onMakeMovingPathEnd(const EvtDataMakeMovingPathEnd & e)
{
	clearArea();
}

void MovingAreaScript::MovingRangeScriptImpl::clearAndShowArea(const UnitScript & movingUnit)
{
	auto newArea = createArea(movingUnit, *m_TileMapScript.lock(), *m_UnitMapScript.lock());
	if (newArea != m_MovingArea) {
		clearArea();

		m_MovingArea = std::move(newArea);
		setChildrenGridActors(m_MovingArea, *m_OwnerActor.lock());
	}
}

void MovingAreaScript::MovingRangeScriptImpl::clearArea()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));

	m_MovingArea.clear();
	m_ChildrenGridActorIDs.clear();
}

MovingArea MovingAreaScript::MovingRangeScriptImpl::createArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const
{
	const auto originIndex = movingUnit.getGridIndex();
	const auto & movementType = movingUnit.getUnitData()->getMovementType();
	auto movingArea = MovingArea(movingUnit.getUnitData()->getMovementRange(), originIndex);
	auto visitedGridList = std::list<GridIndex>{ originIndex };

	//Iterate through the visited list to see if the unit can move further. Uses BFS.
	for (const auto & currentIndex : visitedGridList) {
		for (auto && nextIndex : currentIndex.getAdjacentIndexes()) {
			if (!tileMap.canPassThrough(movementType, nextIndex) || !unitMap.canPassThrough(movingUnit, nextIndex))
				continue;

			auto currentlyRemainingMovementRange = movingArea.getMovingInfo(currentIndex).m_MaxRemainingMovementRange;
			auto movingCost = tileMap.getMovingCost(movementType, nextIndex);
			auto movingInfo = MovingArea::MovingInfo(movingCost, currentlyRemainingMovementRange - movingCost, unitMap.canUnitStayAtIndex(movingUnit, nextIndex), currentIndex);

			//Add the nextIndex into the visited list if we can update the area using the index.
			if (movingArea.tryUpdateIndex(nextIndex, std::move(movingInfo)))
				visitedGridList.emplace_back(std::move(nextIndex));
		}
	}

	return movingArea;
}

void MovingAreaScript::MovingRangeScriptImpl::setChildrenGridActors(const MovingArea & movingArea, Actor & self)
{
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & index : movingArea.getAllIndexesInArea()) {
		auto gridActor = gameLogic->createActor(m_MovingAreaGridActorPath.c_str());
		auto gridScript = gridActor->getComponent<MovingAreaGridScript>();
		gridScript->setGridIndexAndPosition(index);

		self.addChild(*gridActor);
		m_ChildrenGridActorIDs.emplace(gridActor->getID());
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingAreaScript::MovingAreaScript() : pimpl{ std::make_shared<MovingRangeScriptImpl>() }
{
}

MovingAreaScript::~MovingAreaScript()
{
}

void MovingAreaScript::setTileMapScript(std::weak_ptr<const TileMapScript> && tileMapScript)
{
	pimpl->m_TileMapScript = std::move(tileMapScript);
}

void MovingAreaScript::setUnitMapScript(std::weak_ptr<const UnitMapScript> && unitMapScript)
{
	pimpl->m_UnitMapScript = std::move(unitMapScript);
}

void MovingAreaScript::clearAndShowArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap)
{
	auto newArea = pimpl->createArea(movingUnit, tileMap, unitMap);
	if (newArea != pimpl->m_MovingArea) {
		clearArea();

		pimpl->m_MovingArea = std::move(newArea);
		pimpl->setChildrenGridActors(pimpl->m_MovingArea, *m_OwnerActor.lock());
	}
}

void MovingAreaScript::clearArea()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : pimpl->m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));

	pimpl->m_MovingArea.clear();
	pimpl->m_ChildrenGridActorIDs.clear();
}

const MovingArea & MovingAreaScript::getUnderlyingArea() const
{
	return pimpl->m_MovingArea;
}

bool MovingAreaScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	auto relatedActorsPath = xmlElement->FirstChildElement("RelatedActorsPath");
	pimpl->m_MovingAreaGridActorPath = relatedActorsPath->Attribute("MovingAreaGrid");

	return true;
}

void MovingAreaScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;

	auto transformComponent = m_OwnerActor.lock()->getComponent<TransformComponent>();
	assert(transformComponent && "MovingAreaScript::vPostInit() the actor has no transform component");
	pimpl->m_TransformComponent = std::move(transformComponent);

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataActivateUnitEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onActivateUnitEnd(static_cast<const EvtDataActivateUnitEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataDeactivateUnitEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onDeactivateUnitEnd(static_cast<const EvtDataDeactivateUnitEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataMakeMovingPathEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onMakeMovingPathEnd(static_cast<const EvtDataMakeMovingPathEnd &>(e));
	});
}

const std::string MovingAreaScript::Type = "MovingAreaScript";

const std::string & MovingAreaScript::getType() const
{
	return Type;
}
