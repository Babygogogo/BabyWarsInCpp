#ifndef __ACTOR__
#define __ACTOR__

#include <memory>
#include <chrono>
#include <string>

#include "ActorID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class ActorComponent;
class BaseRenderComponent;

/*!
 * \brief	Almost everything in the game is a Actor, such as an unit, a strategy map, an effect of an explosion, and so on.
 *
 * \details
 *	Actors are organized in the form of trees. Methods like addChild(), removeFromParent() are provided for organizing actors.
 *	If a parent actor is destroyed, all of its children will be destroyed too.
 *	Roots of the trees are Actors that created as "scene". They should be push into SceneStack to be actually running.
 *	Actor is a container of various components and/or scripts which implement most of the logics of the real game object.
 *
 * \warning
 * All the component getters returns raw pointers. This implies that:
 * - You should have a shared_ptr actor, which owns the components, to ensure that the returned pointer is alive.
 * - You should not own the returned pointer.
 *
 * \author	Babygogogo
 * \date	2015.03
 */
class Actor final
{
	friend class ActorFactory;
	friend class GameLogic;

public:
	Actor();
	~Actor();

	const ActorID & getID() const;
	const ActorID & getParentID() const;

	//Get an attached component by its type name. Returns nullptr if no such component attached.
	//Warning: You should not own the pointer returned by this method.
	ActorComponent * getComponent(const std::string & type) const;

	//Convenient function for getting component, which automatically downcast the pointer.
	//Returns nullptr if no such component attached.
	//Warning: You should not own the pointer returned by this method.
	template<typename T>
	T * getComponent() const	//T should derive from Component
	{
		return dynamic_cast<T*>(getComponent(T::Type));
	}

	//Convenient function for getting the base of render component. An actor can have no more than one concrete render component.
	//If there is no render component attached, nullptr is returned.
	//Warning: You should not own the pointer returned by this method.
	BaseRenderComponent * getRenderComponent() const;

	//Stuff for organizing the Actors as trees.
	bool hasParent() const;
	bool isAncestorOf(const Actor & child) const;
	//If the child has a render component, it will also be added to the parent's render component.
	//If the child has a parent already, nothing happens.
	void addChild(Actor & child);
	//If the actor has a render component, it will also be removed from its parent's render component.
	//If the actor has no parent, nothing happens.
	void removeFromParent();

	//Disable copy/move constructor and operator=.
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	//Called by ActorFactory right after the actor is created.
	//Loads the basic data of the actor from the xmlElement. Doesn't create or attach any component.
	bool init(ActorID id, tinyxml2::XMLElement *xmlElement);

	//Called by ActorFactory after the actor is initialized and all its components are attached.
	//Calls vPostInit() on every attached component.
	void postInit();

	//Called by ActorFactory during the creation of the actor.
	void addComponent(std::unique_ptr<ActorComponent> && component);

	//Called by GameLogic on every game loop.
	//Calles vUpdate() on every attached component.
	void update(const std::chrono::milliseconds & delteTimeMs);

	//Implementation stuff.
	struct ActorImpl;
	std::unique_ptr<ActorImpl> pimpl;
};

#endif // !__ACTOR__
