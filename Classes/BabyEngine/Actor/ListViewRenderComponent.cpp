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

const std::string ListViewRenderComponent::Type{ "ListViewRenderComponent" };

const std::string & ListViewRenderComponent::getType() const
{
	return Type;
}
