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
	MovePathScriptImpl() = default;
	~MovePathScriptImpl() = default;

	GridIndexPath m_MovePath;
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

void MovingPathScript::showPath(const GridIndex & destination, const MovingArea & area)
{
}

void MovingPathScript::clearPath()
{
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
