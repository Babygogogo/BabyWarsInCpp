#ifndef __I_EVENT_DISPATCHER__
#define __I_EVENT_DISPATCHER__

#include <memory>
#include <chrono>
#include <functional>

#include "EventType.h"

class IEventData;
class BaseEventData;	//should be removed soon

/*!
 * \class IEventDispatcher
 *
 * \brief The interface of EventDispatcher. Used for dispatching events to listeners.
 *
 * \details
 *	There are two ways to dispatch an event: vQueueEvent() and vTrigger().
 *	vQueueEvent() is the preferred way because it avoids the problem of circular dispatch.
 *
 *	You must call vAddListener() before a listener can listen to any events.
 *	If a listener dies, it will be removed from the listener list automatically.
 *
 * \author Babygogogo
 * \date 2015.7
 */

class IEventDispatcher
{
public:
	virtual ~IEventDispatcher() = default;

	//Type shortcut of the pair of listener and its callback.
	using ListenerCallback = std::pair < std::weak_ptr<void>, std::function<void(const IEventData &)> >;

	//Add an event listener to an given event type.
	//If it's added twice, the second one will be ignored.
	virtual void vAddListener(const EventType & eType, const ListenerCallback & eListenerCallback) = 0;
	virtual void vAddListener(const EventType & eType, ListenerCallback && eListenerCallback) = 0;
	virtual void vAddListener(const EventType & eType, std::weak_ptr<void> && listener, std::function<void(const IEventData &)> && callback) = 0;

	//Remove an event listener to an given event type.
	//Call this only if you want to remove a listener before it dies.
	virtual void vRemoveListener(const EventType & eType, const std::weak_ptr<void> & eListener) = 0;

	//Queue an event. The event will be dispatched in vDispatchQueuedEvents().
	//This is the preferred way to dispatch an event.
	//virtual void vQueueEvent(const std::shared_ptr<IEventData> & eData) = 0;
	//virtual void vQueueEvent(std::shared_ptr<IEventData> && eData) = 0;
	virtual void vQueueEvent(std::unique_ptr<IEventData> && eData) = 0;

	//Abort the first event of the given type in the queue.
	//If allOfThisType is true, all of the events of the given type will be aborted.
	virtual void vAbortEvent(const EventType & eType, bool allOfThisType = false) = 0;

	//Dispatch an event immediately. Should be called only if you have a good reason to.
	virtual void vTrigger(const std::shared_ptr<IEventData> & eData) = 0;

	//Dispatch all events in the queue. Should be called at the beginning of each game loop.
	//If the time for dispatching events exceeds timeOutMs, the events left will be dispatched on the next call.
	virtual void vDispatchQueuedEvents(const std::chrono::milliseconds & timeOutMs = std::chrono::milliseconds{ 10 }) = 0;
};

#endif // !__I_EVENT_DISPATCHER__
