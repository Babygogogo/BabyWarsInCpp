#ifndef __BASE_GAME_LOGIC__
#define __BASE_GAME_LOGIC__

#include <memory>
#include <chrono>

#include "../Actor/ActorID.h"
#include "../UserInterface/GameViewID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class Actor;
class BaseGameView;
class BaseHumanView;
class BaseActorFactory;

/*!
 * \class BaseGameLogic
 *
 * \brief Represents the world of the game.
 *
 * \details
 *	Inherit from this class to make a game-specific logic. The inherited class should be singleton.
 *	By now, its main job is to maintain actors and game views.
 *	You can create actors by calling createActorAndChildren(). But to destroy actors, you must dispatch events.
 *	Avoid owning any std::shared_ptr<Actor> outside this class, otherwise it will be hard to destory that actor.
 *	For simplicity, the logic can own one (and only) human view.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseGameLogic
{
public:
	BaseGameLogic();
	~BaseGameLogic();

	//Call this before you call any other members.
	void init(std::weak_ptr<BaseGameLogic> self);

	//Update the game world (ProcessRunner, views, actors and so on)
	void vUpdate(const std::chrono::milliseconds & deltaTimeMs);

	bool isActorAlive(ActorID actorId) const;

	//Get actor with an id. You will get nullptr if the id is not in use.
	std::shared_ptr<Actor> getActor(ActorID actorId) const;

	//Create an actor with a .xml file or xml element. The actor will be add into the internal actor list.
	//Children actors will also be created and added into the internal actor list if the xml file specified.
	//In this case, the most parent actor will be returned.
	std::shared_ptr<Actor> createActorAndChildren(const char * resourceFile, const tinyxml2::XMLElement * overrides = nullptr);
	std::shared_ptr<Actor> createActorAndChildren(const tinyxml2::XMLElement * actorElement, const tinyxml2::XMLElement * overrides = nullptr);

	//There's no destroyActor() here. To destroy an actor, you must dispatch an EvtDataRequestDestroyActor with the id of that actor.
	//Warning: If you own a std::shared_ptr<Actor> corresponding to the id outside this class, the destruction is not guaranteed to happen.
	//Warning: You can't destroy an actor that is also the current scene, or an assertion will be triggered. Instead, you should destroy the scene through SceneStack.

	//View stuff.
	void addView(std::shared_ptr<BaseGameView> gameView);
	std::shared_ptr<BaseGameView> getGameView(GameViewID viewID) const;
	void removeView(GameViewID viewID);

	//By now, the game logic can have no more than one human view. Once set, you can't replace nor remove it.
	void setHumanView(const std::shared_ptr<BaseHumanView> & humanView);
	void setHumanView(std::shared_ptr<BaseHumanView> && humanView);
	std::shared_ptr<BaseHumanView> getHumanView() const;

	//Disable copy/move constructor and operator=.
	BaseGameLogic(const BaseGameLogic&) = delete;
	BaseGameLogic(BaseGameLogic&&) = delete;
	BaseGameLogic& operator=(const BaseGameLogic&) = delete;
	BaseGameLogic& operator=(BaseGameLogic&&) = delete;

protected:
	//Called within init(). Override this in subclass to create and initialize a game-specific actor factory.
	virtual std::unique_ptr<BaseActorFactory> vCreateActorFactory() const = 0;

private:
	//Implementation stuff.
	struct BaseGameLogicImpl;
	std::shared_ptr<BaseGameLogicImpl> pimpl;
};

#endif // !__BASE_GAME_LOGIC__
