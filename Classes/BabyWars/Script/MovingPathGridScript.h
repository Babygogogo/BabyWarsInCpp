#ifndef __MOVING_PATH_GRID_SCRIPT__
#define __MOVING_PATH_GRID_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

struct GridIndex;
enum class AdjacentDirection;

class MovingPathGridScript : public BaseScriptComponent
{
public:
	MovingPathGridScript();
	~MovingPathGridScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	void setAppearanceWithPreviousAndNextDirection(AdjacentDirection previous, AdjacentDirection next);
	void setPositionWithGridIndex(const GridIndex & index);

	//Disable copy/move ctor and operator=.
	MovingPathGridScript(const MovingPathGridScript &) = delete;
	MovingPathGridScript(MovingPathGridScript &&) = delete;
	MovingPathGridScript & operator=(const MovingPathGridScript &) = delete;
	MovingPathGridScript & operator=(MovingPathGridScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovingPathGridScriptImpl;
	std::unique_ptr<MovingPathGridScriptImpl> pimpl;
};

#endif // __MOVING_PATH_GRID_SCRIPT__
