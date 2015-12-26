#ifndef __UNIT_ANIMATION__
#define __UNIT_ANIMATION__

#include <memory>

#include "UnitDataID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
namespace cocos2d
{
	class Animation;
}
enum class UnitStateTypeCode;
enum class ColorTypeCode;

class UnitAnimation
{
public:
	UnitAnimation();
	~UnitAnimation();

	void loadAnimation(const tinyxml2::XMLElement * xmlElement);
	cocos2d::Animation * getAnimation(UnitStateTypeCode unitStateTypeCode, ColorTypeCode colorTypeCode) const;

private:
	struct UnitAnimationImpl;
	std::unique_ptr<UnitAnimationImpl> pimpl;
};

#endif // !__UNIT_ANIMATION__
