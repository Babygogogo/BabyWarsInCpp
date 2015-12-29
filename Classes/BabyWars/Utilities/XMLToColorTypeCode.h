#ifndef __XML_TO_COLOR_TYPE_CODE__
#define __XML_TO_COLOR_TYPE_CODE__

#include "ColorTypeCode.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	ColorTypeCode XMLToColorTypeCode(const tinyxml2::XMLElement * xmlElement);
	ColorTypeCode StringToColorTypeCode(const std::string & codeString);
}

#endif // !__XML_TO_COLOR_TYPE_CODE__
