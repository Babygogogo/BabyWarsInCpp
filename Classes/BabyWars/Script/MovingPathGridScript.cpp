#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "MovingPathGridScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Utilities/AdjacentDirection.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/MovingPath.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MovingPathGridScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MovingPathGridScript::MovingPathGridScriptImpl
{
	MovingPathGridScriptImpl() = default;
	~MovingPathGridScriptImpl() = default;

	const std::string & getSpriteFrameName(AdjacentDirection previous, AdjacentDirection next) const;
	void setAppearance(cocos2d::Sprite * sprite, const std::string & spriteFrameName);

	static std::string s_SpriteFrameNameEmpty;
	static std::string s_SpriteFrameNameArrowUpper;
	static std::string s_SpriteFrameNameArrowBelow;
	static std::string s_SpriteFrameNameArrowLeft;
	static std::string s_SpriteFrameNameArrowRight;
	static std::string s_SpriteFrameNameCornerUpperLeft;
	static std::string s_SpriteFrameNameCornerUpperRight;
	static std::string s_SpriteFrameNameCornerBelowLeft;
	static std::string s_SpriteFrameNameCornerBelowRight;
	static std::string s_SpriteFrameNameLineHorizontal;
	static std::string s_SpriteFrameNameLineVertical;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameEmpty;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameArrowUpper;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameArrowBelow;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameArrowLeft;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameArrowRight;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameCornerUpperLeft;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameCornerUpperRight;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameCornerBelowLeft;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameCornerBelowRight;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameLineHorizontal;
std::string MovingPathGridScript::MovingPathGridScriptImpl::s_SpriteFrameNameLineVertical;

const std::string & MovingPathGridScript::MovingPathGridScriptImpl::getSpriteFrameName(AdjacentDirection previous, AdjacentDirection next) const
{
	if (previous == next)
		return s_SpriteFrameNameEmpty;

	switch (previous)
	{
	case AdjacentDirection::INVALID:
		switch (next)
		{
		case AdjacentDirection::INVALID:	return s_SpriteFrameNameEmpty;				break;
		case AdjacentDirection::UPPER:		return s_SpriteFrameNameLineVertical;		break;
		case AdjacentDirection::BELOW:		return s_SpriteFrameNameLineVertical;		break;
		case AdjacentDirection::LEFT:		return s_SpriteFrameNameLineHorizontal;		break;
		case AdjacentDirection::RIGHT:		return s_SpriteFrameNameLineHorizontal;		break;
		default:							return s_SpriteFrameNameEmpty;				break;
		}
	case AdjacentDirection::UPPER:
		switch (next)
		{
		case AdjacentDirection::INVALID:	return s_SpriteFrameNameArrowBelow;			break;
		case AdjacentDirection::UPPER:		return s_SpriteFrameNameEmpty;				break;
		case AdjacentDirection::BELOW:		return s_SpriteFrameNameLineVertical;		break;
		case AdjacentDirection::LEFT:		return s_SpriteFrameNameCornerUpperLeft;	break;
		case AdjacentDirection::RIGHT:		return s_SpriteFrameNameCornerUpperRight;	break;
		default:							return s_SpriteFrameNameEmpty;				break;
		}
	case AdjacentDirection::BELOW:
		switch (next)
		{
		case AdjacentDirection::INVALID:	return s_SpriteFrameNameArrowUpper;			break;
		case AdjacentDirection::UPPER:		return s_SpriteFrameNameLineVertical;		break;
		case AdjacentDirection::BELOW:		return s_SpriteFrameNameEmpty;				break;
		case AdjacentDirection::LEFT:		return s_SpriteFrameNameCornerBelowLeft;	break;
		case AdjacentDirection::RIGHT:		return s_SpriteFrameNameCornerBelowRight;	break;
		default:							return s_SpriteFrameNameEmpty;				break;
		}
	case AdjacentDirection::LEFT:
		switch (next)
		{
		case AdjacentDirection::INVALID:	return s_SpriteFrameNameArrowRight;			break;
		case AdjacentDirection::UPPER:		return s_SpriteFrameNameCornerUpperLeft;	break;
		case AdjacentDirection::BELOW:		return s_SpriteFrameNameCornerBelowLeft;	break;
		case AdjacentDirection::LEFT:		return s_SpriteFrameNameEmpty;				break;
		case AdjacentDirection::RIGHT:		return s_SpriteFrameNameLineHorizontal;		break;
		default:							return s_SpriteFrameNameEmpty;				break;
		}
	case AdjacentDirection::RIGHT:
		switch (next)
		{
		case AdjacentDirection::INVALID:	return s_SpriteFrameNameArrowLeft;			break;
		case AdjacentDirection::UPPER:		return s_SpriteFrameNameCornerUpperRight;	break;
		case AdjacentDirection::BELOW:		return s_SpriteFrameNameCornerBelowRight;	break;
		case AdjacentDirection::LEFT:		return s_SpriteFrameNameLineHorizontal;		break;
		case AdjacentDirection::RIGHT:		return s_SpriteFrameNameEmpty;				break;
		default:							return s_SpriteFrameNameEmpty;				break;
		}
	default:
		return s_SpriteFrameNameEmpty;
	}
}

void MovingPathGridScript::MovingPathGridScriptImpl::setAppearance(cocos2d::Sprite * sprite, const std::string & spriteFrameName)
{
	if (spriteFrameName.empty()) {
		sprite->setVisible(false);
		return;
	}

	sprite->setSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName));

	//#TODO: Set the scale.
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MovingPathGridScript.
//////////////////////////////////////////////////////////////////////////
MovingPathGridScript::MovingPathGridScript() : pimpl{ std::make_unique<MovingPathGridScriptImpl>() }
{
}

