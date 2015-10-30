#include <cassert>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "GeneralRenderComponent.h"
#include "Actor.h"
#include "../Utilities/RelativePosition.h"

//////////////////////////////////////////////////////////////////////////
//Definition of DisplayNodeImpl.
//////////////////////////////////////////////////////////////////////////
struct GeneralRenderComponent::GeneralRenderComponentImpl
{
public:
	GeneralRenderComponentImpl();
	~GeneralRenderComponentImpl();

	cocos2d::Node * createNode(tinyxml2::XMLElement * xmlElement);
	cocos2d::Sprite * createSprite(tinyxml2::XMLElement * xmlElement);
	cocos2d::Layer * createLayer(tinyxml2::XMLElement * xmlElement);
	cocos2d::Scene * createScene(tinyxml2::XMLElement * xmlElement);
	cocos2d::Label * createLabel(tinyxml2::XMLElement * xmlElement);
	cocos2d::Menu * createMenu(tinyxml2::XMLElement * xmlElement);
	cocos2d::MenuItemImage * createMenuItemImage(tinyxml2::XMLElement *xmlElement);
	cocos2d::ParticleExplosion * createParticleExplosion(tinyxml2::XMLElement * xmlElement);
};

GeneralRenderComponent::GeneralRenderComponentImpl::GeneralRenderComponentImpl()
{
}

GeneralRenderComponent::GeneralRenderComponentImpl::~GeneralRenderComponentImpl()
{
}

cocos2d::Node * GeneralRenderComponent::GeneralRenderComponentImpl::createNode(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Node::create();
}

cocos2d::Sprite * GeneralRenderComponent::GeneralRenderComponentImpl::createSprite(tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")){
		if (auto fileName = createWith->Attribute("FileName"))
			return cocos2d::Sprite::create(fileName);
		if (auto spriteFrameName = createWith->Attribute("SpriteFrameName"))
			return cocos2d::Sprite::createWithSpriteFrameName(spriteFrameName);
	}

	return cocos2d::Sprite::create();
}

cocos2d::Layer * GeneralRenderComponent::GeneralRenderComponentImpl::createLayer(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Layer::create();
}

cocos2d::Scene * GeneralRenderComponent::GeneralRenderComponentImpl::createScene(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Scene::create();
}

cocos2d::Label * GeneralRenderComponent::GeneralRenderComponentImpl::createLabel(tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")){
		if (createWith->Attribute("FunctionName", "createWithSystemFont")){
			auto text = createWith->Attribute("Text");
			auto fontName = createWith->Attribute("FontName");
			auto fontSize = createWith->FloatAttribute("FontSize");

			return cocos2d::Label::createWithSystemFont(text, fontName, fontSize);
		}
	}

	return cocos2d::Label::create();
}

cocos2d::Menu * GeneralRenderComponent::GeneralRenderComponentImpl::createMenu(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Menu::create();
}

cocos2d::MenuItemImage * GeneralRenderComponent::GeneralRenderComponentImpl::createMenuItemImage(tinyxml2::XMLElement *xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")){
		auto menuItemImage = cocos2d::MenuItemImage::create();
		auto spriteFrameCache = cocos2d::SpriteFrameCache::getInstance();

		if (auto normalName = createWith->Attribute("NormalSpriteFrameName"))
			menuItemImage->setNormalSpriteFrame(spriteFrameCache->getSpriteFrameByName(normalName));
		if (auto selectedName = createWith->Attribute("SelectedSpriteFrameName"))
			menuItemImage->setSelectedSpriteFrame(spriteFrameCache->getSpriteFrameByName(selectedName));

		return menuItemImage;
	}

	return cocos2d::MenuItemImage::create();
}

