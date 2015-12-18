#ifndef __COMMAND_LIST_SCRIPT__
#define __COMMAND_LIST_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class CommandListScript : public BaseScriptComponent
{
public:
	CommandListScript();
	~CommandListScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	CommandListScript(const CommandListScript &) = delete;
	CommandListScript(CommandListScript &&) = delete;
	CommandListScript & operator=(const CommandListScript &) = delete;
	CommandListScript & operator=(CommandListScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct CommandListScriptImpl;
	std::shared_ptr<CommandListScriptImpl> pimpl;
};

#endif // __COMMAND_LIST_SCRIPT__
