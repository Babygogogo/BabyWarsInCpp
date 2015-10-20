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

//////////////////////////////////////////////////////////////////////////
//Definition of WorldScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitMapScript::UnitMapScriptImpl
{
	UnitMapScriptImpl(){};
	~UnitMapScriptImpl(){};

	static std::string s_UnitActorPath;

	int m_RowCount{}, m_ColCount{};
	std::vector<std::vector<std::weak_ptr<UnitScript>>> m_UnitMap;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string UnitMapScript::UnitMapScriptImpl::s_UnitActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
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
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "UnitMapScript::loadUnitMap() failed to load xml file.");

	//Load the unit map size.
	pimpl->m_RowCount = rootElement->IntAttribute("Height");
	pimpl->m_ColCount = rootElement->IntAttribute("Width");
	pimpl->m_UnitMap.resize(pimpl->m_RowCount);

	//Some object to ease the following steps.
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	auto ownerActor = m_OwnerActor.lock();

	//Load the unit data ids of the unit map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = rootElement->FirstChildElement("Row");
	for (auto rowIndex = pimpl->m_RowCount - 1; rowIndex >= 0; --rowIndex){
		assert(rowElement && "WorldScript::loadWorld() the rows count is less than the height of the world.");

		//Load the UnitDataIDs of the row.
		auto rowIDs = utilities::toVector<UnitDataID>(rowElement->Attribute("UnitDataIDs"));
		assert(rowIDs.size() == pimpl->m_ColCount && "WorldScript::loadWorld() the columns count is less than the width of the world.");

		//For each ID in the row, create an unit actor add the scripts into the unit map.
		for (auto colIndex = rowIDs.size() * 0; colIndex < rowIDs.size(); ++colIndex){
			//There is no unit in [rowIndex, colIndex] if the id is 0. Do nothing and continue.
			if (rowIDs[colIndex] == 0)
				continue;

			//Create a new unit actor and initialize it with the id and indexes.
			auto unitActor = gameLogic->createActor(UnitMapScriptImpl::s_UnitActorPath.c_str());
			auto unitScript = unitActor->getComponent<UnitScript>();
			unitScript->setUnitData(resourceLoader->getUnitData(rowIDs[colIndex]));
			unitScript->setRowAndColIndex(rowIndex, colIndex);

			//Add the unit actor and script to world.
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

const std::string UnitMapScript::Type = "UnitMapScript";

const std::string & UnitMapScript::getType() const
{
	return Type;
}
