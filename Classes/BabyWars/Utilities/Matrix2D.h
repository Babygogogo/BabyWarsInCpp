#ifndef __MATRIX__2D__
#define __MATRIX__2D__

#include <vector>
#include <cassert>

#include "Matrix2DDimension.h"
#include "GridIndex.h"

template <class Containee>
class Matrix2D
{
public:
	using SizeType = Matrix2DDimension::SizeType;

	Matrix2D() = default;
	Matrix2D(SizeType rowCount, SizeType colCount);
	Matrix2D(const Matrix2DDimension & dimension);
	~Matrix2D() = default;

	void setDimension(SizeType rowCount, SizeType colCount);
	void setDimension(const Matrix2DDimension & dimension);
	Matrix2DDimension getDimension() const;

	bool isIndexValid(const GridIndex & gridIndex) const;
	Containee & operator[](const GridIndex & gridIndex);
	const Containee & operator[](const GridIndex & gridIndex) const;

private:
	std::vector < std::vector<Containee>> m_Matrix;
};

template <class Containee>
Matrix2D<Containee>::Matrix2D(SizeType rowCount, SizeType colCount)
{
	setDimension(rowCount, colCount);
}

template <class Containee>
Matrix2D<Containee>::Matrix2D(const Matrix2DDimension & dimension)
{
	setDimension(dimension);
}

template <class Containee>
void Matrix2D<Containee>::setDimension(SizeType rowCount, SizeType colCount)
{
	assert(rowCount >= 0 && colCount >= 0 && "Matrix2D<>::resize() the new size is negative.");

	m_Matrix.resize(rowCount);
	for (auto & row : m_Matrix)
		row.resize(colCount);
}

template <class Containee>
void Matrix2D<Containee>::setDimension(const Matrix2DDimension & dimension)
{
	setDimension(dimension.rowCount, dimension.colCount);
}

template <class Containee>
Matrix2DDimension Matrix2D<Containee>::getDimension() const
{
	auto rowCount = m_Matrix.size();
	if (rowCount == 0)
		return{ rowCount, 0 };

	return{ rowCount, m_Matrix[0].size() };
}

template <class Containee>
bool Matrix2D<Containee>::isIndexValid(const GridIndex & gridIndex) const
{
	if (gridIndex.rowIndex < 0 || gridIndex.colIndex < 0)
		return false;

	auto dimension = getDimension();
	//Casting from signed to/from unsigned is a nightmare. But because the negative cases is dealt above, it seems to be safe to make a direct cast.
	return static_cast<SizeType>(gridIndex.rowIndex) < dimension.rowCount && static_cast<SizeType>(gridIndex.colIndex) < dimension.colCount;
}

template <class Containee>
Containee & Matrix2D<Containee>::operator[](const GridIndex & gridIndex)
{
	assert(isIndexValid(gridIndex) && "Matrix2D<>::operator[]() the gridIndex is invalid.");

	return m_Matrix[gridIndex.rowIndex][gridIndex.colIndex];
}

template <class Containee>
const Containee & Matrix2D<Containee>::operator[](const GridIndex & gridIndex) const
{
	assert(isIndexValid(gridIndex) && "Matrix2D<>::operator[]()const the gridIndex is invalid.");

	return m_Matrix[gridIndex.rowIndex][gridIndex.colIndex];
}

#endif // !__MATRIX__2D__
