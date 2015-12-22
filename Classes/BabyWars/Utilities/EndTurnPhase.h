#ifndef __END_TURN_PHASE__
#define __END_TURN_PHASE__

#include "TurnPhase.h"

class EndTurnPhase : public TurnPhase
{
public:
	EndTurnPhase() = default;
	~EndTurnPhase() = default;

	//Disable copy/move constructor and operator=.
	EndTurnPhase(const EndTurnPhase &) = delete;
	EndTurnPhase(EndTurnPhase &&) = delete;
	EndTurnPhase & operator=(const EndTurnPhase &) = delete;
	EndTurnPhase & operator=(EndTurnPhase &&) = delete;

private:
	virtual TurnPhaseTypeCode vGetTypeCode() const override;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const override;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const override;

	virtual std::vector<GameCommand> vGenerateGameCommands() const override;
};

#endif // !__END_TURN_PHASE__
