#ifndef __MONEY_INFO_PANEL_SCRIPT__
#define __MONEY_INFO_PANEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class MoneyInfoPanelScript : public BaseScriptComponent
{
public:
	MoneyInfoPanelScript();
	~MoneyInfoPanelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	MoneyInfoPanelScript(const MoneyInfoPanelScript &) = delete;
	MoneyInfoPanelScript(MoneyInfoPanelScript &&) = delete;
	MoneyInfoPanelScript & operator=(const MoneyInfoPanelScript &) = delete;
	MoneyInfoPanelScript & operator=(MoneyInfoPanelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MoneyInfoPanelScriptImpl;
	std::unique_ptr<MoneyInfoPanelScriptImpl> pimpl;
};

#endif // __MONEY_INFO_PANEL_SCRIPT__
