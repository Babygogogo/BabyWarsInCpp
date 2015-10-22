#ifndef __UNIT_MAP_SCRIPT__
#define __UNIT_MAP_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d{
	class Size;
	class Vec2;
}
class UnitScript;

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
	int getRowCount() const;
	int getColumnCount() const;

	std::shared_ptr<UnitScript> getUnit(const cocos2d::Vec2 & position) const;
	std::shared_ptr<UnitScript> getActiveUnit() const;
	bool isActiveUnitAtPosition(const cocos2d::Vec2 & position) const;
	bool setActiveUnitAtPosition(bool active, const cocos2d::Vec2 & position);
	bool moveAndDeactivateUnit(const cocos2d::Vec2 & fromPos, const cocos2d::Vec2 & toPos);

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
