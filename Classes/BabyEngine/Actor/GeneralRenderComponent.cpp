#include <cassert>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "GeneralRenderComponent.h"
#include "Actor.h"
#include "../Utilities/RelativePosition.h"
#include "../Utilities/XMLToSprite.h"
#include "../Utilities/XMLToNode.h"
#include "../Utilities/XMLToLayer.h"
#include "../Utilities/XMLToMenu.h"
#include "../Utilities/XMLToLabel.h"
#include "../Utilities/XMLToMenuItemImage.h"
#include "../Utilities/XMLToParticleExplosion.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Definition of DisplayNodeImpl.
//////////////////////////////////////////////////////////////////////////
struct GeneralRenderComponent::GeneralRenderComponentImpl
{
public:
	GeneralRenderComponentImpl();
	~GeneralRenderComponentImpl();
};

GeneralRenderComponent::GeneralRenderComponentImpl::GeneralRenderComponentImpl()
{
}

GeneralRenderComponent::GeneralRenderComponentImpl::~GeneralRenderComponentImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of DisplayNode.
//////////////////////////////////////////////////////////////////////////
GeneralRenderComponent::GeneralRenderComponent() : pimpl{ std::make_unique<GeneralRenderComponentImpl>() }
{
}

GeneralRenderComponent::~GeneralRenderComponent()
{
	CC_SAFE_RELEASE_NULL(m_Node);
}

bool GeneralRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "GeneralRenderComponent::vInit() the scene node is already initialized.");

	//Get the type of node from xmlElement.
	//auto nodeType = xmlElement->Attribute("Type");
	auto nodeElement = xmlElement->FirstChildElement("SceneNode");
	assert(nodeElement && "GeneralRenderComponent::vInit() the xml has no node element.");

	//Create the node as the type.
	//#TODO: Complete the if statements.
	//#TODO: Cache the frequently used xmls to avoid unnecessary disk i/o.
	auto nodeType = std::string(nodeElement->Attribute("Type"));
	if (nodeType == "Sprite")					m_Node = utilities::XMLToSprite(nodeElement);
	else if (nodeType == "Layer")				m_Node = utilities::XMLToLayer(nodeElement);
	else if (nodeType == "Node")				m_Node = utilities::XMLToNode(nodeElement);
	else if (nodeType == "Label")				m_Node = utilities::XMLToLabel(nodeElement);
	else if (nodeType == "Menu")				m_Node = utilities::XMLToMenu(nodeElement);
	else if (nodeType == "MenuItemImage")		m_Node = utilities::XMLToMenuItemImage(nodeElement);
	else if (nodeType == "ParticleExplosion")	m_Node = utilities::XMLToParticleExplosion(xmlElement);

	//Ensure that the node is created, then retain it.
	assert(m_Node && "GeneralRenderComponent::vInit() can't create a cocos2d::Node!");
	m_Node->retain();

	if (auto propertiesElement = xmlElement->FirstChildElement("SceneNodeProperties"))
		utilities::setSceneNodePropertiesWithXML(m_Node, propertiesElement);

	return true;
}

const std::string & GeneralRenderComponent::getType() const
{
	return Type;
}

const std::string GeneralRenderComponent::Type = "GeneralRenderComponent";
