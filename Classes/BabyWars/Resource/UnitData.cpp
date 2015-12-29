#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

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

	UnitDataType m_Type;

	UnitAnimation m_Animation;
	float m_MovingSpeedGridsPerSec{};

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

void UnitData::init(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "UnitData::init() the xml element is nullptr.");
	pimpl->m_Type = xmlElement->Attribute("Type");

	//Load movement data.
	auto movementElement = xmlElement->FirstChildElement("Movement");
	pimpl->m_Movement = movementElement->IntAttribute("Range");
	pimpl->m_MovementType = movementElement->Attribute("Type");

	//Load animations.
	pimpl->m_Animation.loadAnimation(xmlElement->FirstChildElement("Animation"));

	//Load other animation data.
	pimpl->m_MovingSpeedGridsPerSec = xmlElement->FirstChildElement("MovingSpeed")->FloatAttribute("Value");
}

const UnitDataType & UnitData::getType() const
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

cocos2d::Animation * UnitData::getAnimation(const ColorTypeCode & color) const
{
	return pimpl->m_Animation.getAnimation(UnitStateTypeCode::Idle, color);
}

float UnitData::getAnimationMovingSpeedGridPerSec() const
{
	return pimpl->m_MovingSpeedGridsPerSec;
}
