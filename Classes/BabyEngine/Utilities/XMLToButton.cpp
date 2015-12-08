#include "ui/UIButton.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToButton.h"
#include "XMLToRect.h"
#include "XMLToColor3B.h"

namespace utilities
{
	enum class ButtonTextureType
	{
		Normal,
		Pressed,
		Disabled
	};

	std::string XMLToButtonTextureName(const tinyxml2::XMLElement * textureNameElement)
	{
		return textureNameElement->Attribute("Value");
	}

	cocos2d::ui::Button::TextureResType XMLToButtonTextureResType(const tinyxml2::XMLElement * textureResourceTypeElement)
	{
		const auto typeString = std::string(textureResourceTypeElement->Attribute("Value"));
		if (typeString == "Local")
			return cocos2d::ui::Button::TextureResType::LOCAL;

		return cocos2d::ui::Button::TextureResType::PLIST;
	}

	void setButtonTextureWithXML(cocos2d::ui::Button * button, const tinyxml2::XMLElement * textureElement, ButtonTextureType textureType)
	{
		if (!button || !textureElement)
			return;

		//Load the texture.
		const auto textureName = XMLToButtonTextureName(textureElement->FirstChildElement("TextureName"));
		const auto textureResourceType = XMLToButtonTextureResType(textureElement->FirstChildElement("TextureResourceType"));
		switch (textureType) {
		case ButtonTextureType::Normal:		button->loadTextureNormal(textureName, textureResourceType);	break;
		case ButtonTextureType::Pressed:	button->loadTexturePressed(textureName, textureResourceType);	break;
		case ButtonTextureType::Disabled:	button->loadTextureDisabled(textureName, textureResourceType);	break;
		default:																							break;
		}

		//Set stuff of scale9.
		if (const auto capInsetsElement = textureElement->FirstChildElement("CapInsets")) {
			button->setScale9Enabled(true);
			const auto capInsets = XMLToRect(capInsetsElement);

			switch (textureType) {
			case ButtonTextureType::Normal:		button->setCapInsetsNormalRenderer(capInsets);		break;
			case ButtonTextureType::Pressed:	button->setCapInsetsPressedRenderer(capInsets);		break;
			case ButtonTextureType::Disabled:	button->setCapInsetsDisabledRenderer(capInsets);	break;
			default:																				break;
			}
		}
	}

	void setButtonTexturesWithXML(cocos2d::ui::Button * button, const tinyxml2::XMLElement * buttonElement)
	{
		if (!button || !buttonElement)
			return;

		setButtonTextureWithXML(button, buttonElement->FirstChildElement("NormalTexture"), ButtonTextureType::Normal);
		setButtonTextureWithXML(button, buttonElement->FirstChildElement("PressedTexture"), ButtonTextureType::Pressed);
		setButtonTextureWithXML(button, buttonElement->FirstChildElement("DisabledTexture"), ButtonTextureType::Disabled);
	}

	void setButtonZoomScaleWithXML(cocos2d::ui::Button * button, const tinyxml2::XMLElement * zoomScaleElement)
	{
		if (!button || !zoomScaleElement)
			return;

		button->setZoomScale(zoomScaleElement->FloatAttribute("Value"));
	}

	void setButtonTitlePropertiesWithXML(cocos2d::ui::Button * button, const tinyxml2::XMLElement * titlePropertiesElement)
	{
		if (!button || !titlePropertiesElement) {
			return;
		}

		if (const auto fontNameElement = titlePropertiesElement->FirstChildElement("FontName")) {
			button->setTitleFontName(fontNameElement->Attribute("Value"));
		}

		if (const auto fontSizeElement = titlePropertiesElement->FirstChildElement("FontSize")) {
			button->setTitleFontSize(fontSizeElement->FloatAttribute("Value"));
		}

		if (const auto colorElement = titlePropertiesElement->FirstChildElement("TitleColor")) {
			button->setTitleColor(XMLToColor3B(colorElement));
		}
	}
}

cocos2d::ui::Button * utilities::XMLToButton(const tinyxml2::XMLElement * xmlElement)
{
	auto button = cocos2d::ui::Button::create();

	setButtonTexturesWithXML(button, xmlElement);
	setButtonZoomScaleWithXML(button, xmlElement->FirstChildElement("ZoomScale"));
	setButtonTitlePropertiesWithXML(button, xmlElement->FirstChildElement("TitleProperties"));

	return button;
}
