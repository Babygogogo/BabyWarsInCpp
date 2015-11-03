#ifndef __BASE_HUMAN_VIEW__
#define __BASE_HUMAN_VIEW__

#include <memory>

#include "BaseGameView.h"
#include "../Actor/ActorID.h"

namespace cocos2d
{
	class Node;
}
class Actor;

/*!
 * \class BaseHumanView
 *
 * \brief
 *
 * \author Babygogogo
 * \date 2015.11
 */
class BaseHumanView : public BaseGameView
{
public:
	virtual ~BaseHumanView();

	void init(std::weak_ptr<BaseHumanView> self);

	void addActor(const std::shared_ptr<Actor> & actor);
	void removeActor(ActorID actorID);

	cocos2d::Node * getSceneNode() const;

protected:
	BaseHumanView();

	//Called within init(). Override this in subclass to load any game-specific resources.
	virtual void vLoadResource() = 0;

private:
	//Implementation stuff.
	struct BaseHumanViewImpl;
	std::shared_ptr<BaseHumanViewImpl> pimpl;
};

#endif // !__BASE_HUMAN_VIEW__
