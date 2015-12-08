#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "TouchAndDragController.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TouchAndDragController.
//////////////////////////////////////////////////////////////////////////
struct TouchAndDragController::TouchAndDragControllerImpl
{
	TouchAndDragControllerImpl();
	~TouchAndDragControllerImpl();

	bool m_IsTouchMoved{ false };
	std::weak_ptr<Actor> m_TargetActor;

	cocos2d::Vec2 m_TouchOneByOneBeganPosition;
	cocos2d::EventListenerTouchOneByOne * m_TouchOneByOneListener{ cocos2d::EventListenerTouchOneByOne::create() };
};

TouchAndDragController::TouchAndDragControllerImpl::TouchAndDragControllerImpl()
{
	m_TouchOneByOneListener->retain();

	m_TouchOneByOneListener->onTouchBegan = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		return !m_TargetActor.expired();
	};
	m_TouchOneByOneListener->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		if (m_TargetActor.expired())
			return;

		auto dragState = EvtDataInputDrag::DragState::Dragging;
		if (!m_IsTouchMoved) {
			dragState = EvtDataInputDrag::DragState::Begin;
			m_IsTouchMoved = true;
		}
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataInputDrag>(m_TargetActor.lock()->getID(), touch->getLocation(), touch->getPreviousLocation(), dragState));
	};
	m_TouchOneByOneListener->onTouchEnded = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		if (m_TargetActor.expired())
			return;

		if (m_IsTouchMoved) {
			SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataInputDrag>(m_TargetActor.lock()->getID(), touch->getLocation(), touch->getPreviousLocation(), EvtDataInputDrag::DragState::End));
			m_IsTouchMoved = false;
		}
		else
			SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataInputTouch>(m_TargetActor.lock()->getID(), touch->getLocation()));
	};
}

TouchAndDragController::TouchAndDragControllerImpl::~TouchAndDragControllerImpl()
{
	m_TouchOneByOneListener->release();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TouchAndDragController.
//////////////////////////////////////////////////////////////////////////
TouchAndDragController::TouchAndDragController() : pimpl{ std::make_unique<TouchAndDragControllerImpl>() }
{
}

TouchAndDragController::~TouchAndDragController()
{
	unsetTarget();
}

void TouchAndDragController::vSetTargetDelegate(const std::shared_ptr<Actor> & actor)
{
	pimpl->m_TargetActor = actor;
}

void TouchAndDragController::vUnsetTargetDelegate()
{
	pimpl->m_TargetActor.reset();
}

void TouchAndDragController::vSetEnableDelegate(bool enable, cocos2d::Node * targetSceneNode)
{
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	if (enable)
		eventDispatcher->addEventListenerWithSceneGraphPriority(pimpl->m_TouchOneByOneListener, targetSceneNode);
	else
		eventDispatcher->removeEventListener(pimpl->m_TouchOneByOneListener);
}
