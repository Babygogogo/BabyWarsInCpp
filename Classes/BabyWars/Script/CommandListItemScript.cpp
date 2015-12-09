#include "cocos2d.h"
#include "ui/UIButton.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../Utilities/GameCommand.h"
#include "CommandListItemScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CommandListItemScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CommandListItemScript::CommandListItemScriptImpl
{
	CommandListItemScriptImpl() = default;
	~CommandListItemScriptImpl() = default;

	void initWithNecessaryComponents(const Actor & ownerActor);
	void initTouchCallback();

	void setAppearance(const std::string & titleName);
	void setTouchCallback(const GameCommand::Callback & callback);

	bool m_IsTouchMoved{ false };
	GameCommand::Callback m_GameCommandCallback;
	cocos2d::ui::Widget::ccWidgetTouchCallback m_TouchCallback;

	std::weak_ptr<CommandListItemScriptImpl> m_Self;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

void CommandListItemScript::CommandListItemScriptImpl::initWithNecessaryComponents(const Actor & ownerActor)
{
	auto renderComponent = ownerActor.getRenderComponent();
	assert(dynamic_cast<cocos2d::ui::Button*>(renderComponent->getSceneNode()) && "CommandListItemScript::initWithNecessaryComponents() the actor has no underlying cocos2d::ui::Button.");
	m_RenderComponent = std::move(renderComponent);
}

void CommandListItemScript::CommandListItemScriptImpl::initTouchCallback()
{
	assert(m_RenderComponent && "CommandListItemScriptImpl::initTouchCallback() the render component is not set yet.");

	using TouchEventType = cocos2d::ui::Widget::TouchEventType;

	m_TouchCallback = [scriptImpl = m_Self](cocos2d::Ref * target, TouchEventType touchType) {
		switch (touchType) {
		case TouchEventType::BEGAN:
			break;

		case TouchEventType::MOVED:
			if (!scriptImpl.expired()) {
				scriptImpl.lock()->m_IsTouchMoved = true;
			}
			break;

		case TouchEventType::CANCELED:
			break;

		case TouchEventType::ENDED:
			if (scriptImpl.expired()) {
				return;
			}

			auto strongScriptImpl = scriptImpl.lock();
			if (!strongScriptImpl->m_IsTouchMoved) {
				strongScriptImpl->m_GameCommandCallback();
			}

			strongScriptImpl->m_IsTouchMoved = false;
			break;
		}
	};

	static_cast<cocos2d::ui::Button*>(m_RenderComponent->getSceneNode())->addTouchEventListener(m_TouchCallback);
}

void CommandListItemScript::CommandListItemScriptImpl::setAppearance(const std::string & titleName)
{
	auto button = static_cast<cocos2d::ui::Button*>(m_RenderComponent->getSceneNode());
	button->setTitleText(titleName);
}

void CommandListItemScript::CommandListItemScriptImpl::setTouchCallback(const GameCommand::Callback & callback)
{
	m_GameCommandCallback = callback;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CommandListItemScript.
//////////////////////////////////////////////////////////////////////////
CommandListItemScript::CommandListItemScript() : pimpl{ std::make_shared<CommandListItemScriptImpl>() }
{
	pimpl->m_Self = pimpl;
}

CommandListItemScript::~CommandListItemScript()
{
}

void CommandListItemScript::initWithGameCommand(const GameCommand & command)
{
	pimpl->setAppearance(command.getName());
	pimpl->setTouchCallback(command.getCallback());
}

bool CommandListItemScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void CommandListItemScript::vPostInit()
{
	pimpl->initWithNecessaryComponents(*m_OwnerActor.lock());
	pimpl->initTouchCallback();
}

const std::string CommandListItemScript::Type{ "CommandListItemScript" };

const std::string & CommandListItemScript::getType() const
{
	return Type;
}
