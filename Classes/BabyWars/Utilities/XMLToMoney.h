#ifndef __XML_TO_MONEY__
#define __XML_TO_MONEY__

#include "Money.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	Money XMLToMoney(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_MONEY__
