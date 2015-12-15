#ifndef __WAR_SCENE_HUD_SCRIPT__
#define __WAR_SCENE_HUD_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class EvtDataInputTouch;

class WarSceneHUDScript : public BaseScriptComponent
{
public:
	WarSceneHUDScript();
	~WarSceneHUDScript();

	bool onInputTouch(const EvtDataInputTouch & touch);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	WarSceneHUDScript(const WarSceneHUDScript &) = delete;
	WarSceneHUDScript(WarSceneHUDScript &&) = delete;
	WarSceneHUDScript & operator=(const WarSceneHUDScript &) = delete;
	WarSceneHUDScript & operator=(WarSceneHUDScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct WarSceneHUDScriptImpl;
	std::unique_ptr<WarSceneHUDScriptImpl> pimpl;
};

#endif // __WAR_SCENE_HUD_SCRIPT__
