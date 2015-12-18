#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/ActionComponent.h"
#include "../../BabyEngine/Utilities/XMLToFiniteTimeAction.h"
#include "../Event/EvtDataBeginTurn.h"
#include "../Event/EvtDataBeginTurnEffectDisappeared.h"
#include "BeginTurnEffectScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BeginTurnEffectScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct BeginTurnEffectScript::BeginTurnEffectScriptImpl
{
	BeginTurnEffectScriptImpl() = default;
	~BeginTurnEffectScriptImpl();

	void onBeginTurn(const EvtDataBeginTurn & e);

	void initAppearingAction(const tinyxml2::XMLElement * appearingActionElement);
	void initDisappearingAction(const tinyxml2::XMLElement * disappearingActionElement);

	bool m_IsDisplaying{ false };
	cocos2d::Action * m_AppearingAction{ nullptr };
	cocos2d::Action * m_DisappearingAction{ nullptr };

	std::shared_ptr<ActionComponent> m_ActionComponent;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

BeginTurnEffectScript::BeginTurnEffectScriptImpl::~BeginTurnEffectScriptImpl()
{
	CC_SAFE_RELEASE_NULL(m_AppearingAction);
	CC_SAFE_RELEASE_NULL(m_DisappearingAction);
}

void BeginTurnEffectScript::BeginTurnEffectScriptImpl::onBeginTurn(const EvtDataBeginTurn & e)
{
	if (!m_IsDisplaying) {
		m_ActionComponent->runAction(m_AppearingAction);

		m_IsDisplaying = true;
	}
}

void BeginTurnEffectScript::BeginTurnEffectScriptImpl::initAppearingAction(const tinyxml2::XMLElement * appearingActionElement)
{
	CC_SAFE_RELEASE_NULL(m_AppearingAction);

	auto setVisibleFunc = cocos2d::CallFuncN::create([](auto node) {
		node->setVisible(true);
	});

	if (appearingActionElement) {
		if (auto rawAppearingAction = utilities::XMLToFiniteTimeAction(appearingActionElement->FirstChildElement())) {
			m_AppearingAction = cocos2d::Sequence::create(setVisibleFunc, rawAppearingAction, nullptr);
		}
	}
	else {
		m_AppearingAction = setVisibleFunc;
	}

	m_AppearingAction->retain();
}

void BeginTurnEffectScript::BeginTurnEffectScriptImpl::initDisappearingAction(const tinyxml2::XMLElement * disappearingActionElement)
{
	CC_SAFE_RELEASE_NULL(m_DisappearingAction);

	auto setVisibleFunc = cocos2d::CallFuncN::create([](auto node) {
		node->setVisible(false);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataBeginTurnEffectDisappeared>());
	});

	if (disappearingActionElement) {
		if (auto rawDisappearingAction = utilities::XMLToFiniteTimeAction(disappearingActionElement->FirstChildElement())) {
			m_DisappearingAction = cocos2d::Sequence::create(rawDisappearingAction, setVisibleFunc, nullptr);
		}
	}
	else {
		m_DisappearingAction = setVisibleFunc;
	}

	m_DisappearingAction->retain();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BeginTurnEffectScript.
//////////////////////////////////////////////////////////////////////////
BeginTurnEffectScript::BeginTurnEffectScript() : pimpl{ std::make_shared<BeginTurnEffectScriptImpl>() }
{
}

BeginTurnEffectScript::~BeginTurnEffectScript()
{
}

bool BeginTurnEffectScript::onInputTouch(const EvtDataInputTouch & touch)
{
	if (pimpl->m_IsDisplaying) {
		if (pimpl->m_AppearingAction->isDone()) {
			pimpl->m_ActionComponent->runAction(pimpl->m_DisappearingAction);
			pimpl->m_IsDisplaying = false;
		}

		return true;
	}

	return false;
}

bool BeginTurnEffectScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	if (const auto appearingActionElement = xmlElement->FirstChildElement("AppearingAction")) {
		pimpl->initAppearingAction(appearingActionElement);
	}
	if (const auto disappearingActionElement = xmlElement->FirstChildElement("DisappearingAction")) {
		pimpl->initDisappearingAction(disappearingActionElement);
	}

	return true;
}

void BeginTurnEffectScript::vPostInit()
{
	pimpl->m_RenderComponent = getRenderComponent();
	pimpl->m_ActionComponent = getComponent<ActionComponent>();

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataBeginTurn::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onBeginTurn(static_cast<const EvtDataBeginTurn &>(e));
	});
}

const std::string BeginTurnEffectScript::Type{ "BeginTurnEffectScript" };

const std::string & BeginTurnEffectScript::getType() const
{
	return Type;
}
