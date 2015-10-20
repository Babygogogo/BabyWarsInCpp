#ifndef __UNIT_SCRIPT__
#define __UNIT_SCRIPT__

#include "../Actor/BaseScriptComponent.h"

class UnitData;

class UnitScript : public BaseScriptComponent
{
public:
	UnitScript();
	~UnitScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Getter/setter for UnitData.
	void setUnitData(std::shared_ptr<UnitData> unitData);
	const std::shared_ptr<UnitData> & getUnitData() const;

	//The setter also sets the position of the Unit according to the indexes.
	void setRowAndColIndex(int rowIndex, int colIndex);
	int getRowIndex() const;
	int getColIndex() const;

	//Disable copy/move constructor and operator=.
	UnitScript(const UnitScript &) = delete;
	UnitScript(UnitScript &&) = delete;
	UnitScript & operator=(const UnitScript &) = delete;
	UnitScript & operator=(UnitScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct UnitScriptImpl;
	std::unique_ptr<UnitScriptImpl> pimpl;
};

#endif // !__UNIT_SCRIPT__
