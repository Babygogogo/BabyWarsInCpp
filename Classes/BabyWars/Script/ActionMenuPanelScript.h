#ifndef __ACTION_MENU_PANEL_SCRIPT__
#define __ACTION_MENU_PANEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class ActionMenuPanelScript : public BaseScriptComponent
{
public:
	ActionMenuPanelScript();
	~ActionMenuPanelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	ActionMenuPanelScript(const ActionMenuPanelScript &) = delete;
	ActionMenuPanelScript(ActionMenuPanelScript &&) = delete;
	ActionMenuPanelScript & operator=(const ActionMenuPanelScript &) = delete;
	ActionMenuPanelScript & operator=(ActionMenuPanelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ActionMenuPanelScriptImpl;
	std::unique_ptr<ActionMenuPanelScriptImpl> pimpl;
};

#endif // __ACTION_MENU_PANEL_SCRIPT__
