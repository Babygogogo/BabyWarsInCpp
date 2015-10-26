#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "UnitMapScript.h"
#include "UnitScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../GameLogic/GameLogic.h"
#include "../Resource/UnitDataID.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/StringToVector.h"
#include "../Utilities/SingletonContainer.h"
#include "../Utilities/GridIndex.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitMapScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitMapScript::UnitMapScriptImpl
{
	UnitMapScriptImpl(){};
	~UnitMapScriptImpl(){};

	bool isGridIndexValid(const GridIndex & index) const;
	bool isRowIndexValid(int rowIndex) const;
	bool isColIndexValid(int colIndex) const;

	static std::string s_UnitActorPath;

	int m_RowCount{}, m_ColCount{};
	std::vector<std::vector<std::weak_ptr<UnitScript>>> m_UnitMap;
	std::weak_ptr<UnitScript> m_ActiveUnit;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string UnitMapScript::UnitMapScriptImpl::s_UnitActorPath;

bool UnitMapScript::UnitMapScriptImpl::isGridIndexValid(const GridIndex & index) const
{
	return isRowIndexValid(index.rowIndex) && isColIndexValid(index.colIndex);
}

bool UnitMapScript::UnitMapScriptImpl::isRowIndexValid(int rowIndex) const
{
	return rowIndex >= 0 && rowIndex < m_RowCount;
}

bool UnitMapScript::UnitMapScriptImpl::isColIndexValid(int colIndex) const
{
	return colIndex >= 0 && colIndex < m_ColCount;
}

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
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
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
	pimpl->m_RowCount = rootElement->IntAttribute("Height");
	pimpl->m_ColCount = rootElement->IntAttribute("Width");
	pimpl->m_UnitMap.resize(pimpl->m_RowCount);

	//////////////////////////////////////////////////////////////////////////
	//Load the map.
	//Some variables to make the job easier.
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	auto ownerActor = m_OwnerActor.lock();
	const auto unitsElement = rootElement->FirstChildElement("Unit");
	const auto mapElement = rootElement->FirstChildElement("Map");

	//Start loading the unit indexes of the unit map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = mapElement->FirstChildElement("Row");
	for (auto rowIndex = pimpl->m_RowCount - 1; rowIndex >= 0; --rowIndex){
		assert(rowElement && "UnitMapScript::loadUnitMap() the rows count is less than the height of the UnitMap.");

		//Load the unit indexes of the row.
		auto rowIndexes = utilities::toVector<std::string>(rowElement->Attribute("UnitIndexes"));
		assert(rowIndexes.size() == pimpl->m_ColCount && "UnitMapScript::loadUnitMap() the columns count is less than the width of the UnitMap.");
		pimpl->m_UnitMap[rowIndex].clear();

		//For each ID in the row, create an unit actor add the scripts into the unit map.
		for (auto colIndex = rowIndexes.size() * 0; colIndex < rowIndexes.size(); ++colIndex){
			//There is no unit in [rowIndex, colIndex] if the unit index is 0. Emplace a nullptr in the map and continue.
			if (std::stoi(rowIndexes[colIndex]) == 0){
				pimpl->m_UnitMap[rowIndex].emplace_back();
				continue;
			}

			//Create a new unit actor and initialize it with the id and indexes.
			auto unitActor = gameLogic->createActor(UnitMapScriptImpl::s_UnitActorPath.c_str());
			auto unitScript = unitActor->getComponent<UnitScript>();
			unitScript->loadUnit(unitsElement->FirstChildElement((std::string("Index") + rowIndexes[colIndex]).c_str()));
			unitScript->setGridIndexAndPosition(GridIndex(rowIndex, colIndex));

			//Add the unit actor and script to UnitMap.
			ownerActor->addChild(*unitActor);
			pimpl->m_UnitMap[rowIndex].emplace_back(std::move(unitScript));
		}

		//Load the next row of the unit map.
		rowElement = rowElement->NextSiblingElement();
	}
}

int UnitMapScript::getRowCount() const
{
	return pimpl->m_RowCount;
}

int UnitMapScript::getColumnCount() const
{
	return pimpl->m_ColCount;
}

std::shared_ptr<UnitScript> UnitMapScript::getUnit(const cocos2d::Vec2 & position) const
{
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto gridIndex = GridIndex(position, gridSize);
	//cocos2d::log("UnitMapScript::getUnit() rowIndex: %d colIndex: %d", rowIndex, colIndex);
	if (!pimpl->isGridIndexValid(gridIndex))
		return nullptr;

	auto unitScript = pimpl->m_UnitMap[gridIndex.rowIndex][gridIndex.colIndex];
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

bool UnitMapScript::isActiveUnitAtPosition(const cocos2d::Vec2 & position) const
{
	if (pimpl->m_ActiveUnit.expired())
		return false;

	auto unitAtPosition = getUnit(position);
	if (!unitAtPosition)
		return false;

	return unitAtPosition == pimpl->m_ActiveUnit.lock();
}

bool UnitMapScript::setActiveUnitAtPosition(bool active, const cocos2d::Vec2 & position)
{
	if (!pimpl->m_ActiveUnit.expired()){
		pimpl->m_ActiveUnit.lock()->setActive(false);
		pimpl->m_ActiveUnit.reset();
	}

	auto targetUnit = getUnit(position);
	if (!targetUnit)
		return false;

	targetUnit->setActive(active);
	if (active)
		pimpl->m_ActiveUnit = targetUnit;
	else
		pimpl->m_ActiveUnit.reset();

	return true;
}

bool UnitMapScript::moveAndDeactivateUnit(const cocos2d::Vec2 & fromPos, const cocos2d::Vec2 & toPos)
{
	//#TODO: This function should take something like MovePath as parameter and do the job.
	auto targetUnit = getUnit(fromPos);
	if (!targetUnit)
		return false;

	targetUnit->setActive(false);
	pimpl->m_ActiveUnit.reset();

	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	auto fromIndex = GridIndex(fromPos, gridSize);
	auto toIndex = GridIndex(toPos, gridSize);

	if (fromIndex == toIndex || !pimpl->isGridIndexValid(fromIndex) || !pimpl->isGridIndexValid(toIndex))
		return false;

	if (auto existingUnit = getUnit(toPos))
		return false;

	targetUnit->moveTo(toIndex);
	pimpl->m_UnitMap[fromIndex.rowIndex][fromIndex.colIndex].reset();
	pimpl->m_UnitMap[toIndex.rowIndex][toIndex.colIndex] = std::move(targetUnit);

	return true;
}

const std::string UnitMapScript::Type = "UnitMapScript";

const std::string & UnitMapScript::getType() const
{
	return Type;
}
