#ifndef __TURN_PHASE__
#define __TURN_PHASE__

#include <vector>

//Forward declaration.
class TurnManagerScript;
class GameCommand;
enum class TurnPhaseTypeCode;

class TurnPhase
{
public:
	virtual ~TurnPhase() = default;

	virtual TurnPhaseTypeCode vGetTypeCode() const = 0;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const = 0;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const = 0;

	virtual std::vector<GameCommand> vGenerateGameCommands() const = 0;
};

#endif // !__TURN_PHASE__
