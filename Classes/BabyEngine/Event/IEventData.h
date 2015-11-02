#ifndef __I_EVENT_DATA__
#define __I_EVENT_DATA__

#include "EventType.h"

/*!
 * \class IEventData
 *
 * \brief The interface of the EventData.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class IEventData
{
public:
	virtual ~IEventData() = default;

	virtual const EventType & vGetType() const = 0;
};

#endif // !__I_EVENT_DATA__
