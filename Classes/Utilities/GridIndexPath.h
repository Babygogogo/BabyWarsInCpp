#ifndef __GRID_INDEX_PATH__
#define __GRID_INDEX_PATH__

#include <vector>
#include <algorithm>

#include "GridIndex.h"

class GridIndexPath
{
public:
	GridIndexPath() = default;
	~GridIndexPath() = default;

	void clear()
	{
		m_Path.clear();
	}

	void pushBack(const GridIndex & index)
	{
		m_Path.emplace_back(index);
	}

	//Find the index in the path. If found, erase all indexes behind the found index (exclusive). Otherwise, do nothing.
	//The return value indicates whether the index is found or not.
	bool findAndCut(const GridIndex & index)
	{
		auto existingIter = std::find(m_Path.begin(), m_Path.end(), index);
		if (existingIter == m_Path.end())
			return false;

		m_Path.erase(++existingIter, m_Path.end());
		return true;
	}

	const std::vector<GridIndex> & getPath() const
	{
		return m_Path;
	}

private:
	std::vector<GridIndex> m_Path;
};

#endif // !__GRID_INDEX_PATH__
