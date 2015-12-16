#include <vector>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
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
//Definition of MovingAreaScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingAreaScript::MovingAreaScriptImpl
{
	MovingAreaScriptImpl() = default;
	~MovingAreaScriptImpl() = default;

	using GridActors = std::vector<std::weak_ptr<Actor>>;
	struct MovingAreaStruct
	{
		MovingAreaStruct(std::shared_ptr<MovingArea> logicalArea, GridActors && gridActors) : m_LogicalArea{ std::move(logicalArea) }, m_GridActors{ std::move(gridActors) } {}

		std::shared_ptr<MovingArea> m_LogicalArea;
		GridActors m_GridActors;
	};

	void onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e);

	bool isAreaShownForUnit(const UnitScript & unit) const;
	void createAndShowAreaForUnit(const UnitScript & unit);
	void clearAndDestroyAreaForUnit(const UnitScript & unit);

	std::shared_ptr<MovingArea> _createLogicalArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const;
	GridActors _createGridActorsForLogicalArea(const MovingArea & logicalArea) const;

	void _showAndAddChildGridActors(const GridActors & gridActors);
	void _hideAndRemoveChildGridActors(const GridActors & gridActors);

	static std::string s_MovingAreaGridActorPath;

	std::map<const UnitScript *, MovingAreaStruct> m_MovingAreas;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<const TileMapScript> m_TileMapScript;
	std::weak_ptr<const UnitMapScript> m_UnitMapScript;
};

std::string MovingAreaScript::MovingAreaScriptImpl::s_MovingAreaGridActorPath;

void MovingAreaScript::MovingAreaScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	auto unitScript = e.getUnitScript();
	assert(unitScript && "MovingAreaScriptImpl::onUnitStateChangeEnd() the unit is expired.");
	e.getCurrentState()->vUpdateMovingArea(*m_OwnerActor.lock()->getComponent<MovingAreaScript>(), *unitScript);
}

bool MovingAreaScript::MovingAreaScriptImpl::isAreaShownForUnit(const UnitScript & unit) const
{
	return m_MovingAreas.find(&unit) != m_MovingAreas.end();
}

void MovingAreaScript::MovingAreaScriptImpl::createAndShowAreaForUnit(const UnitScript & unit)
{
	if (isAreaShownForUnit(unit)) {
		return;
	}

	auto logicalArea = _createLogicalArea(unit, *m_TileMapScript.lock(), *m_UnitMapScript.lock());
	auto gridActors = _createGridActorsForLogicalArea(*logicalArea);
	_showAndAddChildGridActors(gridActors);
	m_MovingAreas.emplace(std::make_pair(&unit, MovingAreaStruct(logicalArea, std::move(gridActors))));

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataShowMovingAreaEnd>(logicalArea));
}

void MovingAreaScript::MovingAreaScriptImpl::clearAndDestroyAreaForUnit(const UnitScript & unit)
{
	auto areaIter = m_MovingAreas.find(&unit);
	if (areaIter == m_MovingAreas.end()) {
		return;
	}

	_hideAndRemoveChildGridActors(areaIter->second.m_GridActors);
	m_MovingAreas.erase(areaIter);
}

std::shared_ptr<MovingArea> MovingAreaScript::MovingAreaScriptImpl::_createLogicalArea(const UnitScript & movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap) const
{
	const auto originIndex = movingUnit.getGridIndex();
	const auto & movementType = movingUnit.getUnitData()->getMovementType();
	auto logicalArea = std::make_shared<MovingArea>(movingUnit.getUnitData()->getMovementRange(), originIndex);
	auto visitedGridList = std::list<GridIndex>{ originIndex };

	//Iterate through the visited list to see if the unit can move further. Uses BFS.
	for (const auto & currentIndex : visitedGridList) {
		for (auto && nextIndex : currentIndex.getAdjacentIndexes()) {
			if (!tileMap.canPassThrough(movementType, nextIndex) || !unitMap.canPassThrough(movingUnit, nextIndex))
				continue;

			auto currentlyRemainingMovementRange = logicalArea->getMovingInfo(currentIndex).m_MaxRemainingMovementRange;
			auto movingCost = tileMap.getMovingCost(movementType, nextIndex);
			auto movingInfo = MovingArea::MovingInfo(movingCost, currentlyRemainingMovementRange - movingCost, unitMap.canUnitStayAtIndex(movingUnit, nextIndex), currentIndex);

			//Add the nextIndex into the visited list if we can update the area using the index.
			if (logicalArea->tryUpdateIndex(nextIndex, std::move(movingInfo)))
				visitedGridList.emplace_back(std::move(nextIndex));
		}
	}

	return logicalArea;
}

MovingAreaScript::MovingAreaScriptImpl::GridActors MovingAreaScript::MovingAreaScriptImpl::_createGridActorsForLogicalArea(const MovingArea & logicalArea) const
{
	auto gridActors = GridActors{};
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & index : logicalArea.getAllIndexesInArea()) {
		auto gridActor = gameLogic->createActor(s_MovingAreaGridActorPath.c_str());
		auto gridScript = gridActor->getComponent<MovingAreaGridScript>();
		gridScript->setGridIndexAndPosition(index);

		gridActors.emplace_back(gridActor);
	}

	return gridActors;
}

void MovingAreaScript::MovingAreaScriptImpl::_showAndAddChildGridActors(const GridActors & gridActors)
{
	auto ownerActor = m_OwnerActor.lock();
	auto ownerSceneNode = ownerActor->getRenderComponent()->getSceneNode();

	for (const auto & weakGridActor : gridActors) {
		if (weakGridActor.expired()) {
			continue;
		}

		auto strongGridActor = weakGridActor.lock();
		ownerActor->addChild(*strongGridActor);
		//ownerSceneNode->addChild(strongGridActor->getRenderComponent()->getSceneNode());
	}
}

void MovingAreaScript::MovingAreaScriptImpl::_hideAndRemoveChildGridActors(const GridActors & gridActors)
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();

	for (const auto & weakGridActor : gridActors) {
		if (weakGridActor.expired()) {
			continue;
		}

		auto strongGridActor = weakGridActor.lock();
		strongGridActor->removeFromParent();
		strongGridActor->getRenderComponent()->getSceneNode()->removeFromParent();
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(strongGridActor->getID()));
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingAreaScript::MovingAreaScript() : pimpl{ std::make_shared<MovingAreaScriptImpl>() }
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

bool MovingAreaScript::isAreaShownForUnit(const UnitScript & unit) const
{
	return pimpl->isAreaShownForUnit(unit);
}

void MovingAreaScript::showAreaForUnit(const UnitScript & unit)
{
	pimpl->createAndShowAreaForUnit(unit);
}

void MovingAreaScript::clearAreaForUnit(const UnitScript & unit)
{
	pimpl->clearAndDestroyAreaForUnit(unit);
}

bool MovingAreaScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static bool isStaticMemberInitialized = false;
	if (isStaticMemberInitialized) {
		return true;
	}

	auto relatedActorsPath = xmlElement->FirstChildElement("RelatedActorsPath");
	MovingAreaScriptImpl::s_MovingAreaGridActorPath = relatedActorsPath->Attribute("MovingAreaGrid");

	isStaticMemberInitialized = true;
	return true;
}

void MovingAreaScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;

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
