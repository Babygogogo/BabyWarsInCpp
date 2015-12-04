#include "ui/UIListView.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "ListViewRenderComponent.h"
#include "../Utilities/XMLToListView.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ListViewRenderComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct ListViewRenderComponent::ListViewRenderComponentImpl
{
	ListViewRenderComponentImpl() = default;
	~ListViewRenderComponentImpl() = default;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of ListViewRenderComponent.
//////////////////////////////////////////////////////////////////////////
ListViewRenderComponent::ListViewRenderComponent() : pimpl{ std::make_unique<ListViewRenderComponentImpl>() }
{
}

ListViewRenderComponent::~ListViewRenderComponent()
{
	CC_SAFE_RELEASE_NULL(m_Node);
}

void ListViewRenderComponent::forceDoLayout()
{
	static_cast<cocos2d::ui::ListView *>(m_Node)->forceDoLayout();
}

bool ListViewRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "ListViewRenderComponent::vInit() the node is already initialized.");
	m_Node = utilities::XMLToListView(xmlElement->FirstChildElement("ListView"));
	assert(m_Node && "ListViewRenderComponent::vInit() can't create node using the xml.");
	m_Node->retain();

	if (auto propertiesElement = xmlElement->FirstChildElement("SceneNodeProperties"))
		utilities::setSceneNodePropertiesWithXML(m_Node, propertiesElement);

	return true;
}

void ListViewRenderComponent::vAddChild(const BaseRenderComponent & child)
{
	auto childWidget = dynamic_cast<cocos2d::ui::Widget*>(child.m_Node);
	assert(childWidget && "ListViewRenderComponent::vAddChild() the underlying scene node of the child is not a cocos2d::ui::Widget*.");

	static_cast<cocos2d::ui::ListView*>(m_Node)->pushBackCustomItem(childWidget);
}

const std::string ListViewRenderComponent::Type{ "ListViewRenderComponent" };

const std::string & ListViewRenderComponent::getType() const
{
	return Type;
}
