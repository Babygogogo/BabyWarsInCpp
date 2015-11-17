#ifndef __RESOURCE_LOADER__
#define __RESOURCE_LOADER__

#include <memory>
#include <string>

#include "TileDataID.h"
#include "UnitDataID.h"

//Forward declaration.
namespace cocos2d {
	class Size;
}
class TileData;
class UnitData;

/*!
 * \class ResourceLoader
 *
 * \brief Loads settings and resources of the game.
 *
 * \details
 *	loadGameSettings() and loadResources() should have been called once before you call any getters of this class.
 *
 * \author Babygogogo
 * \date 2015.10
 */
class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();

	void loadGameSettings(const char * xmlPath);
	//Warning: You must call loadGameSettings() before calling loadResource(), otherwise nothing will be loaded.
	void loadResources();

	cocos2d::Size getDesignResolution() const;
	float getFramesPerSecond() const;
	std::string getInitialScenePath() const;

	cocos2d::Size getDesignGridSize() const;

	std::shared_ptr<TileData> getTileData(TileDataID id) const;
	std::shared_ptr<UnitData> getUnitData(UnitDataID id) const;

	//Disable copy/move constructor and operator=.
	ResourceLoader(const ResourceLoader &) = delete;
	ResourceLoader(ResourceLoader &&) = delete;
	ResourceLoader & operator=(const ResourceLoader &) = delete;
	ResourceLoader & operator=(ResourceLoader &&) = delete;

private:
	struct ResourceLoaderImpl;
	std::unique_ptr<ResourceLoaderImpl> pimpl;
};

#endif // !__RESOURCE_LOADER__
