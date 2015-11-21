#ifndef __GENERAL_RENDER_COMPONENT__
#define __GENERAL_RENDER_COMPONENT__

#include <memory>

#include "BaseRenderComponent.h"

class GeneralRenderComponent final : public BaseRenderComponent
{
public:
	GeneralRenderComponent();
	~GeneralRenderComponent();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	GeneralRenderComponent(const GeneralRenderComponent&) = delete;
	GeneralRenderComponent(GeneralRenderComponent&&) = delete;
	GeneralRenderComponent& operator=(const GeneralRenderComponent&) = delete;
	GeneralRenderComponent& operator=(GeneralRenderComponent&&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;

	//Implementation stuff.
	struct GeneralRenderComponentImpl;
	std::unique_ptr<GeneralRenderComponentImpl> pimpl;
};

#endif // !__GENERAL_RENDER_COMPONENT__
