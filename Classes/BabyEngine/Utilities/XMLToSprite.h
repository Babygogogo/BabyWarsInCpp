#ifndef __XML_TO_SPRITE__
#define __XML_TO_SPRITE__

//Forward declaration.
namespace cocos2d
{
	class Sprite;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Sprite * XMLToSprite(tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SPRITE__
