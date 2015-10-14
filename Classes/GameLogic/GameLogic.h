#ifndef __GAME_LOGIC__
#define __GAME_LOGIC__

#include <memory>
#include <chrono>

#include "../Actor/ActorID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class Actor;

/*!
 * \class GameLogic
 *
 * \brief Represents the world of the game.
 *
 * \details
 *	This class should be singleton.
 *	By now, its main job is to maintain a list of all the actors in the game world.
 *	You can create actors by calling createActor(). But to destroy actors, you must dispatch events.
 *	Avoid owning any std::shared_ptr<Actor> outside this class, otherwise it will be hard to destory that actor.
 *	Much more work to do on this class...
 *
 * \author Babygogogo
 * \date 2015.7
 */
class GameLogic
{
	//#TODO: Divide this class into a BaseGameLogic and a game specific logic class.
public:
	GameLogic();
	~GameLogic();

	//Update the game world (ProcessRunner, views, actors and so on)
	void vUpdate(const std::chrono::milliseconds & deltaTimeMs);

	bool isActorAlive(const ActorID & id) const;

	//Get actor with an id. You will get nullptr if the id is not in use.
	const std::shared_ptr<Actor> & getActor(const ActorID & id) const;

	//Create an actor with a .xml file. The actors will be add into the internal actor list.
	//Children actors will also be created and added into the internal actor list if the xml file specified.
	//In this case, the most parent actor will be returned.
	std::shared_ptr<Actor> createActor(const char *resourceFile, tinyxml2::XMLElement *overrides = nullptr);

	//There's no destroyActor() here. To destroy an actor, you must dispatch an EvtDataRequestDestroyActor with the id of that actor.
	//Warning: If you own a std::shared_ptr<Actor> corresponding to the id outside this class, the destruction is not guaranteed to happen.
	//Warning: You can't destroy an actor that is also the current scene, or an assertion will be triggered. Instead, you should destroy the scene through SceneStack.

	//Disable copy/move constructor and operator=.
	GameLogic(const GameLogic&) = delete;
	GameLogic(GameLogic&&) = delete;
	GameLogic& operator=(const GameLogic&) = delete;
	GameLogic& operator=(GameLogic&&) = delete;

private:
	//Implementation stuff.
	struct GameLogicImpl;
	std::shared_ptr<GameLogicImpl> pimpl;
};

#endif // !__GAME_LOGIC__
