#include <map>
#include <unordered_set>
#include <list>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingAreaScript.h"
#include "MovingAreaGridScript.h"
#include "UnitMapScript.h"
#include "UnitScript.h"
#include "TileMapScript.h"
#include "../Resource/UnitData.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/GridIndex.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingRangeScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingAreaScript::MovingRangeScriptImpl
{
	MovingRangeScriptImpl() = default;
	~MovingRangeScriptImpl() = default;

	std::list<GridIndex> calculateRange(const GridIndex & gridIndex, const TileMapScript & tileMap, const UnitMapScript & unitMap);
	void setChildrenGridActorWithIndexes(const std::list<GridIndex> & indexes, Actor & self);

	std::string m_MovingRangeGridActorPath;
	std::unordered_set<ActorID> m_ChildrenGridActorIDs;
};

std::list<GridIndex> MovingAreaScript::MovingRangeScriptImpl::calculateRange(const GridIndex & gridIndex, const TileMapScript & tileMap, const UnitMapScript & unitMap)
{
	auto unit = unitMap.getUnit(gridIndex);
	if (!unit)
		return{};

	const auto & movementType = unit->getUnitData()->getMovementType();
	auto visitedGridAndMovement = std::map<GridIndex, int>{std::make_pair(gridIndex, unit->getUnitData()->getMovement())};
	auto visitedGridList = std::list<GridIndex>{gridIndex};

	//Iterate through the visited list to see if the unit can move further.
	for (const auto & visitedIndex : visitedGridList){
		for (const auto & neighbor : visitedIndex.getNeighbors()){
			//By now, don't check if there is a unit in the neighbor grid (this means that the unit can pass through a grid with unit).
			//TODO: Check if there is a unit and if it's valid to pass through it.

			//Get the moving cost. If the moving cost <= 0, which is invalid, continue.
			auto movingCost = tileMap.getMovingCost(movementType, neighbor);
			if (movingCost <= 0)
				continue;

			//Calculate the remaining movement of the unit after it moves to the neighbor grid.
			auto remainingMovement = visitedGridAndMovement[visitedIndex] - movingCost;
			//If the remaining movement is less than 0, which is invalid, continue.
			if (remainingMovement < 0)
				continue;

			//Update the visited grid and movement list.
			auto visitedIter = visitedGridAndMovement.find(neighbor);
			if (visitedIter == visitedGridAndMovement.end()){
				//If the neighbor is not in the list, add it.
				visitedGridAndMovement.emplace(neighbor, remainingMovement);
				visitedGridList.emplace_back(neighbor);
			}
			else{
				//The neighbor is in the list already. Update the list if the remainingMovement is larger than the value in the list.
				if (remainingMovement > visitedIter->second){
					visitedIter->second = remainingMovement;
					visitedGridList.emplace_back(neighbor);
				}
			}
		}
	}

	return visitedGridList;
}

void MovingAreaScript::MovingRangeScriptImpl::setChildrenGridActorWithIndexes(const std::list<GridIndex> & indexes, Actor & self)
{
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & index : indexes){
		auto gridActor = gameLogic->createActor(m_MovingRangeGridActorPath.c_str());
		auto gridScript = gridActor->getComponent<MovingAreaGridScript>();
		gridScript->setGridIndexAndPosition(index);
		gridScript->setVisible(true);

		self.addChild(*gridActor);
		m_ChildrenGridActorIDs.emplace(gridActor->getID());
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingAreaScript::MovingAreaScript() : pimpl{ std::make_unique<MovingRangeScriptImpl>() }
{
}

MovingAreaScript::~MovingAreaScript()
{
}

void MovingAreaScript::showRange(const GridIndex & gridIndex, const TileMapScript & tileMap, const UnitMapScript & unitMap)
{
	pimpl->setChildrenGridActorWithIndexes(pimpl->calculateRange(gridIndex, tileMap, unitMap), *m_OwnerActor.lock());
}

void MovingAreaScript::clearRange()
{
	m_OwnerActor.lock()->removeAllChildren();
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : pimpl->m_ChildrenGridActorIDs)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));
	pimpl->m_ChildrenGridActorIDs.clear();
}

bool MovingAreaScript::vInit(tinyxml2::XMLElement * xmlElement)
{
	auto relatedActorsPath = xmlElement->FirstChildElement("RelatedActorsPath");
	pimpl->m_MovingRangeGridActorPath = relatedActorsPath->Attribute("MovingRangeGrid");

	return true;
}

void MovingAreaScript::vPostInit()
{
}

const std::string MovingAreaScript::Type = "MovingAreaScript";

const std::string & MovingAreaScript::getType() const
{
	return Type;
}
