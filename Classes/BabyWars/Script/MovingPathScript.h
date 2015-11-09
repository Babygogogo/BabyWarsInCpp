#ifndef __MOVING_PATH_SCRIPT__
#define __MOVING_PATH_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

struct GridIndex;
class UnitScript;
class UnitMapScript;
class TileMapScript;
class MovingArea;

class MovingPathScript : public BaseScriptComponent
{
public:
	MovingPathScript();
	~MovingPathScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	void showPath(const GridIndex & destination, const MovingArea & area);
	void showPath(const GridIndex & destination, std::shared_ptr<UnitScript> movingUnit, const TileMapScript & tileMap, const UnitMapScript & unitMap);

	//Disable copy/move constructor and operator=.
	MovingPathScript(const MovingPathScript &) = delete;
	MovingPathScript(MovingPathScript &&) = delete;
	MovingPathScript & operator=(const MovingPathScript &) = delete;
	MovingPathScript & operator=(MovingPathScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovePathScriptImpl;
	std::unique_ptr<MovePathScriptImpl> pimpl;
};

#endif // !__MOVING_PATH_SCRIPT__
