#ifndef __UNIT_STATE_FACTORY__
#define __UNIT_STATE_FACTORY__

#include <memory>

//Forward declaration.
enum class UnitStateTypeCode;
class UnitState;

namespace utilities
{
	std::shared_ptr<UnitState> createUnitState(UnitStateTypeCode stateCode);
}

#endif // !__UNIT_STATE_FACTORY__
