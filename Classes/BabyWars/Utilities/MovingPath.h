#ifndef __MOVING_PATH__
#define __MOVING_PATH__

#include <vector>
#include <list>

#include "../../BabyEngine/Utilities/GridIndex.h"

class MovingPath
{
public:
	struct PathNode
	{
		PathNode(const GridIndex & index, int remainingMovementRange);

		GridIndex m_GridIndex;
		int m_RemainingMovementRange{ -1 };
	};

	MovingPath() = default;
	~MovingPath() = default;

	MovingPath(const MovingPath &) = default;
	MovingPath(MovingPath &&) = default;

	bool isEmpty() const;
	void init(PathNode && pathNode);
	void init(std::list<PathNode> && path);

	void clear();

	std::size_t getLength() const;
	bool hasIndex(const GridIndex & index) const;
	bool hasPreviousOf(const GridIndex & index) const;
	bool hasNextOf(const GridIndex & index) const;

	//Asserts if the node doesn't exist.
	PathNode getFrontNode() const;
	PathNode getBackNode() const;
	PathNode getPreviousNodeOf(const GridIndex & index) const;
	PathNode getNextNodeOf(const GridIndex & index) const;

	//Find the index in the path. If found, erase all indexes behind the found index (exclusive). Otherwise, do nothing.
	//The return value indicates whether the path is cut or not.
	bool tryFindAndCut(const GridIndex & index);

	//Extend the path to destination. The extension succeeds if the followings are true:
	// - The destination is not in the path already
	// - The destination is adjacent to the back node of the path
	// - The remaining movement range is non-negative if the path is extended.
	//The return value indicates whether the extension is successful.
	bool tryExtend(const GridIndex & destination, int movingCost);

	const std::vector<PathNode> & getUnderlyingPath() const;

private:
	std::vector<PathNode> m_Path;
};

#endif // !__MOVING_PATH__
