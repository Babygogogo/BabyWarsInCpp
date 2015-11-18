#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToScene.h"

cocos2d::Scene * utilities::XMLToScene(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Scene::create();
}
