#ifndef __MOVING_RANGE_GRID_SCRIPT__
#define __MOVING_RANGE_GRID_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

struct GridIndex;

class MovingRangeGridScript : public BaseScriptComponent
{
public:
	MovingRangeGridScript();
	~MovingRangeGridScript();

	void setGridIndexAndPosition(const GridIndex & index);
	void setVisible(bool visible);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	MovingRangeGridScript(const MovingRangeGridScript &) = delete;
	MovingRangeGridScript(MovingRangeGridScript &&) = delete;
	MovingRangeGridScript & operator=(const MovingRangeGridScript &) = delete;
	MovingRangeGridScript & operator=(MovingRangeGridScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovingRangeGridScriptImpl;
	std::unique_ptr<MovingRangeGridScriptImpl> pimpl;
};

#endif // !__MOVING_RANGE_GRID_SCRIPT__
