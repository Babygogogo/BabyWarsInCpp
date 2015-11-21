#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToLabel.h"

cocos2d::Label * utilities::XMLToLabel(const tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")) {
		if (createWith->Attribute("FunctionName", "createWithSystemFont")) {
			auto text = createWith->Attribute("Text");
			auto fontName = createWith->Attribute("FontName");
			auto fontSize = createWith->FloatAttribute("FontSize");

			return cocos2d::Label::createWithSystemFont(text, fontName, fontSize);
		}
	}

	return cocos2d::Label::create();
}
