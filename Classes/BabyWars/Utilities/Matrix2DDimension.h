#ifndef __MATRIX_2D_DIMENSION__
#define __MATRIX_2D_DIMENSION__

#include <cstddef>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}

struct Matrix2DDimension
{
	using SizeType = std::size_t;

	Matrix2DDimension() = default;
	Matrix2DDimension(SizeType rowCount, SizeType colCount) : rowCount{ rowCount }, colCount{ colCount } {}
	Matrix2DDimension(const tinyxml2::XMLElement * xmlElement);
	~Matrix2DDimension() = default;

	bool operator==(const Matrix2DDimension & rhs) { return rowCount == rhs.rowCount && colCount == rhs.colCount; }

	SizeType rowCount{}, colCount{};
};

#endif // !__MATRIX_2D_DIMENSION__
