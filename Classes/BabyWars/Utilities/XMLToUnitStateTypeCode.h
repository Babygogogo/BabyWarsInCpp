#ifndef __XML_TO_UNIT_STATE_TYPE_CODE__
#define __XML_TO_UNIT_STATE_TYPE_CODE__

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
enum class UnitStateTypeCode;

namespace utilities
{
	UnitStateTypeCode XMLToUnitStateTypeCode(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_UNIT_STATE_TYPE_CODE__
