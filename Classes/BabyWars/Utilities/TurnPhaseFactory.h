#ifndef __TURN_PHASE_FACTORY__
#define __TURN_PHASE_FACTORY__

#include <memory>

//Forward declaration.
class TurnPhase;
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	std::shared_ptr<TurnPhase> createTurnPhaseWithTypeCode(TurnPhaseTypeCode typeCode);
	std::shared_ptr<TurnPhase> createTurnPhaseWithXML(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__TURN_PHASE_FACTORY__
