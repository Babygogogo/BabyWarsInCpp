#ifndef __UNIT_STATE_TYPE_CODE__
#define __UNIT_STATE_TYPE_CODE__

enum class UnitStateTypeCode
{
	//#NOTE: If you change the type of the unit states, reflect the changes here.
	Invalid,
	Idle,
	Active,
	Moving,
	MovingEnd,
	Waiting
};

#endif // !__UNIT_STATE_TYPE_CODE__
