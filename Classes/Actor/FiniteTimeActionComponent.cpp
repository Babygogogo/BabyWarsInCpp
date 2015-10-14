#include <cassert>
#include <list>

#include "cocos2d.h"

#include "FiniteTimeActionComponent.h"
#include "Actor.h"
#include "BaseRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SequentialInvokerImpl.
//////////////////////////////////////////////////////////////////////////
struct FiniteTimeActionComponent::FiniteTimeActionComponentImpl
{
	FiniteTimeActionComponentImpl();
	~FiniteTimeActionComponentImpl();

	void pushBackAction(cocos2d::Action *action);
	void popFrontAction();
	void eraseCurrentAction();

	bool runNextAction(bool isCalledByLastAction);
	bool isRunning() const;

	std::weak_ptr<BaseRenderComponent> m_Target;

	cocos2d::Action *m_CurrentAction{ nullptr };
	bool m_IsRunAutomatically{ false };
	std::list<cocos2d::Action*> m_ActionList;
};

FiniteTimeActionComponent::FiniteTimeActionComponentImpl::FiniteTimeActionComponentImpl()
{
}

FiniteTimeActionComponent::FiniteTimeActionComponentImpl::~FiniteTimeActionComponentImpl()
{
	while (!m_ActionList.empty())
		popFrontAction();

	eraseCurrentAction();
}

void FiniteTimeActionComponent::FiniteTimeActionComponentImpl::popFrontAction()
{
	if (m_ActionList.empty())
		return;

	m_ActionList.front()->release();
	m_ActionList.pop_front();
}

void FiniteTimeActionComponent::FiniteTimeActionComponentImpl::pushBackAction(cocos2d::Action *action)
{
	action->retain();
	m_ActionList.emplace_back(action);
}

void FiniteTimeActionComponent::FiniteTimeActionComponentImpl::eraseCurrentAction()
{
	if (!m_CurrentAction)
		return;

	m_CurrentAction->release();
	m_CurrentAction = nullptr;
}

bool FiniteTimeActionComponent::FiniteTimeActionComponentImpl::runNextAction(bool isCalledByLastAction)
{
	if (isCalledByLastAction && !m_IsRunAutomatically)
		return false;
	if (isRunning() || m_ActionList.empty())
		return false;

	m_CurrentAction = m_ActionList.front();
	m_ActionList.pop_front();

	if (!m_Target.expired())
		m_Target.lock()->getSceneNode()->runAction(m_CurrentAction);

	return true;
}

bool FiniteTimeActionComponent::FiniteTimeActionComponentImpl::isRunning() const
{
	if (!m_CurrentAction)
		return false;

	return !m_CurrentAction->isDone();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of SequentialInvoker.
//////////////////////////////////////////////////////////////////////////
FiniteTimeActionComponent::FiniteTimeActionComponent() : pimpl{ std::make_shared<FiniteTimeActionComponentImpl>() }
{
}

FiniteTimeActionComponent::~FiniteTimeActionComponent()
{
}

void FiniteTimeActionComponent::queueAction(cocos2d::FiniteTimeAction* action, std::function<void()> && callback /*= nullptr*/)
{
	assert(action && "FiniteTimeActionComponent::queueAction() trying to queue an empty action.");

	//Create a callback that recalls runNextAction(). Important for automatically running queued actions.
	auto weakPimpl = std::weak_ptr<FiniteTimeActionComponentImpl>(pimpl);
	auto recallSelf = cocos2d::CallFunc::create([weakPimpl](){
		if (!weakPimpl.expired()){
			auto strongPimpl = weakPimpl.lock();
			strongPimpl->eraseCurrentAction();
			strongPimpl->runNextAction(true);
		}
	});

	//Queue an action sequence that wraps the parameters and recallSelf.
	if (callback)
		pimpl->pushBackAction(cocos2d::Sequence::create(action, cocos2d::CallFunc::create(callback), recallSelf, nullptr));
	else
		pimpl->pushBackAction(cocos2d::Sequence::create(action, recallSelf, nullptr));

	if (pimpl->m_IsRunAutomatically)
		runNextAction();
}

void FiniteTimeActionComponent::queueBlink(float duration_s, int blinkCount, Callback && callback /*= nullptr*/)
{
	queueAction(cocos2d::Blink::create(duration_s, blinkCount), std::move(callback));
}

void FiniteTimeActionComponent::queueCallback(std::function<void()> && callback)
{
	queueAction(cocos2d::CallFunc::create(callback));
}

void FiniteTimeActionComponent::queueDelay(float delay_s, std::function<void()> && callback /*= nullptr*/)
{
	assert(delay_s > 0.0f && "SequentialInvoker::addDelay() with time <= 0.");

	queueAction(cocos2d::DelayTime::create(delay_s), std::move(callback));
}

void FiniteTimeActionComponent::queueMoveTo(float duration_s, float x, float y, std::function<void()> && callback /*= nullptr*/)
{
	queueAction(cocos2d::MoveTo::create(duration_s, { x, y }), std::move(callback));
}

void FiniteTimeActionComponent::stopAndClearAllActions()
{
	while (!pimpl->m_ActionList.empty())
		pimpl->popFrontAction();

	if (!pimpl->m_Target.expired())
		pimpl->m_Target.lock()->getSceneNode()->stopAllActions();

	pimpl->eraseCurrentAction();
}

bool FiniteTimeActionComponent::isRunning() const
{
	return pimpl->isRunning();
}

bool FiniteTimeActionComponent::runNextAction()
{
	return pimpl->runNextAction(false);
}

void FiniteTimeActionComponent::setRunAutomatically(bool automatically)
{
	pimpl->m_IsRunAutomatically = automatically;

	runNextAction();
}

void FiniteTimeActionComponent::vPostInit()
{
	pimpl->m_Target = m_Actor.lock()->getRenderComponent();
}

const std::string & FiniteTimeActionComponent::getType() const
{
	return Type;
}

const std::string FiniteTimeActionComponent::Type = "FiniteTimeActionComponent";
