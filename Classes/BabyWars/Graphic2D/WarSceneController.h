#ifndef __WAR_SCENE_CONTROLLER__
#define __WAR_SCENE_CONTROLLER__

#include <memory>

#include "../../BabyEngine/Graphic2D/IController.h"

class Actor;

class WarSceneController : public IController
{
public:
	WarSceneController();
	~WarSceneController();

private:
	//Override functions.
	virtual void setTarget(const std::shared_ptr<Actor> & actor) override;
	virtual void unsetTarget() override;
	virtual void setEnable(bool enable) override;

	//Implementation stuff.
	struct WarSceneControllerImpl;
	std::shared_ptr<WarSceneControllerImpl> pimpl;
};

#endif // !__WAR_SCENE_CONTROLLER__