MovingPathGridScript::~MovingPathGridScript()
{
}

void MovingPathGridScript::setAppearanceAndPosition(const GridIndex & index, AdjacentDirection previous, AdjacentDirection next)
{
	auto renderComponent = pimpl->m_RenderComponent.lock();
	auto sprite = renderComponent->getSceneNode<cocos2d::Sprite>();
	sprite->setPosition(index.toPosition(SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize()));

	pimpl->setAppearance(sprite, pimpl->getSpriteFrameName(previous, next));
}

bool MovingPathGridScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto spriteFrameNames = xmlElement->FirstChildElement("SpriteFrameNames");
	MovingPathGridScriptImpl::s_SpriteFrameNameArrowUpper = spriteFrameNames->Attribute("ArrowUpper");
	MovingPathGridScriptImpl::s_SpriteFrameNameArrowBelow = spriteFrameNames->Attribute("ArrowBelow");
	MovingPathGridScriptImpl::s_SpriteFrameNameArrowLeft = spriteFrameNames->Attribute("ArrowLeft");
	MovingPathGridScriptImpl::s_SpriteFrameNameArrowRight = spriteFrameNames->Attribute("ArrowRight");
	MovingPathGridScriptImpl::s_SpriteFrameNameCornerUpperLeft = spriteFrameNames->Attribute("CornerUpperLeft");
	MovingPathGridScriptImpl::s_SpriteFrameNameCornerUpperRight = spriteFrameNames->Attribute("CornerUpperRight");
	MovingPathGridScriptImpl::s_SpriteFrameNameCornerBelowLeft = spriteFrameNames->Attribute("CornerBelowLeft");
	MovingPathGridScriptImpl::s_SpriteFrameNameCornerBelowRight = spriteFrameNames->Attribute("CornerBelowRight");
	MovingPathGridScriptImpl::s_SpriteFrameNameLineHorizontal = spriteFrameNames->Attribute("LineHorizontal");
	MovingPathGridScriptImpl::s_SpriteFrameNameLineVertical = spriteFrameNames->Attribute("LineVertical");

	isStaticInitialized = true;
	return true;
}

void MovingPathGridScript::vPostInit()
{
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
}

const std::string MovingPathGridScript::Type{ "MovingPathGridScript" };

const std::string & MovingPathGridScript::getType() const
{
	return Type;
}