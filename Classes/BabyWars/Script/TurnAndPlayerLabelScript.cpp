#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../Event/EvtDataTurnPhaseChanged.h"
#include "TurnManagerScript.h"
#include "TurnAndPlayerLabelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TurnAndPlayerLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TurnAndPlayerLabelScript::TurnAndPlayerLabelScriptImpl
{
	TurnAndPlayerLabelScriptImpl() = default;
	~TurnAndPlayerLabelScriptImpl() = default;

	void onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e);

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

void TurnAndPlayerLabelScript::TurnAndPlayerLabelScriptImpl::onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e)
{
	const auto turnManager = e.getTurnManagerScript();
	assert(turnManager && "TurnAndPlayerLabelScriptImpl::onTurnPhaseChanged() the turn manager script is nullptr.");

	auto label = static_cast<cocos2d::Label*>(m_RenderComponent->getSceneNode());
	const auto text = std::string("Turn: ") + std::to_string(turnManager->getCurrentTurnIndex()) + "  Player: " + turnManager->getCurrentPlayerID();
	label->setString(text);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TurnAndPlayerLabelScript.
//////////////////////////////////////////////////////////////////////////
TurnAndPlayerLabelScript::TurnAndPlayerLabelScript() : pimpl{ std::make_unique<TurnAndPlayerLabelScriptImpl>() }
{
}

TurnAndPlayerLabelScript::~TurnAndPlayerLabelScript()
{
}

bool TurnAndPlayerLabelScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void TurnAndPlayerLabelScript::vPostInit()
{
	pimpl->m_RenderComponent = getRenderComponent();
	assert(pimpl->m_RenderComponent && "TurnAndPlayerLabelScript::vPostInit() the actor has no render component.");
	assert(dynamic_cast<cocos2d::Label*>(pimpl->m_RenderComponent->getSceneNode()) && "TurnAndPlayerLabelScript::vPostInit() the scene node inside the render component is not cocos2d::Label");

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataTurnPhaseChanged::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onTurnPhaseChanged(static_cast<const EvtDataTurnPhaseChanged &>(e));
	});
}

const std::string TurnAndPlayerLabelScript::Type{ "TurnAndPlayerLabelScript" };

const std::string & TurnAndPlayerLabelScript::getType() const
{
	return Type;
}
