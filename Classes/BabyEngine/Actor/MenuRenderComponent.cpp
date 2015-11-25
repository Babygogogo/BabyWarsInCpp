#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MenuRenderComponent.h"
#include "../Utilities/XMLToMenu.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MenuRenderComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct MenuRenderComponent::MenuRenderComponentImpl
{
	MenuRenderComponentImpl() = default;
	~MenuRenderComponentImpl() = default;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of MenuRenderComponent.
//////////////////////////////////////////////////////////////////////////
MenuRenderComponent::MenuRenderComponent() : pimpl{ std::make_unique<MenuRenderComponentImpl>() }
{
}

MenuRenderComponent::~MenuRenderComponent()
{
	CC_SAFE_RELEASE_NULL(m_Node);
}

bool MenuRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "MenuRenderComponent::vInit() the sprite is already initialized.");
	m_Node = utilities::XMLToMenu(xmlElement);
	assert(m_Node && "MenuRenderComponent::vInit() can't create sprite using the xml.");
	m_Node->retain();

	if (auto propertiesElement = xmlElement->FirstChildElement("SceneNodeProperties"))
		utilities::setSceneNodePropertiesWithXML(m_Node, propertiesElement);

	return true;
}

const std::string MenuRenderComponent::Type{ "MenuRenderComponent" };

const std::string & MenuRenderComponent::getType() const
{
	return Type;
}
