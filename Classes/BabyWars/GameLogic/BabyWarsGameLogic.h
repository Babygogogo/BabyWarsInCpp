#ifndef __BABY_WARS_GAME_LOGIC__
#define __BABY_WARS_GAME_LOGIC__

#include "../../BabyEngine/GameLogic/BaseGameLogic.h"

class BabyWarsGameLogic : public BaseGameLogic
{
public:
	BabyWarsGameLogic() = default;
	~BabyWarsGameLogic() = default;

private:
	virtual std::unique_ptr<BaseActorFactory> vCreateActorFactory() const override;
};

#endif // !__BABY_WARS_GAME_LOGIC__
