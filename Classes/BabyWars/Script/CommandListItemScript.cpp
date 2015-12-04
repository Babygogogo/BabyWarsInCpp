#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../Utilities/GameCommand.h"
#include "CommandListItemScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CommandListItemScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CommandListItemScript::CommandListItemScriptImpl
{
	CommandListItemScriptImpl() = default;
	~CommandListItemScriptImpl() = default;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of CommandListItemScript.
//////////////////////////////////////////////////////////////////////////
CommandListItemScript::CommandListItemScript() : pimpl{ std::make_unique<CommandListItemScriptImpl>() }
{
}

CommandListItemScript::~CommandListItemScript()
{
}

void CommandListItemScript::initWithGameCommand(const GameCommand & command)
{
}

bool CommandListItemScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void CommandListItemScript::vPostInit()
{
}

const std::string CommandListItemScript::Type{ "CommandListItemScript" };

const std::string & CommandListItemScript::getType() const
{
	return Type;
}
