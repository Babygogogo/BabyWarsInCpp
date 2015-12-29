#ifndef __INVALID_TURN_PHASE__
#define __INVALID_TURN_PHASE__

#include "TurnPhase.h"

class InvalidTurnPhase : public TurnPhase
{
public:
	InvalidTurnPhase() = default;
	~InvalidTurnPhase() = default;

	//Disable copy/move constructor and operator=.
	InvalidTurnPhase(const InvalidTurnPhase &) = delete;
	InvalidTurnPhase(InvalidTurnPhase &&) = delete;
	InvalidTurnPhase & operator=(const InvalidTurnPhase &) = delete;
	InvalidTurnPhase & operator=(InvalidTurnPhase &&) = delete;

private:
	virtual TurnPhaseTypeCode vGetTypeCode() const override;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const override;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const override;

	virtual std::vector<GameCommand> vGenerateGameCommands() const override;

	virtual void vUpdateUnit(UnitScript & unitScript) const override;
};

#endif // !__INVALID_TURN_PHASE__
