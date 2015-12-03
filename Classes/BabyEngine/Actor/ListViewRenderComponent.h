#ifndef __LIST_VIEW_RENDER_COMPONENT__
#define __LIST_VIEW_RENDER_COMPONENT__

#include "BaseRenderComponent.h"

class ListViewRenderComponent : public BaseRenderComponent
{
public:
	ListViewRenderComponent();
	~ListViewRenderComponent();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	ListViewRenderComponent(const ListViewRenderComponent &) = delete;
	ListViewRenderComponent(ListViewRenderComponent &&) = delete;
	ListViewRenderComponent & operator=(const ListViewRenderComponent &) = delete;
	ListViewRenderComponent & operator=(ListViewRenderComponent &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;

	//Implementation stuff.
	struct ListViewRenderComponentImpl;
	std::unique_ptr<ListViewRenderComponentImpl> pimpl;
};

#endif // !__LIST_VIEW_RENDER_COMPONENT__
