#ifndef __BABY_WARS_ACTOR_FACTORY__
#define __BABY_WARS_ACTOR_FACTORY__

#include "../../BabyEngine/Actor/BaseActorFactory.h"

class BabyWarsActorFactory : public BaseActorFactory
{
public:
	BabyWarsActorFactory();
	~BabyWarsActorFactory();

	//Disable copy/move constructor and operator=.
	BabyWarsActorFactory(const BabyWarsActorFactory &) = delete;
	BabyWarsActorFactory(BabyWarsActorFactory &&) = delete;
	BabyWarsActorFactory & operator=(const BabyWarsActorFactory &) = delete;
	BabyWarsActorFactory & operator=(BabyWarsActorFactory &&) = delete;
};

#endif // !__BABY_WARS_ACTOR_FACTORY__
