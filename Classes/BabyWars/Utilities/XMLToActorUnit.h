#ifndef __XML_TO_ACTOR_UNIT__
#define __XML_TO_ACTOR_UNIT__

#include <memory>

//Forward declaration.
class Actor;
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	std::shared_ptr<Actor> XMLToActorUnit(const tinyxml2::XMLElement * unitElement);
}

#endif // !__XML_TO_ACTOR_UNIT__
