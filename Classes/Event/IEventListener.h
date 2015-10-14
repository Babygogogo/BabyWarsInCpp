#ifndef __I_EVENT_LISTENER__
#define __I_EVENT_LISTENER__

#include <memory>

class BaseEventData;
class IEventData;

/*!
 * \class IEventListener
 *
 * \brief The interface of EventListener. Can listen to events after registering to IEventDispatcher.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class IEventListener
{
public:
	~IEventListener(){};

	//Callback function for events.
	virtual void onEvent(BaseEventData* e) = 0;
	virtual void onEvent(const std::shared_ptr<IEventData> & e) = 0;
};

#endif // !__I_EVENT_LISTENER__
