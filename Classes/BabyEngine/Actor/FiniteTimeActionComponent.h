#ifndef __FINITE_TIME_ACTION_COMPONENT__
#define __FINITE_TIME_ACTION_COMPONENT__

#include <memory>
#include <functional>

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class FiniteTimeAction;
}

/*!
 * \class FiniteTimeActionComponent
 *
 * \brief A queue of finite time actions that target the scene node of the actor.
 *
 * \details
 *	Requires that the actor has one render component.
 *	The queued actions can be run manually (by default) or automatically.
 *
 * \author Babygogogo
 * \date 2015.9
 */
class FiniteTimeActionComponent final :public ActorComponent
{
public:
	FiniteTimeActionComponent();
	~FiniteTimeActionComponent();

	static const std::string Type;
	virtual const std::string & getType() const;

	using Callback = std::function<void()>;

	//Methods for queuing actions. The callback (if provided) is called when the main action is done.
	void queueAction(cocos2d::FiniteTimeAction* action, Callback && callback = nullptr);
	void queueBlink(float duration_s, int blinkCount, Callback && callback = nullptr);
	void queueCallback(Callback && callback);
	void queueDelay(float delay_s, Callback && callback = nullptr);
	void queueMoveTo(float duration_s, float x, float y, Callback && callback = nullptr);

	void stopAndClearAllActions();

	//In the automatically-run-mode, all the actions in the queue will be run one by one, automatically.
	//In the manually-run-mode (by default), you must manually call runNextAction() to run an action in the queue.
	void setRunAutomatically(bool automatically);

	//Run the first action in the queue.
	//If it's currently running an action, nothing happens.
	bool runNextAction();
	bool isRunning() const;

	//Disable copy/move constructor and operator=.
	FiniteTimeActionComponent(const FiniteTimeActionComponent&) = delete;
	FiniteTimeActionComponent(FiniteTimeActionComponent&&) = delete;
	FiniteTimeActionComponent& operator=(const FiniteTimeActionComponent&) = delete;
	FiniteTimeActionComponent& operator=(FiniteTimeActionComponent&&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct FiniteTimeActionComponentImpl;
	std::shared_ptr<FiniteTimeActionComponentImpl> pimpl;
};

#endif // !__FINITE_TIME_ACTION_COMPONENT__
