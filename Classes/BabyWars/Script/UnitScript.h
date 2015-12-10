#ifndef __UNIT_SCRIPT__
#define __UNIT_SCRIPT__

#include <memory>
#include <vector>

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
class UnitData;
struct GridIndex;
class MovingPath;
class EvtDataInputTouch;
class GameCommand;
class UnitState;

class UnitScript : public BaseScriptComponent
{
public:
	UnitScript();
	~UnitScript();

	bool onInputTouch(const EvtDataInputTouch & touch);

	void loadUnit(tinyxml2::XMLElement * xmlElement);

	const std::shared_ptr<UnitData> & getUnitData() const;

	std::vector<GameCommand> getCommands() const;

	//The setter also sets the position of the Unit according to the indexes.
	void setGridIndexAndPosition(const GridIndex & gridIndex);
	GridIndex getGridIndex() const;

	bool canSetState(UnitState state) const;
	UnitState getState() const;
	void setState(UnitState state);

	bool canPassThrough(const UnitScript & otherUnit) const;
	bool canStayTogether(const UnitScript & otherUnit) const;

	void moveAlongPath(const MovingPath & path);
	void undoMove();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	UnitScript(const UnitScript &) = delete;
	UnitScript(UnitScript &&) = delete;
	UnitScript & operator=(const UnitScript &) = delete;
	UnitScript & operator=(UnitScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct UnitScriptImpl;
	std::shared_ptr<UnitScriptImpl> pimpl;
};

#endif // !__UNIT_SCRIPT__
