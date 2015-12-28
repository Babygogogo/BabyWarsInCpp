#include "ui/UIListView.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToListView.h"

namespace utilities
{
	void setListViewGravityWithXML(cocos2d::ui::ListView * listView, const tinyxml2::XMLElement * gravityElement)
	{
		if (!gravityElement || !listView)
			return;

		static const auto left = std::string{ "Left" };
		static const auto right = std::string{ "Right" };
		static const auto center_horizontal = std::string{ "Center_horizontal" };
		static const auto top = std::string{ "Top" };
		static const auto bottom = std::string{ "Bottom" };
		static const auto center_vertical = std::string{ "Center_vertical" };

		using ListView = cocos2d::ui::ListView;
		const auto gravity = std::string(gravityElement->Attribute("Value"));
		if (gravity == left)					listView->setGravity(ListView::Gravity::LEFT);
		else if (gravity == right)				listView->setGravity(ListView::Gravity::RIGHT);
		else if (gravity == center_horizontal)	listView->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
		else if (gravity == top)				listView->setGravity(ListView::Gravity::TOP);
		else if (gravity == bottom)				listView->setGravity(ListView::Gravity::BOTTOM);
		else if (gravity == center_vertical)	listView->setGravity(ListView::Gravity::CENTER_VERTICAL);
	}

	void setListViewItemsMarginWithXML(cocos2d::ui::ListView * listView, const tinyxml2::XMLElement * itemsMarginElement)
	{
		if (!listView || !itemsMarginElement)
			return;

		listView->setItemsMargin(itemsMarginElement->FloatAttribute("Value"));
	}
}

cocos2d::ui::ListView * utilities::XMLToListView(const tinyxml2::XMLElement * xmlElement)
{
	auto listView = cocos2d::ui::ListView::create();

	setListViewGravityWithXML(listView, xmlElement->FirstChildElement("Gravity"));
	setListViewItemsMarginWithXML(listView, xmlElement->FirstChildElement("ItemsMargin"));

	//#TODO: Load list view properties from the xml.

	return listView;
}
