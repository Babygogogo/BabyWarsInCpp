#ifndef __RELATIVE_POSITION__
#define __RELATIVE_POSITION__

namespace tinyxml2{
	class XMLElement;
}
namespace cocos2d{
	class Node;
}

class RelativePosition
{
public:
	RelativePosition();
	RelativePosition(tinyxml2::XMLElement *	positionElement);
	~RelativePosition();

	void initialize(tinyxml2::XMLElement * positionElement);

	float getAbsolutePositionX(cocos2d::Node * node = nullptr) const;
	float getAbsolutePositionY(cocos2d::Node * node = nullptr) const;

	float m_NormalizedScreenOffsetX{ 0.0f }, m_NormalizedScreenOffsetY{ 0.0f };
	float m_NormalizedNodeOffsetX{ 0.0f }, m_NormalizedNodeOffsetY{ 0.0f };
	float m_PixelOffsetX{ 0.0f }, m_PixelOffsetY{ 0.0f };
};

#endif // !__RELATIVE_POSITION__
