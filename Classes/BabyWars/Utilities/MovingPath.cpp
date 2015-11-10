#include <cassert>
#include <algorithm>

#include "MovingPath.h"

MovingPath::PathNode::PathNode(const GridIndex & index, int remainingMovementRange) : m_GridIndex{ index }, m_RemainingMovementRange{ remainingMovementRange }
{
}

bool MovingPath::isInitialized() const
{
	return !m_Path.empty();
}

void MovingPath::init(PathNode && pathNode)
{
	m_Path.clear();
	m_Path.emplace_back(std::move(pathNode));
}

void MovingPath::init(std::list<PathNode> && path)
{
	m_Path.clear();
	for (auto && node : std::move(path))
		m_Path.emplace_back(std::move(node));
}

void MovingPath::clear()
{
	m_Path.clear();
}

MovingPath::PathNode MovingPath::getBackNode() const
{
	assert(isInitialized() && "MovingPath::getBackNode() the path is not initialized.");
	return m_Path.back();
}

bool MovingPath::hasIndex(const GridIndex & index) const
{
	for (const auto & node : m_Path)
		if (node.m_GridIndex == index)
			return true;

	return false;
}

bool MovingPath::tryFindAndCut(const GridIndex & index)
{
	auto existingIter = std::find_if(m_Path.begin(), m_Path.end(), [index](const MovingPath::PathNode & node){return node.m_GridIndex == index; });
	if (existingIter == m_Path.end())
		return false;

	m_Path.erase(++existingIter, m_Path.end());
	return true;
}

bool MovingPath::tryExtend(const GridIndex & destination, int movingCost)
{
	auto backNode = getBackNode();
	if (!backNode.m_GridIndex.isAdjacentTo(destination))
		return false;

	if (hasIndex(destination))
		return false;

	auto remainingMovementRange = backNode.m_RemainingMovementRange - movingCost;
	if (remainingMovementRange < 0)
		return false;

	m_Path.emplace_back(PathNode(destination, remainingMovementRange));
	return true;
}

const std::vector<MovingPath::PathNode> & MovingPath::getPath() const
{
	return m_Path;
}
