#ifndef __BASE_CONTROLLER__
#define __BASE_CONTROLLER__

#include <memory>

//Forward declaration.
namespace cocos2d
{
	class Node;
}
class Actor;

class BaseController
{
public:
	virtual ~BaseController();

	void setTarget(const std::shared_ptr<Actor> & actor);
	void unsetTarget();
	void setEnable(bool enable);

protected:
	BaseController();

	virtual void vSetTargetDelegate(const std::shared_ptr<Actor> & actor) = 0;
	virtual void vUnsetTargetDelegate() = 0;
	virtual void vSetEnableDelegate(bool enable, cocos2d::Node * targetSceneNode) = 0;

private:
	//Implementation stuff.
	struct BaseControllerImpl;
	std::unique_ptr<BaseControllerImpl> pimpl;
};

#endif // !__BASE_CONTROLLER__
