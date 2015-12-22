#include "BabyWarsActorFactory.h"

#include "../Script/BeginTurnEffectScript.h"
#include "../Script/BeginTurnEffectLabelScript.h"
#include "../Script/CommandListItemScript.h"
#include "../Script/CommandListScript.h"
#include "../Script/CommandPanelScript.h"
#include "../Script/PlayerManagerScript.h"
#include "../Script/PlayerScript.h"
#include "../Script/TileMapScript.h"
#include "../Script/TileScript.h"
#include "../Script/TerrainInfoPanelScript.h"
#include "../Script/TurnManagerScript.h"
#include "../Script/TurnAndPlayerLabelScript.h"
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
	registerComponent<BeginTurnEffectLabelScript>();
	registerComponent<BeginTurnEffectScript>();
	registerComponent<CommandListItemScript>();
	registerComponent<CommandListScript>();
	registerComponent<CommandPanelScript>();
	registerComponent<MovingPathScript>();
	registerComponent<MovingPathGridScript>();
	registerComponent<MoneyInfoPanelScript>();
	registerComponent<PlayerManagerScript>();
	registerComponent<PlayerScript>();
	registerComponent<TileMapScript>();
	registerComponent<TileScript>();
	registerComponent<TerrainInfoPanelScript>();
	registerComponent<TurnManagerScript>();
	registerComponent<TurnAndPlayerLabelScript>();
	registerComponent<UnitMapScript>();
	registerComponent<UnitScript>();
	registerComponent<WarSceneScript>();
	registerComponent<WarSceneHUDScript>();
	registerComponent<WarFieldScript>();
	registerComponent<MovingAreaScript>();
	registerComponent<MovingAreaGridScript>();
}
