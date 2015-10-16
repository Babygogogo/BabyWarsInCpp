#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "ResourceLoader.h"
#include "TileData.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ResourceLoadImpl.
//////////////////////////////////////////////////////////////////////////
struct ResourceLoader::ResourceLoaderImpl
{
	ResourceLoaderImpl(){};
	~ResourceLoaderImpl(){};

	void loadTextures(const char * listPath);
	void loadTileDatas(const char * xmlPath);

	cocos2d::Size m_TileSize;
	std::unordered_map<TileDataID, std::shared_ptr<TileData>> m_TileDatas;
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

	//Load common settings.
	const auto commonElement = rootElement->FirstChildElement("CommonSettings");
	const auto sizeElement = commonElement->FirstChildElement("Size");
	TileData::setCommonSize(sizeElement->FloatAttribute("Width"), sizeElement->FloatAttribute("Height"));

	//Iterate through the list and load each TileData.
	const auto listElement = rootElement->FirstChildElement("List");
	for (auto tileDataElement = listElement->FirstChildElement("TileDataPath"); tileDataElement; tileDataElement = tileDataElement->NextSiblingElement()){
		auto tileData = std::make_shared<TileData>();
		tileData->initialize(tileDataElement->Attribute("Value"));

		assert(m_TileDatas.find(tileData->getID()) == m_TileDatas.end() && "ResourceLoaderImpl::loadTileDatas() load two TileData with a same ID.");

		m_TileDatas.emplace(tileData->getID(), std::move(tileData));
	}
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

void ResourceLoader::loadResource(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "ResourceLoader::loadResource() failed to load xml file.");

	//Load various resources.
	//Textures and sounds must be loaded firstly because other resources may rely on them.
	pimpl->loadTextures(rootElement->FirstChildElement("TextureListPath")->Attribute("Value"));
	pimpl->loadTileDatas(rootElement->FirstChildElement("TileDataListPath")->Attribute("Value"));
}

const std::shared_ptr<TileData> & ResourceLoader::getTileData(TileDataID id) const
{
	assert(pimpl->m_TileDatas.find(id) != pimpl->m_TileDatas.end() && "ResourceLoader::getTileData() with an invalid ID.");

	return pimpl->m_TileDatas[id];
}
