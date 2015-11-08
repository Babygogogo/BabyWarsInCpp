#ifndef __MOVING_RANGE_SCRIPT__
#define __MOVING_RANGE_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

struct GridIndex;

class MovingRangeScript : public BaseScriptComponent
{
public:
	MovingRangeScript();
	~MovingRangeScript();

	void showRange(const GridIndex & gridIndex);
	void clearRange();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	MovingRangeScript(const MovingRangeScript &) = delete;
	MovingRangeScript(MovingRangeScript &&) = delete;
	MovingRangeScript & operator=(const MovingRangeScript &) = delete;
	MovingRangeScript & operator=(MovingRangeScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovingRangeScriptImpl;
	std::unique_ptr<MovingRangeScriptImpl> pimpl;
};

#endif // !__MOVING_RANGE_SCRIPT__
