#ifndef __XML_TO_LIST_VIEW__
#define __XML_TO_LIST_VIEW__

//Forward declaration.
namespace cocos2d
{
	namespace ui
	{
		class ListView;
	}
}
namespace tinyxml2
{
	class XMLElement;
}

namespace utilities
{
	cocos2d::ui::ListView * XMLToListView(const tinyxml2::XMLElement * xmlElement);
}

#endif // !__XML_TO_LIST_VIEW__
