#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "ActionListItemScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionListItemScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionListItemScript::ActionListItemScriptImpl
{
	ActionListItemScriptImpl() = default;
	~ActionListItemScriptImpl() = default;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionListItemScript.
//////////////////////////////////////////////////////////////////////////
ActionListItemScript::ActionListItemScript() : pimpl{ std::make_unique<ActionListItemScriptImpl>() }
{
}

ActionListItemScript::~ActionListItemScript()
{
}

bool ActionListItemScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void ActionListItemScript::vPostInit()
{
}

const std::string ActionListItemScript::Type{ "ActionListItemScript" };

const std::string & ActionListItemScript::getType() const
{
	return Type;
}
