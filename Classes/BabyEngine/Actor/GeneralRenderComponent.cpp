#include <cassert>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "GeneralRenderComponent.h"
#include "../Utilities/XMLToSceneNode.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Implementation of GeneralRenderComponent.
//////////////////////////////////////////////////////////////////////////
GeneralRenderComponent::GeneralRenderComponent()
{
}

GeneralRenderComponent::~GeneralRenderComponent()
{
	if (!m_Node) {
		return;
	}

	m_Node->removeAllChildren();
	m_Node->removeFromParent();
	CC_SAFE_RELEASE_NULL(m_Node);
}

bool GeneralRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "GeneralRenderComponent::vInit() the scene node is already initialized.");

	//Create the scene node using the xml.
	//#TODO: Cache the frequently used xmls to avoid unnecessary disk i/o.
	m_Node = utilities::XMLToSceneNode(xmlElement->FirstChildElement("SceneNode"));
	assert(m_Node && "GeneralRenderComponent::vInit() can't create an instance of cocos2d::Node or its sub class!");
	m_Node->retain();

	if (auto propertiesElement = xmlElement->FirstChildElement("SceneNodeProperties")) {
		utilities::setSceneNodePropertiesWithXML(m_Node, propertiesElement);
	}

	return true;
}

const std::string GeneralRenderComponent::Type{ "GeneralRenderComponent" };

const std::string & GeneralRenderComponent::getType() const
{
	return Type;
}
