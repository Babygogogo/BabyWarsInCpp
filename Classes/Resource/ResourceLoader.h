#ifndef __RESOURCE_LOADER__
#define __RESOURCE_LOADER__

#include <memory>

#include "TileDataID.h"

//Forward declaration.
class TileData;

class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();

	void loadResource(const char * xmlPath);

	const std::shared_ptr<TileData> & getTileData(TileDataID id) const;

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
