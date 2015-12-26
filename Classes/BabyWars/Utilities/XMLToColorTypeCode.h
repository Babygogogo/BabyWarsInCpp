#ifndef __XML_TO_COLOR_TYPE_CODE__
#define __XML_TO_COLOR_TYPE_CODE__

#include <string>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
enum class ColorTypeCode;

namespace utilities
{
	ColorTypeCode XMLToColorTypeCode(const tinyxml2::XMLElement * xmlElement);
	ColorTypeCode StringToColorTypeCode(const std::string & codeString);
}

#endif // !__XML_TO_COLOR_TYPE_CODE__
