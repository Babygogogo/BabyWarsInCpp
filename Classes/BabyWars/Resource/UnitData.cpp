#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitData.h"
#include "ResourceLoader.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitDataImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitData::UnitDataImpl
{
	UnitDataImpl();
	~UnitDataImpl();

	cocos2d::Vector<cocos2d::AnimationFrame*> loadAnimationFrames(tinyxml2::XMLElement * animationElement) const;

	UnitDataID m_ID{ 0 };
	std::string m_Type;
	cocos2d::Animation * m_NormalAnimation{ cocos2d::Animation::create() };
	float m_DefaultScaleFactorX{}, m_DefaultScaleFactorY{};
	float m_MovingSpeed{};

	int m_Movement{};
	std::string m_MovementType;
};

UnitData::UnitDataImpl::UnitDataImpl()
{
	m_NormalAnimation->setDelayPerUnit(1);
	m_NormalAnimation->retain();
}

UnitData::UnitDataImpl::~UnitDataImpl()
{
	m_NormalAnimation->release();
}

cocos2d::Vector<cocos2d::AnimationFrame*> UnitData::UnitDataImpl::loadAnimationFrames(tinyxml2::XMLElement * animationElement) const
{
	auto animationFrames = cocos2d::Vector<cocos2d::AnimationFrame*>();
	const auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();

	for (auto frameElement = animationElement->FirstChildElement("Frame"); frameElement; frameElement = frameElement->NextSiblingElement()) {
		//Load a frame of the animation.
		auto spriteFrame = spriteFrameCache->getSpriteFrameByName(frameElement->Attribute("Name"));
		auto delaySec = frameElement->FloatAttribute("DelaySec");
		animationFrames.pushBack(cocos2d::AnimationFrame::create(spriteFrame, delaySec, cocos2d::ValueMap()));
	}
	assert(animationFrames.size() > 0 && "UnitData::initialize() the animation is empty.");

	return animationFrames;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of UnitData.
//////////////////////////////////////////////////////////////////////////
UnitData::UnitData() : pimpl{ std::make_unique<UnitDataImpl>() }
{
}

UnitData::~UnitData()
{
}

void UnitData::initialize(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "UnitData::initialize() failed to load xml file.");

	//Load ID and type name.
	pimpl->m_ID = rootElement->IntAttribute("ID");
	pimpl->m_Type = rootElement->Attribute("Type");

	//Load movement data.
	auto movementElement = rootElement->FirstChildElement("Movement");
	pimpl->m_Movement = movementElement->IntAttribute("Range");
	pimpl->m_MovementType = movementElement->Attribute("Type");

	//Load animations.
	auto animationsElement = rootElement->FirstChildElement("Animation");
	pimpl->m_NormalAnimation->setFrames(pimpl->loadAnimationFrames(animationsElement->FirstChildElement("Normal")));

	//Load other animation data.
	auto othersElement = animationsElement->FirstChildElement("Others");
	pimpl->m_MovingSpeed = othersElement->FloatAttribute("MovingSpeed");

	//Calculate the design scale factor.
	auto spriteFrameSize = pimpl->m_NormalAnimation->getFrames().at(0)->getSpriteFrame()->getOriginalSize();
	auto designGridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_DefaultScaleFactorX = designGridSize.width / spriteFrameSize.width;
	pimpl->m_DefaultScaleFactorY = designGridSize.height / spriteFrameSize.height;
}

UnitDataID UnitData::getID() const
{
	return pimpl->m_ID;
}

std::string UnitData::getType() const
{
	return pimpl->m_Type;
}

int UnitData::getMovementRange() const
{
	return pimpl->m_Movement;
}

const std::string & UnitData::getMovementType() const
{
	return pimpl->m_MovementType;
}

cocos2d::Animation * UnitData::getAnimation() const
{
	return pimpl->m_NormalAnimation;
}

float UnitData::getAnimationMovingSpeed() const
{
	return pimpl->m_MovingSpeed;
}

float UnitData::getDefaultScaleFactorX() const
{
	return pimpl->m_DefaultScaleFactorX;
}

float UnitData::getDefaultScaleFactorY() const
{
	return pimpl->m_DefaultScaleFactorY;
}
