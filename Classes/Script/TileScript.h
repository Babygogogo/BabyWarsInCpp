#ifndef __TILE_SCRIPT__
#define __TILE_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
class TileData;

class TileScript : public BaseScriptComponent
{
public:
	TileScript();
	~TileScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Getter/setter for TileData.
	void setTileData(std::shared_ptr<TileData> tileData);
	const std::shared_ptr<TileData> & getTileData() const;

	//The setter also sets the position of the tile according to the indexes.
	void setRowAndColIndex(int rowIndex, int colIndex);
	int getRowIndex() const;
	int getColIndex() const;

	//Disable copy/move constructor and operator=.
	TileScript(const TileScript &) = delete;
	TileScript(TileScript &&) = delete;
	TileScript & operator=(const TileScript &) = delete;
	TileScript & operator=(TileScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TileScriptImpl;
	std::unique_ptr<TileScriptImpl> pimpl;
};

#endif // !__TILE_SCRIPT__
