#include <unordered_set>
#include <unordered_map>
#include <list>
#include <array>
#include <cassert>

#include "EventDispatcher.h"
#include "IEventListener.h"
#include "IEventData.h"
#include "BaseEventData.h"
#include "EventType.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//definition of EventDispatcherImpl
//////////////////////////////////////////////////////////////////////////
struct EventDispatcher::EventDispatcherImpl
{
	EventDispatcherImpl(){};
	~EventDispatcherImpl(){};

	void handleNewListeners();

	//Determine if we can add a listener and callback pair into the listener list.
	bool canAddListenerCallback(const EventType & eType, const ListenerCallback & eListenerCallback) const;

	//Check if there is a same listener to an event type already.
	bool hasSameListener(const EventType & eType, const std::shared_ptr<void> & eListener) const;

	//Get the next index of the event queue for the given index.
	int getNextQueueIndex(const int & index) const;

	//Dispatch an event immediately. Remove the dead listeners by the way.
	void dispatchEvent(const IEventData & eData);

	static const int EVENT_QUEUE_COUNT = 2;

	int m_NextQueueIndex{ 0 };
	bool m_IsDispatchingQueue{ false };

	std::unordered_map<EventType, std::list<ListenerCallback>> m_ListenerCallbackLists;
	std::array<std::list<std::shared_ptr<IEventData>>, EVENT_QUEUE_COUNT> m_EventQueues;
	std::list<std::function<void()>> m_CachedOperations;

	std::unordered_map<EventType, std::unordered_multimap<void*, std::function<void(BaseEventData*)>>> m_listeners;
	std::unordered_map<EventType, std::unordered_multimap<void*, std::function<void(BaseEventData*)>>> m_listeners_to_add;
	std::unordered_set<void*> m_listeners_to_delete;

	std::unordered_map<EventType, std::unordered_set<IEventListener*>> m_script_listeners;
};

void EventDispatcher::EventDispatcherImpl::handleNewListeners()
{
	for (auto &listener_to_delete : m_listeners_to_delete)
		for (auto &type_target_callback : m_listeners)
			type_target_callback.second.erase(listener_to_delete);

	for (auto &type_map : m_listeners_to_add)
		for (auto &target_callback : type_map.second)
			m_listeners[type_map.first].emplace(std::move(target_callback));

	m_listeners_to_add.clear();
	m_listeners_to_delete.clear();
}

bool EventDispatcher::EventDispatcherImpl::canAddListenerCallback(const EventType & eType, const ListenerCallback & eListenerCallback) const
{
	//If the new listener is already dead, we can't add it to the listener list.
	if (eListenerCallback.first.expired())
		return false;

	//If there is a same listener to the same event type, we can't add it to the list either.
	if (hasSameListener(eType, eListenerCallback.first.lock())){
		cocos2d::log("EventDispatcher::vAddListener trying to double-register a listener.");
		return false;
	}

	return true;
}

bool EventDispatcher::EventDispatcherImpl::hasSameListener(const EventType & eType, const std::shared_ptr<void> & eListener) const
{
	//If there is no event type the same as eType in the list, there is of course no the same listener in it.
	auto listenerCallbackListIter = m_ListenerCallbackLists.find(eType);
	if (listenerCallbackListIter == m_ListenerCallbackLists.end())
		return false;

	//Traverse the listener list to see if there is a same listener.
	for (const auto &listenerInList : listenerCallbackListIter->second){
		//If the current listener is dead, simply ignore it.
		if (listenerInList.first.expired())
			continue;

		//If the current listener is the same as eListener, return true.
		if (listenerInList.first.lock() == eListener)
			return true;
	}

	//There is no the same listener in the list. Return false.
	return false;
}

int EventDispatcher::EventDispatcherImpl::getNextQueueIndex(const int & index) const
{
	return (index + 1) % EVENT_QUEUE_COUNT;
}

void EventDispatcher::EventDispatcherImpl::dispatchEvent(const IEventData & eData)
{
	//If there is no listener listening to the event type, simply return.
	auto listenerCallbackListIter = m_ListenerCallbackLists.find(eData.getType());
	if (listenerCallbackListIter == m_ListenerCallbackLists.end())
		return;

	//Traverse all the listeners.
	//Don't use for(xx:yy) because if a dead listener is encountered, it should be remove from the list.
	auto listenerCallbackIter = listenerCallbackListIter->second.begin();
	while (listenerCallbackIter != listenerCallbackListIter->second.end()){
		//Save the current iterator and increment the listenerCallbackIter so that the listenerCallbackIter won't be invalidate if the removal occurs.
		auto currentIter = listenerCallbackIter++;

		//If the current listener is dead, remove it from the list.
		if (currentIter->first.expired())
			listenerCallbackListIter->second.erase(currentIter);
		else //If not dead, call its associated callback.
			currentIter->second(eData);
	}
}

//////////////////////////////////////////////////////////////////////////
//implementation of EventDispatcher
//////////////////////////////////////////////////////////////////////////
EventDispatcher::EventDispatcher() : pimpl(new EventDispatcherImpl)
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::vAddListener(const EventType & eType, const ListenerCallback & eListenerCallback)
{
	if (pimpl->canAddListenerCallback(eType, eListenerCallback))
		pimpl->m_ListenerCallbackLists[eType].emplace_back(eListenerCallback);
}

void EventDispatcher::vAddListener(const EventType & eType, ListenerCallback && eListenerCallback)
{
	if (pimpl->canAddListenerCallback(eType, eListenerCallback))
		pimpl->m_ListenerCallbackLists[eType].emplace_back(std::move(eListenerCallback));
}

