#ifndef __UNIT_DATA__
#define __UNIT_DATA__

#include <memory>
#include <string>

#include "UnitDataType.h"

//Forward declarations.
namespace cocos2d
{
	class Animation;
}
namespace tinyxml2
{
	class XMLElement;
}

class UnitData
{
public:
	UnitData();
	~UnitData();

	//Warning: You must have loaded textures before calling this function.
	void init(const tinyxml2::XMLElement * xmlElement);

	const UnitDataType & getType() const;

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
