#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/XMLToSize.h"
#include "MoneyInfoPanelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MoneyInfoPanelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MoneyInfoPanelScript::MoneyInfoPanelScriptImpl
{
	MoneyInfoPanelScriptImpl() = default;
	~MoneyInfoPanelScriptImpl() = default;

	static std::string s_BackgroundActorPath;
	static cocos2d::Size s_PanelSize;
};

std::string MoneyInfoPanelScript::MoneyInfoPanelScriptImpl::s_BackgroundActorPath;
cocos2d::Size MoneyInfoPanelScript::MoneyInfoPanelScriptImpl::s_PanelSize;

//////////////////////////////////////////////////////////////////////////
//Implementation of MoneyInfoPanelScript.
//////////////////////////////////////////////////////////////////////////
MoneyInfoPanelScript::MoneyInfoPanelScript() : pimpl{ std::make_unique<MoneyInfoPanelScriptImpl>() }
{
}

MoneyInfoPanelScript::~MoneyInfoPanelScript()
{
}

bool MoneyInfoPanelScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	MoneyInfoPanelScriptImpl::s_BackgroundActorPath = relatedActorsElement->Attribute("Background");

	MoneyInfoPanelScriptImpl::s_PanelSize = utilities::XMLToSize(xmlElement->FirstChildElement("PanelSize"));

	isStaticMemberInitialized = true;
	return true;
}

void MoneyInfoPanelScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	auto backgroundActor = gameLogic->createActor(MoneyInfoPanelScriptImpl::s_BackgroundActorPath.c_str());
	backgroundActor->getComponent<TransformComponent>()->setContentSize(MoneyInfoPanelScriptImpl::s_PanelSize);
	ownerActor->addChild(*backgroundActor);
}

const std::string MoneyInfoPanelScript::Type{ "MoneyInfoPanelScript" };

const std::string & MoneyInfoPanelScript::getType() const
{
	return Type;
}
