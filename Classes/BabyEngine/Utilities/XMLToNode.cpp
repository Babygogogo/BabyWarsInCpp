#include "cocos2d.h"

#include "XMLToNode.h"

cocos2d::Node * utilities::XMLToNode(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Node::create();
}
