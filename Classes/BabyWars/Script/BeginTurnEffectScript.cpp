#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../Event/EvtDataBeginTurn.h"
#include "BeginTurnEffectScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BeginTurnEffectScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct BeginTurnEffectScript::BeginTurnEffectScriptImpl
{
	BeginTurnEffectScriptImpl() = default;
	~BeginTurnEffectScriptImpl() = default;

	void onBeginTurn(const EvtDataBeginTurn & e);

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

void BeginTurnEffectScript::BeginTurnEffectScriptImpl::onBeginTurn(const EvtDataBeginTurn & e)
{
	m_RenderComponent->getSceneNode()->setVisible(true);
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
	auto sceneNode = pimpl->m_RenderComponent->getSceneNode();
	if (sceneNode->isVisible()) {
		sceneNode->setVisible(false);
		return true;
	}

	return false;
}

bool BeginTurnEffectScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void BeginTurnEffectScript::vPostInit()
{
	pimpl->m_RenderComponent = getRenderComponent();

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
