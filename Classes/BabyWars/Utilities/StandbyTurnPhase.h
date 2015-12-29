#ifndef __STANDBY_TURN_PHASE__
#define __STANDBY_TURN_PHASE__

#include "TurnPhase.h"

class StandbyTurnPhase : public TurnPhase
{
public:
	StandbyTurnPhase() = default;
	~StandbyTurnPhase() = default;

	//Disable copy/move constructor and operator=.
	StandbyTurnPhase(const StandbyTurnPhase &) = delete;
	StandbyTurnPhase(StandbyTurnPhase &&) = delete;
	StandbyTurnPhase & operator=(const StandbyTurnPhase &) = delete;
	StandbyTurnPhase & operator=(StandbyTurnPhase &&) = delete;

private:
	virtual TurnPhaseTypeCode vGetTypeCode() const override;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const override;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const override;

	virtual std::vector<GameCommand> vGenerateGameCommands() const override;

	virtual void vUpdateUnit(UnitScript & unitScript) const override;
};

#endif // !__STANDBY_TURN_PHASE__
