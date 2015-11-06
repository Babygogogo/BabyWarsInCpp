#ifndef __WAR_SCENE_CONTROLLER__
#define __WAR_SCENE_CONTROLLER__

#include "../../BabyEngine/Graphic2D/IController.h"

class WarSceneController : public IController
{
public:
	WarSceneController();
	~WarSceneController();

	void setEnable(bool enable);

	virtual void setTarget(const std::shared_ptr<Actor> & actor) override;

private:
	struct WarSceneControllerImpl;
	std::shared_ptr<WarSceneControllerImpl> pimpl;
};

#endif // !__WAR_SCENE_CONTROLLER__
