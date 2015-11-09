#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingPathScript.h"
#include "UnitScript.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/GridIndexPath.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovePathScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingPathScript::MovePathScriptImpl
{
	MovePathScriptImpl(){};
	~MovePathScriptImpl(){};

	GridIndexPath m_MovePath;
	std::weak_ptr<UnitScript> m_MovingUnit;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovingPathScript::MovingPathScript() : pimpl{ std::make_unique<MovePathScriptImpl>() }
{
}

MovingPathScript::~MovingPathScript()
{
}

void MovingPathScript::showPath(const GridIndex & destination, std::shared_ptr<UnitScript> movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap)
{
	//Check if the moving unit is changed. If so, reset the path.
	//if (pimpl->m_MovingUnit.expired() || pimpl->m_MovingUnit.lock() != movingUnit){
	//	pimpl->m_MovePath.clear();
	//	pimpl->m_MovePath.pushBack(movingUnit->getGridIndex());

	//	pimpl->m_MovingUnit = movingUnit;
	//}

	//Check if the destination is in the path already. If so, just cut the path according to the destination.
	//if (pimpl->m_MovePath.findAndCut(destination))
	//	return;

	//#TODO: Extend the path from its origin end to the destination.
}

bool MovingPathScript::vInit(tinyxml2::XMLElement *xmlElement)
{
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
