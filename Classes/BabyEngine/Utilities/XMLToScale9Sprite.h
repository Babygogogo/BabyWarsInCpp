#ifndef __XML_TO_SCALE_9_SPRITE__
#define __XML_TO_SCALE_9_SPRITE__

//Forward declaration.
namespace cocos2d
{
	namespace ui
	{
		class Scale9Sprite;
	}
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::ui::Scale9Sprite * XMLToScale9Sprite(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_SCALE_9_SPRITE__
