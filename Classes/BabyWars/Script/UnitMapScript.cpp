#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/StringToVector.h"
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Event/EvtDataUnitIndexChangeEnd.h"
#include "../Event/EvtDataMakeMovingPathEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2D.h"
#include "../Utilities/MovingPath.h"
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

	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	std::shared_ptr<UnitScript> getUnit(const GridIndex & gridIndex) const;
	std::shared_ptr<UnitScript> getFocusedUnit() const;
	bool canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const;

	static std::string s_UnitActorPath;

	Matrix2D<std::weak_ptr<UnitScript>> m_UnitMap;
	std::weak_ptr<UnitScript> m_FocusedUnit;

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
	auto previousState = e.getPreviousState();
	auto currentState = e.getCurrentState();
	if (previousState == currentState) {
		return;
	}

	using State = UnitState::State;
	if (currentState.getState() == State::Active) {
		if (auto focusUnit = getFocusedUnit()) {
			assert(focusUnit != e.getUnitScript() && "UnitMapScriptImpl::onUnitStateChangeEnd() the unit which is changed to be active is the focus unit.");
			focusUnit->undoMove();
		}

		m_FocusedUnit = e.getUnitScript();
	}
	else if (currentState.getState() == State::Idle) {
		if (getFocusedUnit() == e.getUnitScript()) {
			m_FocusedUnit.reset();
		}
	}
	else if (currentState.getState() == State::Moving) {
		auto focusUnit = getFocusedUnit();
		assert(focusUnit == e.getUnitScript() && "UnitMapScriptImpl::onUnitStateChangeEnd() the moving unit is not the focus unit.");

		m_UnitMap[focusUnit->getGridIndex()].reset();
	}
	else if (currentState.getState() == State::MovingEnd) {
	}
	else if (currentState.getState() == State::Waiting) {
		if (getFocusedUnit() == e.getUnitScript()) {
			m_FocusedUnit.reset();
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
	if (auto touchUnit = pimpl->getUnit(pimpl->toGridIndex(touch.getPositionInWorld()))) {
		return touchUnit->onInputTouch(touch);
	}
	else {
		if (auto focusedUnit = pimpl->getFocusedUnit()) {
			focusedUnit->undoMove();
			return true;
		}
	}

	return false;
}

bool UnitMapScript::onInputDrag(const EvtDataInputDrag & drag)
{
	return false;
}

void UnitMapScript::loadUnitMap(const char * xmlPath)
{
	//////////////////////////////////////////////////////////////////////////
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "UnitMapScript::loadUnitMap() failed to load xml file.");

	//Load the unit map size.
	auto dimension = Matrix2DDimension(rootElement->IntAttribute("Height"), rootElement->IntAttribute("Width"));
	pimpl->m_UnitMap.setDimension(dimension);

	//////////////////////////////////////////////////////////////////////////
	//Load the map.
	//Some variables to make the job easier.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto ownerActor = m_OwnerActor.lock();
	auto selfSceneNode = ownerActor->getRenderComponent()->getSceneNode();
	const auto unitsElement = rootElement->FirstChildElement("Unit");
	const auto mapElement = rootElement->FirstChildElement("Map");

	//Start loading the unit indexes of the unit map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = mapElement->FirstChildElement("Row");
	for (auto rowIndex = dimension.rowCount - 1; rowIndex < dimension.rowCount; --rowIndex) {
		assert(rowElement && "UnitMapScript::loadUnitMap() the rows count is less than the height of the UnitMap.");

		//Load the unit indexes of the row.
		auto rowIndexes = utilities::toVector<std::string>(rowElement->Attribute("UnitIndexes"));
		assert(rowIndexes.size() == dimension.colCount && "UnitMapScript::loadUnitMap() the columns count is less than the width of the UnitMap.");

		//For each ID in the row, create an unit actor add the scripts into the unit map.
		for (auto colIndex = rowIndexes.size() * 0; colIndex < rowIndexes.size(); ++colIndex) {
			auto unitElement = unitsElement->FirstChildElement((std::string("Index") + rowIndexes[colIndex]).c_str());
			if (!unitElement)
				continue;

			//Create a new unit actor and initialize it with the id and indexes.
			auto unitActor = gameLogic->createActor(UnitMapScriptImpl::s_UnitActorPath.c_str());
			auto unitScript = unitActor->getComponent<UnitScript>();
			unitScript->loadUnit(unitElement);
			auto gridIndex = GridIndex(rowIndex, colIndex);
			unitScript->setGridIndexAndPosition(gridIndex);

			//Add the unit actor and script to UnitMap.
			ownerActor->addChild(*unitActor);
			selfSceneNode->addChild(unitActor->getRenderComponent()->getSceneNode());
		}

		//Load the next row of the unit map.
		rowElement = rowElement->NextSiblingElement();
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
	auto ownerActor = m_OwnerActor.lock();

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "UnitMapScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataMakeMovingPathEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onMakeMovingPathEnd(static_cast<const EvtDataMakeMovingPathEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataUnitStateChangeEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onUnitStateChangeEnd(static_cast<const EvtDataUnitStateChangeEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataUnitIndexChangeEnd::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onUnitIndexChangeEnd(static_cast<const EvtDataUnitIndexChangeEnd &>(e));
	});
}

const std::string UnitMapScript::Type = "UnitMapScript";

const std::string & UnitMapScript::getType() const
{
	return Type;
}
