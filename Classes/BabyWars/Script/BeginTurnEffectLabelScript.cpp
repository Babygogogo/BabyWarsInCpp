#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../Event/EvtDataTurnPhaseChanged.h"
#include "TurnManagerScript.h"
#include "BeginTurnEffectLabelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BeginTurnEffectLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct BeginTurnEffectLabelScript::BeginTurnEffectLabelScriptImpl
{
	BeginTurnEffectLabelScriptImpl() = default;
	~BeginTurnEffectLabelScriptImpl() = default;

	void onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e);

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

void BeginTurnEffectLabelScript::BeginTurnEffectLabelScriptImpl::onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e)
{
	const auto turnManager = e.getTurnManagerScript();
	assert(turnManager && "BeginTurnEffectLabelScriptImpl::onTurnPhaseChanged() the turn manager script is nullptr.");

	auto label = static_cast<cocos2d::Label*>(m_RenderComponent->getSceneNode());
	const auto text = std::string("Turn: ") + std::to_string(turnManager->getCurrentTurnIndex()) + "  Player: " + turnManager->getCurrentPlayerID();
	label->setString(text);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BeginTurnEffectLabelScript.
//////////////////////////////////////////////////////////////////////////
BeginTurnEffectLabelScript::BeginTurnEffectLabelScript() : pimpl{ std::make_shared<BeginTurnEffectLabelScriptImpl>() }
{
}

BeginTurnEffectLabelScript::~BeginTurnEffectLabelScript()
{
}

bool BeginTurnEffectLabelScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void BeginTurnEffectLabelScript::vPostInit()
{
	pimpl->m_RenderComponent = getRenderComponent();

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataTurnPhaseChanged::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onTurnPhaseChanged(static_cast<const EvtDataTurnPhaseChanged &>(e));
	});
}

const std::string BeginTurnEffectLabelScript::Type{ "BeginTurnEffectLabelScript" };

const std::string & BeginTurnEffectLabelScript::getType() const
{
	return Type;
}
