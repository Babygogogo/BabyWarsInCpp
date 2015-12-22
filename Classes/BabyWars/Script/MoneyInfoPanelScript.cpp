#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
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
};

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

	isStaticMemberInitialized = true;
	return true;
}

void MoneyInfoPanelScript::vPostInit()
{
}

const std::string MoneyInfoPanelScript::Type{ "MoneyInfoPanelScript" };

const std::string & MoneyInfoPanelScript::getType() const
{
	return Type;
}
