#ifndef __UNIT_MAP_SCRIPT__
#define __UNIT_MAP_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d{
	class Size;
	class Vec2;
}
class UnitScript;
struct Matrix2DDimension;
struct GridIndex;

class UnitMapScript : public BaseScriptComponent
{
public:
	UnitMapScript();
	~UnitMapScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Load a unit map with a xml file. Create unit and unit actors if needed.
	void loadUnitMap(const char * xmlPath);

	//Getter of the size of the unit map.
	Matrix2DDimension getMapDimension() const;

	std::shared_ptr<UnitScript> getUnit(const GridIndex & gridIndex) const;
	std::shared_ptr<UnitScript> getActiveUnit() const;

	void deactivateActiveUnit();
	bool isUnitActiveAtIndex(const GridIndex & gridIndex) const;

	//This function deactivates the currently active unit, even if there is no unit at the grid index.
	//Return value indicates that if the unit at the index is successfully activated (if there's no unit, false is returned).
	bool activateUnitAtIndex(const GridIndex & gridIndex);

	//Deactivate the unit at the fromIndex. Then move it to toIndex if the path is valid.
	//Nothing happens if there's no unit at fromIndex.
	void deactivateAndMoveUnit(const GridIndex & fromIndex, const GridIndex & toIndex);

	//Disable copy/move constructor and operator=.
	UnitMapScript(const UnitMapScript &) = delete;
	UnitMapScript(UnitMapScript &&) = delete;
	UnitMapScript & operator=(const UnitMapScript &) = delete;
	UnitMapScript & operator=(UnitMapScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct UnitMapScriptImpl;
	std::unique_ptr<UnitMapScriptImpl> pimpl;
};

#endif // !__UNIT_MAP_SCRIPT__