void EventDispatcher::vAddListener(const EventType & eType, std::weak_ptr<void> && listener, std::function<void(const IEventData &)> && callback)
{
	auto listenerCallback = std::make_pair(std::move(listener), std::move(callback));
	if (pimpl->canAddListenerCallback(eType, listenerCallback))
		pimpl->m_ListenerCallbackLists[eType].emplace_back(std::move(listenerCallback));
}

void EventDispatcher::vRemoveListener(const EventType & eType, const std::weak_ptr<void> & eListener)
{
	//If the eListener is dead or there is no eType in the listener list, simply return.
	if (eListener.expired() || pimpl->m_ListenerCallbackLists.find(eType) == pimpl->m_ListenerCallbackLists.end())
		return;

	//If it's dispatching, we should cache this operation and call it after the dispatch.
	if (pimpl->m_IsDispatchingQueue){
		pimpl->m_CachedOperations.emplace_back([this, eType, eListener](){vRemoveListener(eType, eListener); });
		return;
	}

	//Prepare for traversing the listener list.
	const auto strongListener = eListener.lock();
	auto & listenerListOfType = pimpl->m_ListenerCallbackLists[eType];

	//Traverse the listener list to see if there is an listener to be removed.
	//Don't use for(xx:yy) because there is at most one listener to remove from the list. Once we find it, we can simply remove it and return.
	for (auto listenerIter = listenerListOfType.begin(); listenerIter != listenerListOfType.end(); ++listenerIter){
		//If the current listener is dead, ignore it.
		if (listenerIter->first.expired())
			continue;

		//If the current listener "equals" eListener, remove it from the list.
		if (listenerIter->first.lock() == strongListener){
			listenerListOfType.erase(listenerIter);
			return;
		}
	}
}

//void EventDispatcher::vQueueEvent(const std::shared_ptr<IEventData> & eData)
//{
//	pimpl->m_EventQueues[pimpl->m_NextQueueIndex].emplace_back(eData);
//}
//
//void EventDispatcher::vQueueEvent(std::shared_ptr<IEventData> && eData)
//{
//	pimpl->m_EventQueues[pimpl->m_NextQueueIndex].emplace_back(std::move(eData));
//}

void EventDispatcher::vQueueEvent(std::unique_ptr<IEventData> && eData)
{
	pimpl->m_EventQueues[pimpl->m_NextQueueIndex].emplace_back(std::move(eData));
}

void EventDispatcher::vAbortEvent(const EventType & eType, bool allOfThisType /*= false*/)
{
	//If it's dispatching, cache this operation and call it after the dispatch.
	if (pimpl->m_IsDispatchingQueue){
		pimpl->m_CachedOperations.emplace_back([this, eType, allOfThisType](){vAbortEvent(eType, allOfThisType); });
		return;
	}

	//We should traverse all of the event queues and we should begin with the next one.
	auto queueIndex = pimpl->m_NextQueueIndex;
	for (auto traverseCount = 0; traverseCount < EventDispatcherImpl::EVENT_QUEUE_COUNT; ++traverseCount){
		auto & eventQueue = pimpl->m_EventQueues[queueIndex];
		auto eventIter = eventQueue.begin();

		//Traverse through the queue.
		while (eventIter != eventQueue.end()){
			//Save the current iterator and ++eventIter so that the eventIter won't be invalidate if the removal occurs.
			auto currentIter = eventIter;
			++eventIter;

			//If we find the event to delete, remove it from the queue.
			if ((*currentIter)->getType() == eType){
				eventQueue.erase(currentIter);

				//If allOfThisType is false, we simply return because there's no need to remove the other events.
				if (!allOfThisType)
					return;
			}
		}

		//Update the queue index so that we can deal with the next queue in the next loop.
		queueIndex = pimpl->getNextQueueIndex(queueIndex);
	}
}

void EventDispatcher::vTrigger(const std::shared_ptr<IEventData> & eData)
{
	pimpl->dispatchEvent(*eData);
}

void EventDispatcher::vDispatchQueuedEvents(const std::chrono::milliseconds & timeOutMs /*= 10ms*/)
{
	//Make sure that vDispatchQueueEvents isn't called recursively.
	assert(!pimpl->m_IsDispatchingQueue);

	//Set the flag for dispatching.
	pimpl->m_IsDispatchingQueue = true;

	//Save the current time point. It will be used for calculating the time period of dispatch.
	const auto beginningTimePoint = std::chrono::steady_clock::now();

	//Get the queue for dispatching.
	auto & eventQueue = pimpl->m_EventQueues[pimpl->m_NextQueueIndex];
	while (!eventQueue.empty()){
		//For each event in the queue, dispatch it and then remove it from the queue.
		pimpl->dispatchEvent(*eventQueue.front());
		eventQueue.pop_front();

		//Check if time runs out.
		auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginningTimePoint);
		if (elapsedTimeMs >= timeOutMs){
			//If there is any event left in the queue, we must move it into the next queue.
			if (!eventQueue.empty()){
				//Make a log.
				cocos2d::log("EventDispatcher::vDispatchQueuedEvent time out");

				//Move the events left in the queue into the beginning of the next queue.
				auto & nextQueue = pimpl->m_EventQueues[pimpl->getNextQueueIndex(pimpl->m_NextQueueIndex)];
				nextQueue.splice(nextQueue.begin(), std::move(eventQueue));
			}

			//End the dispatch.
			break;
		}
	}

	//Unset the flag for dispatching.
	pimpl->m_IsDispatchingQueue = false;

	//Update the next queue index.
	pimpl->m_NextQueueIndex = pimpl->getNextQueueIndex(pimpl->m_NextQueueIndex);

	//Call the cached operations.
	while (!pimpl->m_CachedOperations.empty()){
		pimpl->m_CachedOperations.front()();
		pimpl->m_CachedOperations.pop_front();
	}
}
