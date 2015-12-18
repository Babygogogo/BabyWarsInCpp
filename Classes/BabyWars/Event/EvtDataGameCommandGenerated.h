#ifndef __EVENT_DATA_ACTIVATE_UNIT_END__
#define __EVENT_DATA_ACTIVATE_UNIT_END__

#include <memory>
#include <vector>

#include "../../BabyEngine/Event/BaseEventData.h"

class GameCommand;

class EvtDataGameCommandGenerated : public BaseEventData
{
public:
	EvtDataGameCommandGenerated(std::vector<GameCommand> && gameCommands) : m_GameCommands{ std::move(gameCommands) } {}
	virtual ~EvtDataGameCommandGenerated() = default;

	const std::vector<GameCommand> & getGameCommands() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataGameCommandGenerated(const EvtDataGameCommandGenerated &) = delete;
	EvtDataGameCommandGenerated(EvtDataGameCommandGenerated &&) = delete;
	EvtDataGameCommandGenerated & operator=(const EvtDataGameCommandGenerated &) = delete;
	EvtDataGameCommandGenerated & operator=(EvtDataGameCommandGenerated &&) = delete;

private:
	const std::vector<GameCommand> m_GameCommands;
};

#endif // !__EVENT_DATA_ACTIVATE_UNIT_END__
