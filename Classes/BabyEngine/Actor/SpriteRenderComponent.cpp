#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "SpriteRenderComponent.h"
#include "../../BabyEngine/Utilities/XMLToSprite.h"

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

bool SpriteRenderComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	m_Node = utilities::XMLToSprite(xmlElement);
	assert(m_Node && "SpriteRenderComponent::vInit() can't create sprite using the xml.");
	m_Node->retain();

	return true;
}

const std::string SpriteRenderComponent::Type{ "SpriteRenderComponent" };

const std::string & SpriteRenderComponent::getType() const
{
	return Type;
}
