#include "BabyWarsActorFactory.h"

#include "../Script/ActionMenuPanelScript.h"
#include "../Script/TileMapScript.h"
#include "../Script/TileScript.h"
#include "../Script/TerrainInfoPanelScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "../Script/WarSceneScript.h"
#include "../Script/WarSceneHUDScript.h"
#include "../Script/WarFieldScript.h"
#include "../Script/MoneyInfoPanelScript.h"
#include "../Script/MovingPathScript.h"
#include "../Script/MovingPathGridScript.h"
#include "../Script/MovingAreaScript.h"
#include "../Script/MovingAreaGridScript.h"

void BabyWarsActorFactory::vRegisterSpecificComponents()
{
	//#TODO: Update the register calls whenever the game-specific actor components are changed.
	registerComponent<ActionMenuPanelScript>();
	registerComponent<MovingPathScript>();
	registerComponent<MovingPathGridScript>();
	registerComponent<MoneyInfoPanelScript>();
	registerComponent<TileMapScript>();
	registerComponent<TileScript>();
	registerComponent<TerrainInfoPanelScript>();
	registerComponent<UnitMapScript>();
	registerComponent<UnitScript>();
	registerComponent<WarSceneScript>();
	registerComponent<WarSceneHUDScript>();
	registerComponent<WarFieldScript>();
	registerComponent<MovingAreaScript>();
	registerComponent<MovingAreaGridScript>();
}