cocos2d::ParticleExplosion * GeneralRenderComponent::GeneralRenderComponentImpl::createParticleExplosion(tinyxml2::XMLElement * xmlElement)
{
	auto particle = cocos2d::ParticleExplosion::create();
	if (auto properties = xmlElement->FirstChildElement("Properties")){
		if (auto texture = properties->Attribute("Texture"))
			particle->setTexture(cocos2d::Director::getInstance()->getTextureCache()->addImage(texture));
		if (auto totalParticles = properties->IntAttribute("TotalParticles"))
			particle->setTotalParticles(totalParticles);
		if (auto startSize = properties->FloatAttribute("StartSize"))
			particle->setStartSize(startSize);
		if (auto life = properties->FloatAttribute("Life"))
			particle->setLife(life);
		if (auto speed = properties->FloatAttribute("Speed"))
			particle->setSpeed(speed);
		if (auto speedVariance = properties->FloatAttribute("SpeedVariance"))
			particle->setSpeedVar(speedVariance);

		if (auto startColorVariance = properties->FirstChildElement("StartColorVariance")){
			auto r = startColorVariance->FloatAttribute("R") / 255.0f;
			auto g = startColorVariance->FloatAttribute("G") / 255.0f;
			auto b = startColorVariance->FloatAttribute("B") / 255.0f;
			auto a = startColorVariance->FloatAttribute("A") / 255.0f;
			particle->setStartColorVar({ r, g, b, a });
		}
		if (auto endColorVariance = properties->FirstChildElement("EndColorVariance")){
			auto r = endColorVariance->FloatAttribute("R") / 255.0f;
			auto g = endColorVariance->FloatAttribute("G") / 255.0f;
			auto b = endColorVariance->FloatAttribute("B") / 255.0f;
			auto a = endColorVariance->FloatAttribute("A") / 255.0f;
			particle->setEndColorVar({ r, g, b, a });
		}
		if (auto gravity = properties->FirstChildElement("Gravity")){
			auto x = gravity->FloatAttribute("X");
			auto y = gravity->FloatAttribute("Y");
			particle->setGravity({ x, y });
		}
	}

	return particle;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of DisplayNode.
//////////////////////////////////////////////////////////////////////////
GeneralRenderComponent::GeneralRenderComponent() : pimpl{ std::make_unique<GeneralRenderComponentImpl>() }
{
}

GeneralRenderComponent::~GeneralRenderComponent()
{
}

bool GeneralRenderComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Get the type of node from xmlElement.
	auto nodeType = xmlElement->Attribute("Type");

	//Create the node as the type.
	//#TODO: Complete the if statements.
	//#TODO: Cache the frequently used xmls to avoid unnecessary disk i/o.
	if (strcmp(nodeType, "Sprite") == 0)
		m_Node = pimpl->createSprite(xmlElement);
	else if (strcmp(nodeType, "Layer") == 0)
		m_Node = pimpl->createLayer(xmlElement);
	else if (strcmp(nodeType, "Scene") == 0)
		m_Node = pimpl->createScene(xmlElement);
	else if (strcmp(nodeType, "Node") == 0)
		m_Node = pimpl->createNode(xmlElement);
	else if (strcmp(nodeType, "Label") == 0)
		m_Node = pimpl->createLabel(xmlElement);
	else if (strcmp(nodeType, "Menu") == 0)
		m_Node = pimpl->createMenu(xmlElement);
	else if (strcmp(nodeType, "MenuItemImage") == 0)
		m_Node = pimpl->createMenuItemImage(xmlElement);
	else if (strcmp(nodeType, "ParticleExplosion") == 0)
		m_Node = pimpl->createParticleExplosion(xmlElement);

	//Ensure that the node is created, then retain it.
	assert(m_Node && "GeneralRenderComponent::vInit() can't create a cocos2d::Node!");
	m_Node->retain();

	//Set some extra data if presents in the xmlElement.
	if (auto positionElement = xmlElement->FirstChildElement("Position")){
		setPosition(RelativePosition(positionElement));

		auto localZOrder = positionElement->IntAttribute("LocalZOrder");
		m_Node->setLocalZOrder(localZOrder);
	}

	return true;
}

const std::string & GeneralRenderComponent::getType() const
{
	return Type;
}

const std::string GeneralRenderComponent::Type = "GeneralRenderComponent";
