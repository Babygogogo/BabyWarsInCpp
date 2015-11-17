#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileData.h"
#include "ResourceLoader.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileDataImpl.
//////////////////////////////////////////////////////////////////////////
struct TileData::TileDataImpl
{
	TileDataImpl();
	~TileDataImpl();

	TileDataID m_ID{ 0 };
	std::string m_Type;

	std::unordered_map<std::string, int> m_MovingCost;

	cocos2d::Animation * m_Animation{ cocos2d::Animation::create() };
	float m_DefaultScaleFactorX{}, m_DefaultScaleFactorY{};
};

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

	//Load the moving cost.
	auto movingCostListElement = rootElement->FirstChildElement("MovingCostList");
	for (auto movingCost = movingCostListElement->FirstChildElement("MovingCost"); movingCost; movingCost = movingCost->NextSiblingElement())
		pimpl->m_MovingCost.emplace(movingCost->Attribute("MovementType"), movingCost->IntAttribute("Cost"));

	//Load animation.
	auto animationFrames = cocos2d::Vector<cocos2d::AnimationFrame*>();
	const auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();
	const auto animationElement = rootElement->FirstChildElement("Animation");
	for (auto frameElement = animationElement->FirstChildElement("Frame"); frameElement; frameElement = frameElement->NextSiblingElement()) {
		//Load a frame of the animation.
		auto spriteFrame = spriteFrameCache->getSpriteFrameByName(frameElement->Attribute("Name"));
		auto delaySec = frameElement->FloatAttribute("DelaySec");
		animationFrames.pushBack(cocos2d::AnimationFrame::create(spriteFrame, delaySec, cocos2d::ValueMap()));
	}
	assert(animationFrames.size() > 0 && "TileData::initialize() the animation is empty.");
	pimpl->m_Animation->setFrames(animationFrames);

	//Calculate the design scale factor.
	auto spriteFrameSize = animationFrames.at(0)->getSpriteFrame()->getOriginalSize();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_DefaultScaleFactorX = gridSize.width / spriteFrameSize.width;
	pimpl->m_DefaultScaleFactorY = gridSize.height / spriteFrameSize.height;
}

TileDataID TileData::getID() const
{
	return pimpl->m_ID;
}

std::string TileData::getType() const
{
	return pimpl->m_Type;
}

int TileData::getMovingCost(const std::string & movementType) const
{
	auto costIter = pimpl->m_MovingCost.find(movementType);
	if (costIter == pimpl->m_MovingCost.end())
		return 0;

	return costIter->second;
}

cocos2d::Animation * TileData::getAnimation() const
{
	return pimpl->m_Animation;
}

float TileData::getDefaultScaleFactorX() const
{
	return pimpl->m_DefaultScaleFactorX;
}

float TileData::getDefaultScaleFactorY() const
{
	return pimpl->m_DefaultScaleFactorY;
}
