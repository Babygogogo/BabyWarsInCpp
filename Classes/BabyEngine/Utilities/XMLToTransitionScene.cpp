#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToTransitionScene.h"
#include "XMLToColor3B.h"

namespace utilities
{
	cocos2d::TransitionFade * XMLToTransitionFade(tinyxml2::XMLElement *xmlElement, cocos2d::Scene * scene);
}

cocos2d::TransitionFade * utilities::XMLToTransitionFade(tinyxml2::XMLElement *xmlElement, cocos2d::Scene * scene)
{
	auto durationElement = xmlElement->FirstChildElement("Duration");
	assert(durationElement && "utilities::XMLToTransitionFade() the xml element has no duration element.");
	auto durationSec = durationElement->FloatAttribute("Value");

	auto color3BElement = xmlElement->FirstChildElement("Color3B");
	if (!color3BElement)
		return cocos2d::TransitionFade::create(durationSec, scene);

	return cocos2d::TransitionFade::create(durationSec, scene, XMLToColor3B(color3BElement));
}

cocos2d::TransitionScene * utilities::XMLToTransitionScene(tinyxml2::XMLElement * xmlElement, cocos2d::Scene * scene)
{
	assert(xmlElement && "utilities::XMLToTransitionScene() the xml element is nullptr.");

	auto typeName = std::string(xmlElement->Attribute("Type"));
	if (typeName == "TransitionFade")	return XMLToTransitionFade(xmlElement, scene);

	assert("utilities::XMLToTransitionScene() can't create transition scene possibly because the type is invalid.");
	return nullptr;
}
