#ifndef __TILE_DATA__
#define __TILE_DATA__

#include <memory>
#include <string>

#include "TileDataID.h"

//Forward declarations.
namespace cocos2d{
	class Animation;
	class Size;
}

class TileData
{
public:
	TileData();
	~TileData();

	//Warning: You must finish loading textures before calling this function.
	void initialize(const char * xmlPath);

	TileDataID getID() const;
	std::string getType() const;
	cocos2d::Animation * getAnimation() const;
	cocos2d::Size getSize() const;

private:
	//Implementation stuff.
	struct TileDataImpl;
	std::unique_ptr<TileDataImpl> pimpl;
};

#endif // !__TILE_DATA__
