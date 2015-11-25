#include "BabyWarsActorFactory.h"

#include "../Script/ActionMenuScript.h"
#include "../Script/TileMapScript.h"
#include "../Script/TileScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "../Script/WarSceneScript.h"
#include "../Script/WarFieldScript.h"
#include "../Script/MovingPathScript.h"
#include "../Script/MovingPathGridScript.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/MovingAreaGridScript.h"

void BabyWarsActorFactory::vRegisterSpecificComponents()
{
	//#TODO: Update the register calls whenever the game-specific actor components are changed.
	registerComponent<ActionMenuScript>();
	registerComponent<MovingPathScript>();
	registerComponent<MovingPathGridScript>();
	registerComponent<TileMapScript>();
	registerComponent<TileScript>();
	registerComponent<UnitMapScript>();
	registerComponent<UnitScript>();
	registerComponent<WarSceneScript>();
	registerComponent<WarFieldScript>();
	registerComponent<MovingAreaScript>();
	registerComponent<MovingAreaGridScript>();
}
