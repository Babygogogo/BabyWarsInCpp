#ifndef __BASE_EVENT_DATA__
#define __BASE_EVENT_DATA__

#include "IEventData.h"

/*!
 * \class BaseEventData
 *
 * \brief The base class of event data.
 *
 * \details
 *	Maybe some more functionalities later...
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseEventData : public IEventData
{
public:
	virtual ~BaseEventData() = default;

	virtual const EventType & getType() const override = 0;
};

#endif // !__BASE_EVENT_DATA__
