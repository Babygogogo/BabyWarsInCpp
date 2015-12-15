#ifndef __EVENT_DATA_BEGIN_TURN__
#define __EVENT_DATA_BEGIN_TURN__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"
#include "../Utilities/PlayerID.h"

class EvtDataBeginTurn : public BaseEventData
{
public:
	EvtDataBeginTurn(int turnIndex, PlayerID playerID) : m_TurnIndex{ turnIndex }, m_PlayerID{ playerID } {}
	virtual ~EvtDataBeginTurn() = default;

	int getTurnIndex() const;
	PlayerID getPlayerID() const;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataBeginTurn(const EvtDataBeginTurn &) = delete;
	EvtDataBeginTurn(EvtDataBeginTurn &&) = delete;
	EvtDataBeginTurn & operator=(const EvtDataBeginTurn &) = delete;
	EvtDataBeginTurn & operator=(EvtDataBeginTurn &&) = delete;

private:
	int m_TurnIndex{};
	PlayerID m_PlayerID{ INVALID_PLAYER_ID };
};

#endif // !__EVENT_DATA_BEGIN_TURN__
