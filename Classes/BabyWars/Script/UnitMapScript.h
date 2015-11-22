#ifndef __UNIT_MAP_SCRIPT__
#define __UNIT_MAP_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d {
	class Size;
	class Vec2;
}
class UnitScript;
class MovingPath;
struct Matrix2DDimension;
struct GridIndex;

class UnitMapScript : public BaseScriptComponent
{
public:
	UnitMapScript();
	~UnitMapScript();

	//Load a unit map with a xml file. Create unit and unit actors if needed.
	void loadUnitMap(const char * xmlPath);

	void setPosition(const cocos2d::Vec2 & position);

	//Getter of the size of the unit map.
	Matrix2DDimension getMapDimension() const;

	std::shared_ptr<UnitScript> getUnit(const GridIndex & gridIndex) const;
	std::shared_ptr<UnitScript> getActiveUnit() const;

	void deactivateActiveUnit();
	bool isUnitActiveAtIndex(const GridIndex & gridIndex) const;
	bool canActivateUnitAtIndex(const GridIndex & gridIndex) const;

	//Check if a moving unit can pass through or stay at a grid. Only considers the other units on the map, IGNORING the tile map.
	bool canPassThrough(const UnitScript & unitScript, const GridIndex & gridIndex) const;
	bool canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const;

	//We can have no more than one active unit at the same time.
	//Asserts if there is an active unit, or can't activate the unit at the index, or there's no unit at the index.
	bool activateUnitAtIndex(const GridIndex & gridIndex);

	//Deactivate the unit at the fromIndex. Then move it to toIndex if the path is valid.
	//Nothing happens if there's no unit at fromIndex.
	void deactivateAndMoveUnit(UnitScript & unit, const MovingPath & path);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	UnitMapScript(const UnitMapScript &) = delete;
	UnitMapScript(UnitMapScript &&) = delete;
	UnitMapScript & operator=(const UnitMapScript &) = delete;
	UnitMapScript & operator=(UnitMapScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct UnitMapScriptImpl;
	std::unique_ptr<UnitMapScriptImpl> pimpl;
};

#endif // !__UNIT_MAP_SCRIPT__
