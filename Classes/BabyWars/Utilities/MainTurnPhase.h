#ifndef __MAIN_TURN_PHASE__
#define __MAIN_TURN_PHASE__

#include "TurnPhase.h"

class MainTurnPhase : public TurnPhase
{
public:
	MainTurnPhase() = default;
	~MainTurnPhase() = default;

	//Disable copy/move constructor and operator=.
	MainTurnPhase(const MainTurnPhase &) = delete;
	MainTurnPhase(MainTurnPhase &&) = delete;
	MainTurnPhase & operator=(const MainTurnPhase &) = delete;
	MainTurnPhase & operator=(MainTurnPhase &&) = delete;

private:
	virtual TurnPhaseTypeCode vGetTypeCode() const override;

	virtual bool vCanEnterNewPhase(TurnPhaseTypeCode newPhaseTypeCode) const override;
	virtual void vOnEnterPhase(TurnManagerScript & turnManagerScript) const override;

	virtual std::vector<GameCommand> vGenerateGameCommands() const override;
};

#endif // !__MAIN_TURN_PHASE__
