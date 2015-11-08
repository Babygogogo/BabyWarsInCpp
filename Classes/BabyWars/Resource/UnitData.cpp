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

	UnitDataID m_ID{ 0 };
	std::string m_Type;
	cocos2d::Animation * m_Animation{ cocos2d::Animation::create() };
	float m_DesignScaleFactor{};

	int m_Movement{ 0 };
	std::string m_MovementType;
};

UnitData::UnitDataImpl::UnitDataImpl()
{
	m_Animation->setDelayPerUnit(1);
	m_Animation->retain();
}

UnitData::UnitDataImpl::~UnitDataImpl()
{
	m_Animation->release();
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
	assert(animationFrames.size() > 0 && "UnitData::initialize() the animation is empty.");
	pimpl->m_Animation->setFrames(animationFrames);

	//Calculate the design scale factor.
	auto spriteFrameSize = animationFrames.at(0)->getSpriteFrame()->getOriginalSize();
	auto designGridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_DesignScaleFactor = std::max(designGridSize.width / spriteFrameSize.width, designGridSize.height / spriteFrameSize.height);
}

UnitDataID UnitData::getID() const
{
	return pimpl->m_ID;
}

std::string UnitData::getType() const
{
	return pimpl->m_Type;
}

int UnitData::getMovement() const
{
	return pimpl->m_Movement;
}

const std::string & UnitData::getMovementType() const
{
	return pimpl->m_MovementType;
}

cocos2d::Animation * UnitData::getAnimation() const
{
	return pimpl->m_Animation;
}

float UnitData::getDesignScaleFactor() const
{
	return pimpl->m_DesignScaleFactor;
}
