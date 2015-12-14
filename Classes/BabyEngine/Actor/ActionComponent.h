#ifndef __ACTION_COMPONENT__
#define __ACTION_COMPONENT__

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Action;
}

class ActionComponent : public ActorComponent
{
public:
	ActionComponent();
	~ActionComponent();

	void runAction(cocos2d::Action * action);
	void stopAction(cocos2d::Action * action);
	void stopAllActions();

	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	ActionComponent(const ActionComponent &) = delete;
	ActionComponent(ActionComponent &&) = delete;
	ActionComponent & operator=(const ActionComponent &) = delete;
	ActionComponent & operator=(ActionComponent &&) = delete;

private:
	virtual void vPostInit() override;

	struct ActionComponentImpl;
	std::unique_ptr<ActionComponentImpl> pimpl;
};

#endif // !__ACTION_COMPONENT__
