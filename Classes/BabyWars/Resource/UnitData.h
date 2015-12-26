#ifndef __UNIT_DATA__
#define __UNIT_DATA__

#include <memory>
#include <string>

#include "UnitDataID.h"

//Forward declarations.
namespace cocos2d {
	class Animation;
}

class UnitData
{
public:
	UnitData();
	~UnitData();

	//Warning: You must finish loading textures before calling this function.
	void initialize(const char * xmlPath);

	UnitDataID getID() const;
	std::string getType() const;

	int getMovementRange() const;
	const std::string & getMovementType() const;

	cocos2d::Animation * getAnimation() const;
	float getAnimationMovingSpeedGridPerSec() const;

private:
	//Implementation stuff.
	struct UnitDataImpl;
	std::unique_ptr<UnitDataImpl> pimpl;
};

#endif // !__UNIT_DATA__
