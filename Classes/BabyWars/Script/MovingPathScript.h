#ifndef __MOVING_PATH_SCRIPT__
#define __MOVING_PATH_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Vec2;
}
struct GridIndex;
class MovingArea;
class MovingPath;

class MovingPathScript : public BaseScriptComponent
{
public:
	MovingPathScript();
	~MovingPathScript();

	void setPosition(const cocos2d::Vec2 & position);

	void showPath(const GridIndex & destination, const MovingArea & area);
	void clearPath();

	bool isBackIndex(const GridIndex & index) const;

	const MovingPath & getUnderlyingPath() const;

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	MovingPathScript(const MovingPathScript &) = delete;
	MovingPathScript(MovingPathScript &&) = delete;
	MovingPathScript & operator=(const MovingPathScript &) = delete;
	MovingPathScript & operator=(MovingPathScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MovingPathScriptImpl;
	std::unique_ptr<MovingPathScriptImpl> pimpl;
};

#endif // !__MOVING_PATH_SCRIPT__
