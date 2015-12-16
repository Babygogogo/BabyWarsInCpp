#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToSceneNodeAddChildCallback.h"

namespace utilities
{
	template<typename ParentType, typename ChildType>
	std::function<void(cocos2d::Node*, cocos2d::Node*)> XMLToSceneNodeAddChildCallback(const tinyxml2::XMLElement * xmlElement)
	{
		assert(xmlElement && "utilities::XMLToSceneNodeAddChildCallback() the xml element is nullptr.");
		const auto method = std::string(xmlElement->Attribute("Method"));

		if (method == "pushBackCustomItem") {
			return [](cocos2d::Node * parent, cocos2d::Node * child) {
				static_cast<ParentType*>(parent)->pushBackCustomItem(static_cast<ChildType*>(child));
			};
		}

		return nullptr;
	}
}

std::function<void(cocos2d::Node*, cocos2d::Node*)> utilities::XMLToSceneNodeAddChildCallback(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "utilities::XMLToSceneNodeAddChildCallback() the xml element is nullptr.");

	const auto parentType = std::string(xmlElement->Attribute("ParentType"));
	const auto childType = std::string(xmlElement->Attribute("ChildType"));

	if (parentType == "ListView" && childType == "Widget") {
		return XMLToSceneNodeAddChildCallback<cocos2d::ui::ListView, cocos2d::ui::Widget>(xmlElement);
	}

	return nullptr;
}
