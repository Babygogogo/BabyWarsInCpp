#ifndef __MOVING_AREA_SCRIPT__
#define __MOVING_AREA_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
class TileMapScript;
class UnitMapScript;
class UnitScript;

class MovingAreaScript : public BaseScriptComponent
{
public:
	MovingAreaScript();
	~MovingAreaScript();

	void setTileMapScript(std::weak_ptr<const TileMapScript> && tileMapScript);
	void setUnitMapScript(std::weak_ptr<const UnitMapScript> && unitMapScript);

	bool isAreaShownForUnit(const UnitScript & unit) const;
	void showAreaForUnit(const UnitScript & unit);
	void clearAreaForUnit(const UnitScript & unit);

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
	struct MovingAreaScriptImpl;
	std::shared_ptr<MovingAreaScriptImpl> pimpl;
};

#endif // !__MOVING_AREA_SCRIPT__
