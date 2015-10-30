#ifndef __EVENT_DISPATCHER__
#define __EVENT_DISPATCHER__

#include <memory>
#include <functional>

#include "IEventDispatcher.h"

class IEventListener;
class BaseEventData;
enum class EventType;

class EventDispatcher final : public IEventDispatcher
{
public:
	EventDispatcher();
	~EventDispatcher();

	//Disable copy/move constructor and operator=.
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;
	EventDispatcher& operator=(EventDispatcher&&) = delete;

private:
	//override functions of the interface
	virtual void vAddListener(const EventType & eType, const ListenerCallback & eListenerCallback) override;
	virtual void vAddListener(const EventType & eType, ListenerCallback && eListenerCallback) override;
	virtual void vAddListener(const EventType & eType, std::weak_ptr<void> && listener, std::function<void(const IEventData &)> && callback) override;

	virtual void vRemoveListener(const EventType & eType, const std::weak_ptr<void> & eListener) override;

	//virtual void vQueueEvent(const std::shared_ptr<IEventData> & eData) override;
	//virtual void vQueueEvent(std::shared_ptr<IEventData> && eData) override;
	virtual void vQueueEvent(std::unique_ptr<IEventData> && eData) override;

	virtual void vAbortEvent(const EventType & eType, bool allOfThisType = false) override;
	virtual void vTrigger(const std::shared_ptr<IEventData> & eData) override;
	virtual void vDispatchQueuedEvents(const std::chrono::milliseconds & timeOutMs = std::chrono::milliseconds{ 10 }) override;

	//implementation stuff
	struct EventDispatcherImpl;
	std::unique_ptr<EventDispatcherImpl> pimpl;
};

#endif // !__EVENT_DISPATCHER__
