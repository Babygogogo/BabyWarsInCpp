#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileData.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileDataImpl.
//////////////////////////////////////////////////////////////////////////
struct TileData::TileDataImpl
{
	TileDataImpl();
	~TileDataImpl();

	static float s_CommonWidth, s_CommonHeight;

	TileDataID m_ID{ 0 };
	std::string m_Type;
	cocos2d::Animation * m_Animation{ cocos2d::Animation::create() };
};

float TileData::TileDataImpl::s_CommonWidth;
float TileData::TileDataImpl::s_CommonHeight;

TileData::TileDataImpl::TileDataImpl()
{
	m_Animation->setDelayPerUnit(1);
	m_Animation->retain();
}

TileData::TileDataImpl::~TileDataImpl()
{
	m_Animation->release();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TileData.
//////////////////////////////////////////////////////////////////////////
TileData::TileData() : pimpl{ std::make_unique<TileDataImpl>() }
{
}

TileData::~TileData()
{
}

void TileData::setCommonSize(float width, float height)
{
	assert(width > 0. && height > 0. && "TileData::setCommonSize() with non-positive width or height.");
	TileDataImpl::s_CommonWidth = width;
	TileDataImpl::s_CommonHeight = height;
}

float TileData::getCommonWidth()
{
	return TileDataImpl::s_CommonWidth;
}

float TileData::getCommonHeight()
{
	return TileDataImpl::s_CommonHeight;
}

void TileData::initialize(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "TileData::initialize() failed to load xml file.");

	//Load ID and type name.
	pimpl->m_ID = rootElement->IntAttribute("ID");
	pimpl->m_Type = rootElement->Attribute("Type");

	//Load animation.
	auto animationFrames = cocos2d::Vector<cocos2d::AnimationFrame*>();
	const auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();
	const auto animationElement = rootElement->FirstChildElement("Animation");
	for (auto frameElement = animationElement->FirstChildElement("Frame"); frameElement; frameElement = frameElement->NextSiblingElement()){
		//Load a frame of the animation.
		auto spriteFrame = spriteFrameCache->getSpriteFrameByName(frameElement->Attribute("Name"));
		auto delaySec = frameElement->FloatAttribute("DelaySec");
		animationFrames.pushBack(cocos2d::AnimationFrame::create(spriteFrame, delaySec, cocos2d::ValueMap()));
	}
	pimpl->m_Animation->setFrames(animationFrames);
}

TileDataID TileData::getID() const
{
	return pimpl->m_ID;
}

std::string TileData::getType() const
{
	return pimpl->m_Type;
}

cocos2d::Animation * TileData::getAnimation() const
{
	return pimpl->m_Animation;
}
