#ifndef __BEGINNING_TURN_PHASE__
#define __BEGINNING_TURN_PHASE__

#include "TurnPhase.h"

class BeginningTurnPhase : public TurnPhase
{
public:
	BeginningTurnPhase() = default;
	~BeginningTurnPhase() = default;

	//Disable copy/move constructor and operator=.
	BeginningTurnPhase(const BeginningTurnPhase &) = delete;
	BeginningTurnPhase(BeginningTurnPhase &&) = delete;
	BeginningTurnPhase & operator=(const BeginningTurnPhase &) = delete;
	BeginningTurnPhase & operator=(BeginningTurnPhase &&) = delete;

private:
	virtual TurnPhaseTypeCode vGetTypeCode() const override;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const override;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const override;

	virtual std::vector<GameCommand> vGenerateGameCommands() const override;

	virtual void vUpdateUnit(UnitScript & unitScript) const override;
};

#endif // !__BEGINNING_TURN_PHASE__
