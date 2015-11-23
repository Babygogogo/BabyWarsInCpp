#ifndef __WAR_SCENE_SCRIPT__
#define __WAR_SCENE_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class WarSceneScript : public BaseScriptComponent
{
public:
	WarSceneScript();
	~WarSceneScript();

	void loadWarScene(const char * xmlPath);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	WarSceneScript(const WarSceneScript &) = delete;
	WarSceneScript(WarSceneScript &&) = delete;
	WarSceneScript & operator=(const WarSceneScript &) = delete;
	WarSceneScript & operator=(WarSceneScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct WarSceneScriptImpl;
	std::shared_ptr<WarSceneScriptImpl> pimpl;
};

#endif // !__WAR_SCENE_SCRIPT__
