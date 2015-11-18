#include <vector>

#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/StringToVector.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/UnitDataID.h"
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
	UnitMapScriptImpl() {};
	~UnitMapScriptImpl() {};

	static std::string s_UnitActorPath;

	Matrix2D<std::weak_ptr<UnitScript>> m_UnitMap;
	std::weak_ptr<UnitScript> m_ActiveUnit;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string UnitMapScript::UnitMapScriptImpl::s_UnitActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of UnitMapScript.
//////////////////////////////////////////////////////////////////////////
UnitMapScript::UnitMapScript() : pimpl{ std::make_unique<UnitMapScriptImpl>() }
{
}

UnitMapScript::~UnitMapScript()
{
}

bool UnitMapScript::vInit(tinyxml2::XMLElement *xmlElement)
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
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getBaseRenderComponent();
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
			pimpl->m_UnitMap[gridIndex] = unitScript;
		}

		//Load the next row of the unit map.
		rowElement = rowElement->NextSiblingElement();
	}
}

Matrix2DDimension UnitMapScript::getMapDimension() const
{
	return pimpl->m_UnitMap.getDimension();
}

std::shared_ptr<UnitScript> UnitMapScript::getUnit(const GridIndex & gridIndex) const
{
	if (!pimpl->m_UnitMap.isIndexValid(gridIndex))
		return nullptr;

	auto unitScript = pimpl->m_UnitMap[gridIndex];
	if (unitScript.expired())
		return nullptr;

	return unitScript.lock();
}

std::shared_ptr<UnitScript> UnitMapScript::getActiveUnit() const
{
	if (!pimpl->m_ActiveUnit.expired())
		return pimpl->m_ActiveUnit.lock();

	return nullptr;
}

void UnitMapScript::deactivateActiveUnit()
{
	if (pimpl->m_ActiveUnit.expired())
		return;

	pimpl->m_ActiveUnit.lock()->setActive(false);
	pimpl->m_ActiveUnit.reset();
}

bool UnitMapScript::isUnitActiveAtIndex(const GridIndex & gridIndex) const
{
	if (pimpl->m_ActiveUnit.expired())
		return false;

	auto unitAtIndex = getUnit(gridIndex);
	if (!unitAtIndex)
		return false;

	return unitAtIndex == pimpl->m_ActiveUnit.lock();
}

bool UnitMapScript::canActivateUnitAtIndex(const GridIndex & gridIndex) const
{
	auto unitAtIndex = getUnit(gridIndex);
	if (!unitAtIndex)
		return false;

	return unitAtIndex->canActivate();
}

bool UnitMapScript::canPassThrough(const UnitScript & unitScript, const GridIndex & gridIndex) const
{
	if (!pimpl->m_UnitMap.isIndexValid(gridIndex))
		return false;

	if (unitScript.getGridIndex() == gridIndex)
		return true;

	auto unitAtIndex = getUnit(gridIndex);
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

	auto unitAtIndex = getUnit(gridIndex);
	if (!unitAtIndex)
		return true;

	return unitScript.canStayTogether(*unitAtIndex);
}

bool UnitMapScript::activateUnitAtIndex(const GridIndex & gridIndex)
{
	//Check if there is an currently active unit.
	if (auto currentlyActiveUnit = getActiveUnit()) {
		//If the currently active unit is the same unit as the one that the caller wants to activate, just do nothing and return true.
		if (currentlyActiveUnit->getGridIndex() == gridIndex)
			return true;

		//Otherwise, deactivate the currently active unit.
		deactivateActiveUnit();
	}

	//If there is no unit at the index, return false.
	auto unitAtIndex = getUnit(gridIndex);
	if (!unitAtIndex)
		return false;

	//Activate the unit at the index.
	unitAtIndex->setActive(true);
	pimpl->m_ActiveUnit = unitAtIndex;
	return true;
}

void UnitMapScript::deactivateAndMoveUnit(UnitScript & unit, const MovingPath & path)
{
	//Deactivate the unit.
	unit.setActive(false);
	if (getActiveUnit().get() == &unit)
		pimpl->m_ActiveUnit.reset();

	if (path.getLength() <= 1)
		return;

	//Make the move and update the map.
	const auto startingIndex = path.getFrontNode().m_GridIndex;
	const auto endingIndex = path.getBackNode().m_GridIndex;
	unit.moveAlongPath(path);
	pimpl->m_UnitMap[endingIndex] = pimpl->m_UnitMap[startingIndex];
	pimpl->m_UnitMap[startingIndex].reset();
}

const std::string UnitMapScript::Type = "UnitMapScript";

const std::string & UnitMapScript::getType() const
{
	return Type;
}
