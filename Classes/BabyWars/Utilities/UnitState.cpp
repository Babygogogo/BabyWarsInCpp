#include "UnitState.h"

void UnitState::onUnitEnterState(UnitScript & unit) const
{
	vShowUnitAppearanceInState(unit);
}

void UnitState::onUnitExitState(UnitScript & unit) const
{
	vClearUnitAppearanceInState(unit);
}
