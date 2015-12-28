#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToAnimation.h"

cocos2d::Animation * utilities::XMLToAnimation(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToAnimation() the xml element is nullptr.");

	auto animationFrames = cocos2d::Vector<cocos2d::AnimationFrame*>();
	const auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();

	for (auto frameElement = xmlElement->FirstChildElement("Frame"); frameElement; frameElement = frameElement->NextSiblingElement()) {
		//Load a frame of the animation.
		auto spriteFrame = spriteFrameCache->getSpriteFrameByName(frameElement->Attribute("Name"));
		auto delaySec = frameElement->FloatAttribute("DelaySec");
		animationFrames.pushBack(cocos2d::AnimationFrame::create(spriteFrame, delaySec, cocos2d::ValueMap()));
	}

	return cocos2d::Animation::create(animationFrames, 1, xmlElement->IntAttribute("LoopCount"));
}
