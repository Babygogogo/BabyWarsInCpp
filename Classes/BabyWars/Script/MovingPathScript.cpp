#include <list>
#include <unordered_map>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingPathScript.h"
#include "UnitScript.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/AdjacentDirection.h"
#include "../Utilities/MovingArea.h"
#include "../Utilities/MovingPath.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingPathScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingPathScript::MovingPathScriptImpl
{
	MovingPathScriptImpl() = default;
	~MovingPathScriptImpl() = default;

	//The returned value indicates if the path is updated or not.
	bool createPath(const GridIndex & destination, const MovingArea & area);
	void setChildrenGridActorWithPath(const MovingPath & path);

	MovingPath m_MovingPath;
};

bool MovingPathScript::MovingPathScriptImpl::createPath(const GridIndex & destination, const MovingArea & area)
{
	//If the destination is in the path already, cut the path and return.
	if (m_MovingPath.hasIndex(destination))
		return m_MovingPath.tryFindAndCut(destination);

	//Init the path if it's empty.
	if (!m_MovingPath.isEmpty()) {
		auto startingIndex = area.getStartingIndex();
		auto startingInfo = area.getMovingInfo(startingIndex);

		m_MovingPath.init(MovingPath::PathNode(startingIndex, startingInfo.m_MaxRemainingMovementRange));
	}

	//The destination is not in the path. Try extending the path to destination. Return if succeed.
	if (m_MovingPath.tryExtend(destination, area.getMovingInfo(destination).m_MovingCost))
		return true;

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

	m_MovingPath.init(std::move(pathNodeList));
	return true;
}

void MovingPathScript::MovingPathScriptImpl::setChildrenGridActorWithPath(const MovingPath & path)
{
	for (const auto & pathNode : path.getUnderlyingPath()) {
		auto index = pathNode.m_GridIndex;

		auto previousDirection = AdjacentDirection::INVALID;
		if (path.hasPreviousOf(index))
			previousDirection = index.getAdjacentDirectionOf(path.getPreviousNodeOf(index).m_GridIndex);

		auto nextDirection = AdjacentDirection::INVALID;
		if (path.hasNextOf(index))
			nextDirection = index.getAdjacentDirectionOf(path.getNextNodeOf(index).m_GridIndex);

		//#TODO:
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

	if (pimpl->createPath(destination, area))
		pimpl->setChildrenGridActorWithPath(pimpl->m_MovingPath);
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
