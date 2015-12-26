#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitData.h"
#include "ResourceLoader.h"
#include "UnitAnimation.h"
#include "../Utilities/UnitStateTypeCode.h"
#include "../Utilities/ColorTypeCode.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitDataImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitData::UnitDataImpl
{
	UnitDataImpl() = default;
	~UnitDataImpl() = default;

	UnitDataID m_ID{ 0 };
	std::string m_Type;

	UnitAnimation m_Animation;
	float m_MovingSpeedGridPerSec{};

	int m_Movement{};
	std::string m_MovementType;
};

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
	pimpl->m_Animation.loadAnimation(rootElement->FirstChildElement("Animation"));

	//Load other animation data.
	pimpl->m_MovingSpeedGridPerSec = rootElement->FirstChildElement("MovingSpeed")->FloatAttribute("Value");
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
	return pimpl->m_Animation.getAnimation(UnitStateTypeCode::Idle, ColorTypeCode::Orange);
}

float UnitData::getAnimationMovingSpeedGridPerSec() const
{
	return pimpl->m_MovingSpeedGridPerSec;
}
