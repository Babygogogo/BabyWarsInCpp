#ifndef __XML_TO_LAYER__
#define __XML_TO_LAYER__

//Forward declaration.
namespace cocos2d
{
	class Layer;
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::Layer * XMLToLayer(tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_LAYER__
