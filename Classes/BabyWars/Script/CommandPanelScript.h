#ifndef __COMMAND_PANEL_SCRIPT__
#define __COMMAND_PANEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class CommandPanelScript : public BaseScriptComponent
{
public:
	CommandPanelScript();
	~CommandPanelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	CommandPanelScript(const CommandPanelScript &) = delete;
	CommandPanelScript(CommandPanelScript &&) = delete;
	CommandPanelScript & operator=(const CommandPanelScript &) = delete;
	CommandPanelScript & operator=(CommandPanelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct CommandPanelScriptImpl;
	std::unique_ptr<CommandPanelScriptImpl> pimpl;
};

#endif // __COMMAND_PANEL_SCRIPT__
