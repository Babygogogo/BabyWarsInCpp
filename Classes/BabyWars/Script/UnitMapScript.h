#ifndef __UNIT_MAP_SCRIPT__
#define __UNIT_MAP_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
class UnitScript;
struct Matrix2DDimension;
struct GridIndex;
class EvtDataInputTouch;
class EvtDataInputDrag;

class UnitMapScript : public BaseScriptComponent
{
public:
	UnitMapScript();
	~UnitMapScript();

	bool onInputTouch(const EvtDataInputTouch & touch);
	bool onInputDrag(const EvtDataInputDrag & drag);

	//Load a unit map with a xml file. Create unit and unit actors if needed.
	void loadUnitMap(const char * xmlPath);

	bool isUnitFocused(const UnitScript & unit) const;
	std::shared_ptr<UnitScript> getFocusedUnit() const;
	void setFocusedUnit(const std::shared_ptr<UnitScript> & focusedUnit);

	void undoMoveAndSetToIdleStateForFocusedUnit();
	void removeFocusedUnitIndexFromMap();

	//Getter of the size of the unit map.
	Matrix2DDimension getMapDimension() const;

	//Check if a moving unit can pass through or stay at a grid. Only considers the other units on the map, IGNORING the tile map.
	bool canPassThrough(const UnitScript & unitScript, const GridIndex & gridIndex) const;
	bool canUnitStayAtIndex(const UnitScript & unitScript, const GridIndex & gridIndex) const;

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
	std::shared_ptr<UnitMapScriptImpl> pimpl;
};

#endif // !__UNIT_MAP_SCRIPT__
