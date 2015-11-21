#ifndef __TILE_SCRIPT__
#define __TILE_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
class TileData;
struct GridIndex;

class TileScript : public BaseScriptComponent
{
public:
	TileScript();
	~TileScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	void LoadTile(tinyxml2::XMLElement * xmlElement);

	const std::shared_ptr<TileData> & getTileData() const;

	//The setter also sets the position of the tile according to the indexes.
	void setGridIndexAndPosition(const GridIndex & gridIndex);
	GridIndex getGridIndex() const;

	//Disable copy/move constructor and operator=.
	TileScript(const TileScript &) = delete;
	TileScript(TileScript &&) = delete;
	TileScript & operator=(const TileScript &) = delete;
	TileScript & operator=(TileScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TileScriptImpl;
	std::unique_ptr<TileScriptImpl> pimpl;
};

#endif // !__TILE_SCRIPT__
