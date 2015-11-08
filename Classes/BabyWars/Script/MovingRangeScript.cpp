#include <unordered_set>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingRangeScript.h"
#include "MovingRangeGridScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/GridIndex.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingRangeScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingRangeScript::MovingRangeScriptImpl
{
	MovingRangeScriptImpl(){};
	~MovingRangeScriptImpl(){};

	void createChildGridActorAtIndex(const GridIndex & index, BaseGameLogic & gameLogic, Actor & self);

	std::string m_MovingRangeGridActorPath;
	std::unordered_set<ActorID> m_ChildrenGrid;
};

void MovingRangeScript::MovingRangeScriptImpl::createChildGridActorAtIndex(const GridIndex & index, BaseGameLogic & gameLogic, Actor & self)
{
	auto gridActor = gameLogic.createActor(m_MovingRangeGridActorPath.c_str());
	auto gridScript = gridActor->getComponent<MovingRangeGridScript>();
	gridScript->setGridIndexAndPosition(index);
	gridScript->setVisible(true);

	m_ChildrenGrid.emplace(gridActor->getID());
	self.addChild(*gridActor);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingRangeScript::MovingRangeScript() : pimpl{ std::make_unique<MovingRangeScriptImpl>() }
{
}

MovingRangeScript::~MovingRangeScript()
{
}

void MovingRangeScript::showRange(const GridIndex & gridIndex)
{
	//#TODO: Only for testing and should be modified so that the range is displayed correctly.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	pimpl->createChildGridActorAtIndex(gridIndex, *gameLogic, *m_OwnerActor.lock());
}

void MovingRangeScript::clearRange()
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & actorID : pimpl->m_ChildrenGrid)
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(actorID));
	pimpl->m_ChildrenGrid.clear();
}

bool MovingRangeScript::vInit(tinyxml2::XMLElement * xmlElement)
{
	auto relatedActorsPath = xmlElement->FirstChildElement("RelatedActorsPath");
	pimpl->m_MovingRangeGridActorPath = relatedActorsPath->Attribute("MovingRangeGrid");

	return true;
}

void MovingRangeScript::vPostInit()
{
}

const std::string MovingRangeScript::Type = "MovingRangeScript";

const std::string & MovingRangeScript::getType() const
{
	return Type;
}
