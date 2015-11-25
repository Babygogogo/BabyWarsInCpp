#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "SpriteRenderComponent.h"
#include "../Utilities/XMLToSprite.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SpriteRenderComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct SpriteRenderComponent::SpriteRenderComponentImpl
{
	SpriteRenderComponentImpl() = default;
	~SpriteRenderComponentImpl() = default;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of SpriteRenderComponent.
//////////////////////////////////////////////////////////////////////////
SpriteRenderComponent::SpriteRenderComponent() : pimpl{ std::make_unique<SpriteRenderComponentImpl>() }
{
}

SpriteRenderComponent::~SpriteRenderComponent()
{
	CC_SAFE_RELEASE_NULL(m_Node);
}

void SpriteRenderComponent::setSpriteFrame(cocos2d::SpriteFrame * frame)
{
	assert(m_Node && "SpriteRenderComponent::setSpriteFrame() the sprite is not initialized.");

	static_cast<cocos2d::Sprite*>(m_Node)->setSpriteFrame(frame);
}

bool SpriteRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "SpriteRenderComponent::vInit() the sprite is already initialized.");
	m_Node = utilities::XMLToSprite(xmlElement->FirstChildElement("Sprite"));
	assert(m_Node && "SpriteRenderComponent::vInit() can't create sprite using the xml.");
	m_Node->retain();

	if (auto propertiesElement = xmlElement->FirstChildElement("SceneNodeProperties"))
		utilities::setSceneNodePropertiesWithXML(m_Node, propertiesElement);

	return true;
}

const std::string SpriteRenderComponent::Type{ "SpriteRenderComponent" };

const std::string & SpriteRenderComponent::getType() const
{
	return Type;
}
