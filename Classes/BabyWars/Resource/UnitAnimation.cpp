#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitAnimation.h"
#include "../Utilities/XMLToColorTypeCode.h"
#include "../Utilities/XMLToUnitStateTypeCode.h"
#include "../../BabyEngine/Utilities/XMLToAnimation.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitAnimationImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitAnimation::UnitAnimationImpl
{
	UnitAnimationImpl() = default;
	~UnitAnimationImpl();

	std::map<UnitStateTypeCode, std::map<ColorTypeCode, cocos2d::Animation*>> m_Animations;
};

UnitAnimation::UnitAnimationImpl::~UnitAnimationImpl()
{
	for (auto & unitStateIter : m_Animations) {
		for (auto & colorAnimationPair : unitStateIter.second) {
			CC_SAFE_RELEASE_NULL(colorAnimationPair.second);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of UnitAnimation.
//////////////////////////////////////////////////////////////////////////
UnitAnimation::UnitAnimation() : pimpl{ std::make_unique<UnitAnimationImpl>() }
{
}

UnitAnimation::~UnitAnimation()
{
}

void UnitAnimation::loadAnimation(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "UnitAnimation::loadAnimation() the xml element is nullptr.");
	for (auto animationElement = xmlElement->FirstChildElement("Animation"); animationElement; animationElement = animationElement->NextSiblingElement("Animation")) {
		const auto stateTypeCode = utilities::StringToUnitStateTypeCode(animationElement->Attribute("State"));
		const auto colorTypeCode = utilities::StringToColorTypeCode(animationElement->Attribute("Color"));

		auto animation = utilities::XMLToAnimation(animationElement);
		animation->retain();
		pimpl->m_Animations[std::move(stateTypeCode)][std::move(colorTypeCode)] = std::move(animation);
	}
}

cocos2d::Animation * UnitAnimation::getAnimation(UnitStateTypeCode unitStateTypeCode, ColorTypeCode colorTypeCode) const
{
	const auto unitStateIter = pimpl->m_Animations.find(unitStateTypeCode);
	if (unitStateIter == pimpl->m_Animations.end()) {
		return nullptr;
	}

	const auto colorIter = unitStateIter->second.find(colorTypeCode);
	if (colorIter == unitStateIter->second.end()) {
		return nullptr;
	}

	return colorIter->second;
}
