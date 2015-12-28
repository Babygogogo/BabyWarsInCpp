#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/StringToVector.h"
#include "../Event/EvtDataAllUnitsUnfocused.h"
#include "../Event/EvtDataMakeMovingPathEnd.h"
#include "../Event/EvtDataTurnPhaseChanged.h"
#include "../Event/EvtDataUnitIndexChangeEnd.h"
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2D.h"
#include "../Utilities/MovingPath.h"
#include "../Utilities/UnitState.h"
#include "../Utilities/TurnPhaseTypeCode.h"
#include "../Utilities/TurnPhase.h"
#include "../Utilities/XMLToActorUnit.h"
#include "UnitMapScript.h"
#include "UnitScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitMapScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitMapScript::UnitMapScriptImpl
{
	UnitMapScriptImpl() = default;
	~UnitMapScriptImpl() = default;

	void onMakeMovingPathEnd(const EvtDataMakeMovingPathEnd & e);
	void onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e);
	void onUnitIndexChangeEnd(const EvtDataUnitIndexChangeEnd & e);
	void onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e);

	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	std::shared_ptr<UnitScript> getUnit(const GridIndex & gridIndex) const;
	std::shared_ptr<UnitScript> getFocusedUnit() const;
	bool canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const;

	static std::string s_UnitActorPath;

	bool m_CanRespondToTouch{ false };

	Matrix2D<std::weak_ptr<UnitScript>> m_UnitMap;
	std::weak_ptr<UnitScript> m_FocusedUnit;

	std::weak_ptr<Actor> m_OwnerActor;
	std::weak_ptr<TransformComponent> m_TransformComponent;
};

std::string UnitMapScript::UnitMapScriptImpl::s_UnitActorPath;

void UnitMapScript::UnitMapScriptImpl::onMakeMovingPathEnd(const EvtDataMakeMovingPathEnd & e)
{
	auto focusUnit = getFocusedUnit();
	assert(focusUnit && "UnitMapScriptImpl::onMakeMovingPathEnd() there is no focus unit.");

	if (canUnitStayAtIndex(*focusUnit, e.getMovingPath().getBackNode().m_GridIndex)) {
		focusUnit->moveAlongPath(e.getMovingPath());
	}
}

void UnitMapScript::UnitMapScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	auto changedUnit = e.getUnitScript();
	assert(changedUnit && "UnitMapScriptImpl::onUnitStateChangeEnd() the unit is expired.");
	auto currentState = e.getCurrentState();
	assert(currentState && "UnitMapScriptImpl::onUnitStateChangeEnd() the current unit state is expired.");

	if (currentState->vIsNeedFocusForUnitMap()) {
		if (auto currentlyFocusedUnit = getFocusedUnit()) {
			assert(currentlyFocusedUnit == changedUnit && "UnitMapScriptImpl::onUnitStateChangeEnd() there is already a focused unit when another unit needs to be focused.");
		}
		else {
			m_FocusedUnit = changedUnit;
		}
	}
	else {
		if (auto currentlyFocusedUnit = getFocusedUnit()) {
			assert(currentlyFocusedUnit == changedUnit && "UnitMapScriptImpl::onUnitStateChangeEnd() there is already a focused unit which is not the same as the unit that needs to be unfocused.");
			m_FocusedUnit.reset();
			SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataAllUnitsUnfocused>());
		}
	}
}

void UnitMapScript::UnitMapScriptImpl::onUnitIndexChangeEnd(const EvtDataUnitIndexChangeEnd & e)
{
	auto unitInPreviousIndex = getUnit(e.getPreviousIndex());
	if (unitInPreviousIndex == e.getUnit()) {
		m_UnitMap[e.getPreviousIndex()].reset();
	}

	const auto unit = e.getUnit();
	m_UnitMap[unit->getGridIndex()] = unit;
}

void UnitMapScript::UnitMapScriptImpl::onTurnPhaseChanged(const EvtDataTurnPhaseChanged & e)
{
	const auto currentPhase = e.getCurrentTurnPhase();
	m_CanRespondToTouch = (currentPhase && currentPhase->vGetTypeCode() == TurnPhaseTypeCode::Main);
}

GridIndex UnitMapScript::UnitMapScriptImpl::toGridIndex(const cocos2d::Vec2 & positionInWindow) const
{
	auto positionInMap = m_TransformComponent.lock()->convertToLocalSpace(positionInWindow);
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	return GridIndex(positionInMap, gridSize);
}

