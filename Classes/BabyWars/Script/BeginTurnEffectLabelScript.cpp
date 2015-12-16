#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../Event/EvtDataBeginTurn.h"
#include "BeginTurnEffectLabelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BeginTurnEffectLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct BeginTurnEffectLabelScript::BeginTurnEffectLabelScriptImpl
{
	BeginTurnEffectLabelScriptImpl() = default;
	~BeginTurnEffectLabelScriptImpl() = default;

	void onBeginTurn(const EvtDataBeginTurn & e);

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

void BeginTurnEffectLabelScript::BeginTurnEffectLabelScriptImpl::onBeginTurn(const EvtDataBeginTurn & e)
{
	auto label = static_cast<cocos2d::Label*>(m_RenderComponent->getSceneNode());
	const auto text = std::string("Turn: ") + std::to_string(e.getTurnIndex()) + "  Player: " + std::to_string(e.getPlayerID());

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
	eventDispatcher->vAddListener(EvtDataBeginTurn::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onBeginTurn(static_cast<const EvtDataBeginTurn &>(e));
	});
}

const std::string BeginTurnEffectLabelScript::Type{ "BeginTurnEffectLabelScript" };

const std::string & BeginTurnEffectLabelScript::getType() const
{
	return Type;
}
