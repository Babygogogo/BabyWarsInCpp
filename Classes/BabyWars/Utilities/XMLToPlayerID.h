#ifndef __XML_TO_PLAYER_ID__
#define __XML_TO_PLAYER_ID__

#include "PlayerID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	PlayerID XMLToPlayerID(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_PLAYER_ID__
