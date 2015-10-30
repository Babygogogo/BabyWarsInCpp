#include "BabyWarsActorFactory.h"

#include "../Script/TileMapScript.h"
#include "../Script/TileScript.h"
#include "../Script/UnitMapScript.h"
#include "../Script/UnitScript.h"
#include "../Script/WarSceneScript.h"
#include "../Script/MovePathScript.h"

BabyWarsActorFactory::BabyWarsActorFactory()
{
	//#TODO: Update the register calls whenever the game-specific actor component are changed.
	registerComponent<MovePathScript>();
	registerComponent<TileMapScript>();
	registerComponent<TileScript>();
	registerComponent<UnitMapScript>();
	registerComponent<UnitScript>();
	registerComponent<WarSceneScript>();
}

BabyWarsActorFactory::~BabyWarsActorFactory()
{
}