std::shared_ptr<UnitScript> UnitMapScript::UnitMapScriptImpl::getUnit(const GridIndex & gridIndex) const
{
	if (!m_UnitMap.isIndexValid(gridIndex))
		return nullptr;

	auto unitScript = m_UnitMap[gridIndex];
	if (unitScript.expired())
		return nullptr;

	return unitScript.lock();
}

std::shared_ptr<UnitScript> UnitMapScript::UnitMapScriptImpl::getFocusedUnit() const
{
	if (m_FocusedUnit.expired())
		return nullptr;

	return m_FocusedUnit.lock();
}

bool UnitMapScript::UnitMapScriptImpl::canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const
{
	if (!m_UnitMap.isIndexValid(gridIndex))
		return false;

	if (unitScript.getGridIndex() == gridIndex)
		return true;

	auto unitAtIndex = getUnit(gridIndex);
	if (!unitAtIndex)
		return true;

	return unitScript.canStayTogether(*unitAtIndex);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of UnitMapScript.
//////////////////////////////////////////////////////////////////////////
UnitMapScript::UnitMapScript() : pimpl{ std::make_shared<UnitMapScriptImpl>() }
{
}

UnitMapScript::~UnitMapScript()
{
}

bool UnitMapScript::onInputTouch(const EvtDataInputTouch & touch)
{
	if (!pimpl->m_CanRespondToTouch) {
		return false;
	}

	auto touchedUnit = pimpl->getUnit(pimpl->toGridIndex(touch.getPositionInWorld()));

	if (auto focusedUnit = pimpl->getFocusedUnit()) {
		return focusedUnit->onInputTouch(touch, touchedUnit);
	}
	else if (touchedUnit) {
		return touchedUnit->onInputTouch(touch, touchedUnit);
	}

	return false;
}

bool UnitMapScript::onInputDrag(const EvtDataInputDrag & drag)
{
	return false;
}

void UnitMapScript::loadUnitMap(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "UnitMapScript::loadUnitMap() failed to load xml file.");

	loadUnitMap(rootElement);
}

void UnitMapScript::loadUnitMap(const tinyxml2::XMLElement * unitMapElement)
{
	assert(unitMapElement && "UnitMapScript::loadUnitMap() the xml element is nullptr.");
	pimpl->m_UnitMap.setDimension(Matrix2DDimension(unitMapElement));

	if (const auto unitsElement = unitMapElement->FirstChildElement("Units")) {
		auto ownerActor = m_OwnerActor.lock();
		for (auto singleUnitElement = unitsElement->FirstChildElement("Unit"); singleUnitElement; singleUnitElement = singleUnitElement->NextSiblingElement("Unit")) {
			ownerActor->addChild(*utilities::XMLToActorUnit(singleUnitElement));
		}
	}
}

Matrix2DDimension UnitMapScript::getMapDimension() const
{
	return pimpl->m_UnitMap.getDimension();
}

bool UnitMapScript::canPassThrough(const UnitScript & unitScript, const GridIndex & gridIndex) const
{
	if (!pimpl->m_UnitMap.isIndexValid(gridIndex))
		return false;

	if (unitScript.getGridIndex() == gridIndex)
		return true;

	auto unitAtIndex = pimpl->getUnit(gridIndex);
	if (!unitAtIndex)
		return true;

	return unitScript.canPassThrough(*unitAtIndex);
}

bool UnitMapScript::canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const
{
	if (!pimpl->m_UnitMap.isIndexValid(gridIndex))
		return false;

	if (unitScript.getGridIndex() == gridIndex)
		return true;

	auto unitAtIndex = pimpl->getUnit(gridIndex);
	if (!unitAtIndex)
		return true;

	return unitScript.canStayTogether(*unitAtIndex);
}

bool UnitMapScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	UnitMapScriptImpl::s_UnitActorPath = relatedActorElement->Attribute("Unit");

	isStaticInitialized = true;
	return true;
}

void UnitMapScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;
	auto ownerActor = m_OwnerActor.lock();

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "UnitMapScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataMakeMovingPathEnd::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onMakeMovingPathEnd(static_cast<const EvtDataMakeMovingPathEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataUnitStateChangeEnd::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onUnitStateChangeEnd(static_cast<const EvtDataUnitStateChangeEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataUnitIndexChangeEnd::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onUnitIndexChangeEnd(static_cast<const EvtDataUnitIndexChangeEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataTurnPhaseChanged::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onTurnPhaseChanged(static_cast<const EvtDataTurnPhaseChanged &>(e));
	});
}

const std::string UnitMapScript::Type = "UnitMapScript";

const std::string & UnitMapScript::getType() const
{
	return Type;
}
