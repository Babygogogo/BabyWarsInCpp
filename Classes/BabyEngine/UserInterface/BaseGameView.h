#ifndef __BASE_GAME_VIEW__
#define __BASE_GAME_VIEW__

#include <memory>

#include "GameViewID.h"

class BaseGameView
{
	friend class BaseGameLogic;	//For setLogic().

public:
	virtual ~BaseGameView();

	GameViewID getID() const;

	bool isAttachedToLogic() const;
	std::shared_ptr<BaseGameLogic> getLogic() const;

protected:
	BaseGameView();

private:
	void setLogic(std::weak_ptr<BaseGameLogic> gameLogic);

	//Implementation stuff.
	struct BaseGameViewImpl;
	std::unique_ptr<BaseGameViewImpl> pimpl;
};

#endif // !__BASE_GAME_VIEW__
