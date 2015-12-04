#ifndef __COMMAND_LIST_ITEM_SCRIPT__
#define __COMMAND_LIST_ITEM_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
class GameCommand;

class CommandListItemScript : public BaseScriptComponent
{
public:
	CommandListItemScript();
	~CommandListItemScript();

	void initWithGameCommand(const GameCommand & command);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	CommandListItemScript(const CommandListItemScript &) = delete;
	CommandListItemScript(CommandListItemScript &&) = delete;
	CommandListItemScript & operator=(const CommandListItemScript &) = delete;
	CommandListItemScript & operator=(CommandListItemScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct CommandListItemScriptImpl;
	std::unique_ptr<CommandListItemScriptImpl> pimpl;
};

#endif // __COMMAND_LIST_ITEM_SCRIPT__
