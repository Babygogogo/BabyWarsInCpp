#ifndef __MOVE_PATH_SCRIPT__
#define __MOVE_PATH_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

struct GridIndex;
class UnitScript;
class UnitMapScript;
class TileMapScript;

class MovePathScript : public BaseScriptComponent
{
public:
	MovePathScript();
	~MovePathScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	void updatePath(const GridIndex & destination, std::shared_ptr<UnitScript> movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap);

	//Disable copy/move constructor and operator=.
	MovePathScript(const MovePathScript &) = delete;
	MovePathScript(MovePathScript &&) = delete;
	MovePathScript & operator=(const MovePathScript &) = delete;
	MovePathScript & operator=(MovePathScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovePathScriptImpl;
	std::unique_ptr<MovePathScriptImpl> pimpl;
};

#endif // !__MOVE_PATH_SCRIPT__
