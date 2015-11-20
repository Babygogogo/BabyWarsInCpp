#ifndef __WAR_SCENE_CONTROLLER__
#define __WAR_SCENE_CONTROLLER__

#include "../../BabyEngine/Graphic2D/BaseController.h"

class WarSceneController : public BaseController
{
public:
	WarSceneController();
	~WarSceneController();

private:
	//Override functions.
	virtual void vSetTargetDelegate(const std::shared_ptr<Actor> & actor) override;
	virtual void vUnsetTargetDelegate() override;
	virtual void vSetEnableDelegate(bool enable, cocos2d::Node * targetSceneNode) override;

	//Implementation stuff.
	struct WarSceneControllerImpl;
	std::shared_ptr<WarSceneControllerImpl> pimpl;
};

#endif // !__WAR_SCENE_CONTROLLER__
