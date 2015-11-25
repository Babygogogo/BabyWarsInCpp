#ifndef __ACTION_MENU_SCRIPT__
#define __ACTION_MENU_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class ActionMenuScript : public BaseScriptComponent
{
public:
	ActionMenuScript();
	~ActionMenuScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	ActionMenuScript(const ActionMenuScript &) = delete;
	ActionMenuScript(ActionMenuScript &&) = delete;
	ActionMenuScript & operator=(const ActionMenuScript &) = delete;
	ActionMenuScript & operator=(ActionMenuScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ActionMenuScriptImpl;
	std::unique_ptr<ActionMenuScriptImpl> pimpl;
};

#endif // __ACTION_MENU_SCRIPT__
