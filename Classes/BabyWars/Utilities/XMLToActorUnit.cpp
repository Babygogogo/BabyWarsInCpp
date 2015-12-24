#include <cassert>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "XMLToActorUnit.h"
#include "../Resource/ResourceLoader.h"
#include "../Script/UnitScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"

std::shared_ptr<Actor> utilities::XMLToActorUnit(const tinyxml2::XMLElement * unitElement)
{
	assert(unitElement && "utilities::XMLToActorUnit() the unit actor resource is nullptr.");

	const auto unitActorPath = unitElement->Attribute("ActorPath");
	auto unitActor = SingletonContainer::getInstance()->get<BaseGameLogic>()->createActorAndChildren(unitActorPath);
	assert(unitActor && "utilities::XMLToActorUnit() can't create a unit actor according to the xml element.");

	auto unitScript = unitActor->getComponent<UnitScript>();
	assert(unitScript && "utilities::XMLToActorUnit() the created unit actor has no unit script.");
	unitScript->loadUnit(unitElement);

	return unitActor;
}
