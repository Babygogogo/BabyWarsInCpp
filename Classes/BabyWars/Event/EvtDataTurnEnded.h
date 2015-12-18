#ifndef __EVENT_DATA_TURN_ENDED__
#define __EVENT_DATA_TURN_ENDED__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/PlayerID.h"

class EvtDataTurnEnded : public BaseEventData
{
public:
	EvtDataTurnEnded(int turnIndex, PlayerID playerID) : m_TurnIndex{ turnIndex }, m_PlayerID{ playerID } {}
	virtual ~EvtDataTurnEnded() = default;

	int getTurnIndex() const;
	PlayerID getPlayerID() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataTurnEnded(const EvtDataTurnEnded &) = delete;
	EvtDataTurnEnded(EvtDataTurnEnded &&) = delete;
	EvtDataTurnEnded & operator=(const EvtDataTurnEnded &) = delete;
	EvtDataTurnEnded & operator=(EvtDataTurnEnded &&) = delete;

private:
	int m_TurnIndex{};
	PlayerID m_PlayerID{ INVALID_PLAYER_ID };
};

#endif // !__EVENT_DATA_TURN_ENDED__
