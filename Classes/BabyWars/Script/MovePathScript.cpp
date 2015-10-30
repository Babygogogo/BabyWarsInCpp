#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovePathScript.h"
#include "UnitScript.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/GridIndexPath.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovePathScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovePathScript::MovePathScriptImpl
{
	MovePathScriptImpl(){};
	~MovePathScriptImpl(){};

	GridIndexPath m_MovePath;
	std::weak_ptr<UnitScript> m_MovingUnit;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
MovePathScript::MovePathScript() : pimpl{ std::make_unique<MovePathScriptImpl>() }
{
}

MovePathScript::~MovePathScript()
{
}

void MovePathScript::updatePath(const GridIndex & destination, std::shared_ptr<UnitScript> movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap)
{
	assert(movingUnit && "MovePathScript::updatePath() the moving unit is nullptr.");

	//Check if the moving unit is changed. If so, reset the path.
	if (pimpl->m_MovingUnit.expired() || pimpl->m_MovingUnit.lock() != movingUnit){
		pimpl->m_MovePath.clear();
		pimpl->m_MovePath.pushBack(movingUnit->getGridIndex());

		pimpl->m_MovingUnit = movingUnit;
	}

	//Check if the destination is in the path already. If so, just cut the path according to the destination.
	if (pimpl->m_MovePath.findAndCut(destination))
		return;

	//#TODO: Extend the path from its origin end to the destination.
}

bool MovePathScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void MovePathScript::vPostInit()
{
}

const std::string MovePathScript::Type = "MovePathScript";

const std::string & MovePathScript::getType() const
{
	return Type;
}
