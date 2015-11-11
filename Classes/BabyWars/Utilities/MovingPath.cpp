#include <cassert>
#include <algorithm>

#include "MovingPath.h"

MovingPath::PathNode::PathNode(const GridIndex & index, int remainingMovementRange) : m_GridIndex{ index }, m_RemainingMovementRange{ remainingMovementRange }
{
}

bool MovingPath::isEmpty() const
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

std::size_t MovingPath::getLength() const
{
	return m_Path.size();
}

bool MovingPath::hasIndex(const GridIndex & index) const
{
	for (const auto & node : m_Path)
		if (node.m_GridIndex == index)
			return true;

	return false;
}

bool MovingPath::hasPreviousOf(const GridIndex & index) const
{
	if (m_Path.size() <= 1 || !hasIndex(index))
		return false;

	return getFrontNode().m_GridIndex != index;
}

bool MovingPath::hasNextOf(const GridIndex & index) const
{
	if (m_Path.size() <= 1 || !hasIndex(index))
		return false;

	return getBackNode().m_GridIndex != index;
}

MovingPath::PathNode MovingPath::getFrontNode() const
{
	assert(isEmpty() && "MovingPath::getFrontNode() the path is empty.");
	return m_Path.front();
}

MovingPath::PathNode MovingPath::getBackNode() const
{
	assert(isEmpty() && "MovingPath::getBackNode() the path is empty.");
	return m_Path.back();
}

MovingPath::PathNode MovingPath::getPreviousNodeOf(const GridIndex & index) const
{
	auto findIter = std::find_if(m_Path.begin(), m_Path.end(), [index](const MovingPath::PathNode & node) {
		return node.m_GridIndex == index;
	});

	assert(findIter != m_Path.end() && findIter != m_Path.begin() && "MovingPath::getPreviousNodeOf() the previous node doesn't exist.");

	return *(--findIter);
}

MovingPath::PathNode MovingPath::getNextNodeOf(const GridIndex & index) const
{
	auto findIter = std::find_if(m_Path.begin(), m_Path.end(), [index](const MovingPath::PathNode & node) {
		return node.m_GridIndex == index;
	});

	assert(findIter != m_Path.end() && "MovingPath::getNextNodeOf() the next node doesn't exist.");
	assert((++findIter) != m_Path.end() && "MovingPath::getNextNodeOf() the next node doesn't exist.");

	return *findIter;
}

bool MovingPath::tryFindAndCut(const GridIndex & index)
{
	auto existingIter = std::find_if(m_Path.begin(), m_Path.end(), [index](const MovingPath::PathNode & node) {return node.m_GridIndex == index; });
	if (existingIter == m_Path.end())
		return false;

	auto sizeBeforeErase = m_Path.size();
	m_Path.erase(++existingIter, m_Path.end());

	return sizeBeforeErase > m_Path.size();
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

const std::vector<MovingPath::PathNode> & MovingPath::getUnderlyingPath() const
{
	return m_Path;
}
