#ifndef __MOVING_AREA_SCRIPT__
#define __MOVING_AREA_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Vec2;
}
struct GridIndex;
class TileMapScript;
class UnitMapScript;
class UnitScript;
class MovingArea;

class MovingAreaScript : public BaseScriptComponent
{
public:
	MovingAreaScript();
	~MovingAreaScript();

	void setTileMapScript(std::weak_ptr<const TileMapScript> && tileMapScript);
	void setUnitMapScript(std::weak_ptr<const UnitMapScript> && unitMapScript);

	const MovingArea & getUnderlyingArea() const;

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	MovingAreaScript(const MovingAreaScript &) = delete;
	MovingAreaScript(MovingAreaScript &&) = delete;
	MovingAreaScript & operator=(const MovingAreaScript &) = delete;
	MovingAreaScript & operator=(MovingAreaScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovingRangeScriptImpl;
	std::shared_ptr<MovingRangeScriptImpl> pimpl;
};

#endif // !__MOVING_AREA_SCRIPT__
