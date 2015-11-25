#ifndef __MENU_RENDER_COMPONENT__
#define __MENU_RENDER_COMPONENT__

#include "BaseRenderComponent.h"

class MenuRenderComponent : public BaseRenderComponent
{
public:
	MenuRenderComponent();
	~MenuRenderComponent();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	MenuRenderComponent(const MenuRenderComponent &) = delete;
	MenuRenderComponent(MenuRenderComponent &&) = delete;
	MenuRenderComponent & operator=(const MenuRenderComponent &) = delete;
	MenuRenderComponent & operator=(MenuRenderComponent &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;

	//Implementation stuff.
	struct MenuRenderComponentImpl;
	std::unique_ptr<MenuRenderComponentImpl> pimpl;
};

#endif // !__MENU_RENDER_COMPONENT__
