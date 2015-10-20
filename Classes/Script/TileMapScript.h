#ifndef __TILE_MAP_SCRIPT__
#define __TILE_MAP_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d{
	class Size;
}

class TileMapScript : public BaseScriptComponent
{
public:
	TileMapScript();
	~TileMapScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Load a tile map with a xml file. Create tile and unit actors if needed.
	void loadTileMap(const char * xmlPath);

	cocos2d::Size getUntransformedMapSize() const;

	int getRowCount() const;
	int getColumnCount() const;

	//Disable copy/move constructor and operator=.
	TileMapScript(const TileMapScript &) = delete;
	TileMapScript(TileMapScript &&) = delete;
	TileMapScript & operator=(const TileMapScript &) = delete;
	TileMapScript & operator=(TileMapScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TileMapScriptImpl;
	std::unique_ptr<TileMapScriptImpl> pimpl;
};

#endif // !__TILE_MAP_SCRIPT__
