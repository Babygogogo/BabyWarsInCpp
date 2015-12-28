#include <cassert>

#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "Matrix2DDimension.h"

Matrix2DDimension::Matrix2DDimension(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "Matrix2DDimension::Matrix2DDimension() the xml element is nullptr.");
	if (xmlElement) {
		rowCount = xmlElement->IntAttribute("RowCount");
		colCount = xmlElement->IntAttribute("ColumnCount");
	}
}
