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
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Event/EvtDataShowMovingAreaEnd.h"
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

	void onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e);

	bool isAreaShownForUnit(const UnitScript & unitScript) const;

	void createAndShowArea(const std::weak_ptr<const UnitScript> activeUnit);
	void clearArea();

	std::shared_ptr<MovingArea> createArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const;
	void setChildrenGridActors(const MovingArea & movingArea, Actor & self);

	std::weak_ptr<const UnitScript> m_FocusUnit;
	std::shared_ptr<MovingArea> m_MovingArea;

	std::string m_MovingAreaGridActorPath;
	std::unordered_set<ActorID> m_ChildrenGridActorIDs;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<const TileMapScript> m_TileMapScript;
	std::weak_ptr<const UnitMapScript> m_UnitMapScript;
};

void MovingAreaScript::MovingRangeScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	auto currentState = e.getCurrentState();
	auto previousState = e.getPreviousState();
	if (currentState == previousState) {
		return;
	}

	if (currentState == UnitState::Active) {
		assert(!isAreaShownForUnit(*e.getUnitScript()) && "MovingRangeScriptImpl::onUnitStateChangeEnd() the moving area for the unit is already shown.");

		clearArea();
		createAndShowArea(e.getUnitScript());
	}
	else {
		if (isAreaShownForUnit(*e.getUnitScript())) {
			clearArea();
		}
	}
}

bool MovingAreaScript::MovingRangeScriptImpl::isAreaShownForUnit(const UnitScript & unitScript) const
{
	if (m_FocusUnit.expired())
		return false;

	return m_FocusUnit.lock().get() == &unitScript;
}

void MovingAreaScript::MovingRangeScriptImpl::createAndShowArea(const std::weak_ptr<const UnitScript> activeUnit)
{
	assert(!activeUnit.expired() && "MovingRangeScriptImpl::showArea() the active unit is nullptr.");

	m_FocusUnit = activeUnit;
	m_MovingArea = createArea(*activeUnit.lock(), *m_TileMapScript.lock(), *m_UnitMapScript.lock());
	setChildrenGridActors(*m_MovingArea, *m_OwnerActor.lock());

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataShowMovingAreaEnd>(m_MovingArea));
}

void MovingAreaScript::MovingRangeScriptImpl::clearArea()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));

	m_FocusUnit.reset();
	m_MovingArea.reset();
	m_ChildrenGridActorIDs.clear();
}

std::shared_ptr<MovingArea> MovingAreaScript::MovingRangeScriptImpl::createArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const
{
	const auto originIndex = movingUnit.getGridIndex();
	const auto & movementType = movingUnit.getUnitData()->getMovementType();
	auto movingArea = std::make_shared<MovingArea>(movingUnit.getUnitData()->getMovementRange(), originIndex);
	auto visitedGridList = std::list<GridIndex>{ originIndex };

	//Iterate through the visited list to see if the unit can move further. Uses BFS.
	for (const auto & currentIndex : visitedGridList) {
		for (auto && nextIndex : currentIndex.getAdjacentIndexes()) {
			if (!tileMap.canPassThrough(movementType, nextIndex) || !unitMap.canPassThrough(movingUnit, nextIndex))
				continue;

			auto currentlyRemainingMovementRange = movingArea->getMovingInfo(currentIndex).m_MaxRemainingMovementRange;
			auto movingCost = tileMap.getMovingCost(movementType, nextIndex);
			auto movingInfo = MovingArea::MovingInfo(movingCost, currentlyRemainingMovementRange - movingCost, unitMap.canUnitStayAtIndex(movingUnit, nextIndex), currentIndex);

			//Add the nextIndex into the visited list if we can update the area using the index.
			if (movingArea->tryUpdateIndex(nextIndex, std::move(movingInfo)))
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
	eventDispatcher->vAddListener(EvtDataUnitStateChangeEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onUnitStateChangeEnd(static_cast<const EvtDataUnitStateChangeEnd &>(e));
	});
}

const std::string MovingAreaScript::Type = "MovingAreaScript";

const std::string & MovingAreaScript::getType() const
{
	return Type;
}
