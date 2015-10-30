#ifndef __EVENT_DATA_GENERIC__
#define __EVENT_DATA_GENERIC__

#include "BaseEventData.h"

/*!
 * \class EvtDataGeneric
 *
 * \brief The generic event data class that can be used for any event that has no specific data.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class EvtDataGeneric : public BaseEventData
{
public:
	EvtDataGeneric(EventType eType) : m_Type{ eType }
	{
	}

	virtual ~EvtDataGeneric() = default;

	virtual const EventType & getType() const override
	{
		return m_Type;
	}

	//Disable copy/move constructor and operator=.
	EvtDataGeneric(const EvtDataGeneric &) = delete;
	EvtDataGeneric(EvtDataGeneric &&) = delete;
	EvtDataGeneric & operator=(const EvtDataGeneric &) = delete;
	EvtDataGeneric & operator=(EvtDataGeneric &&) = delete;

private:
	EventType m_Type{ EventType::InvalidEventType };
};

#endif // !__EVENT_DATA_GENERIC__
