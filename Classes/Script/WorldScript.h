#ifndef __WORLD_SCRIPT__
#define __WORLD_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

class WorldScript : public BaseScriptComponent
{
public:
	WorldScript();
	~WorldScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Load a tile map with a xml file. Create tile and unit actors if needed.
	void loadWorld(const char * xmlPath);

	//Disable copy/move constructor and operator=.
	WorldScript(const WorldScript &) = delete;
	WorldScript(WorldScript &&) = delete;
	WorldScript & operator=(const WorldScript &) = delete;
	WorldScript & operator=(WorldScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct WorldScriptImpl;
	std::unique_ptr<WorldScriptImpl> pimpl;
};

#endif // !__WORLD_SCRIPT__
