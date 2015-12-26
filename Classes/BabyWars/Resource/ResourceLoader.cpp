#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "ResourceLoader.h"
#include "TileData.h"
#include "UnitData.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ResourceLoadImpl.
//////////////////////////////////////////////////////////////////////////
struct ResourceLoader::ResourceLoaderImpl
{
	ResourceLoaderImpl() = default;
	~ResourceLoaderImpl() = default;

	void loadTextures(const char * listPath);
	void loadTileDatas(const char * xmlPath);
	void loadUnitDatas(const char * xmlPath);

	std::shared_ptr<UnitData> createUnitDataFromXML(const char * xmlPath) const;

	cocos2d::Size m_DesignResolution;
	cocos2d::Size m_DesignGridSize;
	float m_FramesPerSecond{};
	std::string m_ResourceListPath;
	std::string m_InitialScenePath;

	cocos2d::Size m_TileSize;
	std::unordered_map<TileDataID, std::shared_ptr<TileData>> m_TileDatas;
	std::unordered_map<UnitDataType, std::shared_ptr<UnitData>> m_UnitDatas;
};

void ResourceLoader::ResourceLoaderImpl::loadTextures(const char * listPath)
{
	cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(listPath);
}

void ResourceLoader::ResourceLoaderImpl::loadTileDatas(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoaderImpl::loadTileDatas() failed to load xml file.");

	//Iterate through the list and load each TileData.
	const auto listElement = rootElement->FirstChildElement("List");
	for (auto tileDataElement = listElement->FirstChildElement("TileDataPath"); tileDataElement; tileDataElement = tileDataElement->NextSiblingElement()) {
		auto tileData = std::make_shared<TileData>();
		tileData->initialize(tileDataElement->Attribute("Value"));

		assert(m_TileDatas.find(tileData->getID()) == m_TileDatas.end() && "ResourceLoaderImpl::loadTileDatas() load two TileData with a same ID.");

		m_TileDatas.emplace(tileData->getID(), std::move(tileData));
	}
}

void ResourceLoader::ResourceLoaderImpl::loadUnitDatas(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoaderImpl::loadUnitDatas() failed to load xml file.");

	//Iterate through the list and load each UnitData.
	const auto listElement = rootElement->FirstChildElement("List");
	for (auto tileDataElement = listElement->FirstChildElement("UnitDataPath"); tileDataElement; tileDataElement = tileDataElement->NextSiblingElement()) {
		auto unitData = createUnitDataFromXML(tileDataElement->Attribute("Value"));
		assert(m_UnitDatas.find(unitData->getType()) == m_UnitDatas.end() && "ResourceLoaderImpl::loadUnitDatas() load two UnitData with a same ID.");

		m_UnitDatas.emplace(unitData->getType(), std::move(unitData));
	}
}

std::shared_ptr<UnitData> ResourceLoader::ResourceLoaderImpl::createUnitDataFromXML(const char * xmlPath) const
{
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoaderImpl::createUnitDataFromXML() failed to load xml file.");

	auto unitData = std::make_shared<UnitData>();
	unitData->init(rootElement);
	return unitData;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ResourceLoader.
//////////////////////////////////////////////////////////////////////////
ResourceLoader::ResourceLoader() : pimpl{ std::make_unique<ResourceLoaderImpl>() }
{
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::loadGameSettings(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoader::loadGameSettings() failed to load xml file.");

	//Load the design resolution.
	auto resolutionElement = rootElement->FirstChildElement("DesignResolution");
	pimpl->m_DesignResolution.width = resolutionElement->FloatAttribute("Width");
	pimpl->m_DesignResolution.height = resolutionElement->FloatAttribute("Height");

	//Load the grid size.
	auto gridSizeElement = rootElement->FirstChildElement("GridSize");
	pimpl->m_DesignGridSize.width = gridSizeElement->FloatAttribute("Width");
	pimpl->m_DesignGridSize.height = gridSizeElement->FloatAttribute("Height");

	//Load other settings.
	pimpl->m_FramesPerSecond = rootElement->FirstChildElement("FramesPerSecond")->FloatAttribute("Value");
	pimpl->m_ResourceListPath = rootElement->FirstChildElement("ResourceListPath")->Attribute("Value");
	pimpl->m_InitialScenePath = rootElement->FirstChildElement("InitialScenePath")->Attribute("Value");
}

void ResourceLoader::loadResources()
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(pimpl->m_ResourceListPath.c_str());
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoader::loadResource() failed to load xml file.");

	//Load various resources.
	//Textures and sounds must be loaded firstly because other resources may rely on them.
	pimpl->loadTextures(rootElement->FirstChildElement("TextureListPath")->Attribute("Value"));
	pimpl->loadTileDatas(rootElement->FirstChildElement("TileDataListPath")->Attribute("Value"));
	pimpl->loadUnitDatas(rootElement->FirstChildElement("UnitDataListPath")->Attribute("Value"));
}

cocos2d::Size ResourceLoader::getDesignResolution() const
{
	return pimpl->m_DesignResolution;
}

float ResourceLoader::getFramesPerSecond() const
{
	return pimpl->m_FramesPerSecond;
}

std::string ResourceLoader::getInitialScenePath() const
{
	return pimpl->m_InitialScenePath;
}

cocos2d::Size ResourceLoader::getDesignGridSize() const
{
	return pimpl->m_DesignGridSize;
}

std::shared_ptr<TileData> ResourceLoader::getTileData(TileDataID id) const
{
	assert(pimpl->m_TileDatas.find(id) != pimpl->m_TileDatas.end() && "ResourceLoader::getTileData() with an invalid ID.");

	return pimpl->m_TileDatas[id];
}

std::shared_ptr<UnitData> ResourceLoader::getUnitData(UnitDataType id) const
{
	assert(pimpl->m_UnitDatas.find(id) != pimpl->m_UnitDatas.end() && "ResourceLoader::getUnitData() with an invalid ID.");

	return pimpl->m_UnitDatas[id];
}
