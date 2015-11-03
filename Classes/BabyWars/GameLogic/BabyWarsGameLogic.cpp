#include "BabyWarsGameLogic.h"
#include "../Actor/BabyWarsActorFactory.h"
#include "../UserInterface/BabyWarsHumanView.h"

std::unique_ptr<BaseActorFactory> BabyWarsGameLogic::vCreateActorFactory() const
{
	auto actorFactory = std::make_unique<BabyWarsActorFactory>();
	actorFactory->init();
	return std::move(actorFactory);
}

void BabyWarsGameLogic::vInitViews()
{
	auto humanView = std::make_shared<BabyWarsHumanView>();
	humanView->init(humanView);
	addView(humanView);
}
