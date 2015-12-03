#ifndef __ACTION_LIST_ITEM_SCRIPT__
#define __ACTION_LIST_ITEM_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class ActionListItemScript : public BaseScriptComponent
{
public:
	ActionListItemScript();
	~ActionListItemScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	ActionListItemScript(const ActionListItemScript &) = delete;
	ActionListItemScript(ActionListItemScript &&) = delete;
	ActionListItemScript & operator=(const ActionListItemScript &) = delete;
	ActionListItemScript & operator=(ActionListItemScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ActionListItemScriptImpl;
	std::unique_ptr<ActionListItemScriptImpl> pimpl;
};

#endif // __ACTION_LIST_ITEM_SCRIPT__
