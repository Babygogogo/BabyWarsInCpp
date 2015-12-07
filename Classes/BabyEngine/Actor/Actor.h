#ifndef __ACTOR__
#define __ACTOR__

#include <memory>
#include <chrono>
#include <string>
#include <unordered_map>

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
 *	Roots of the trees of actors must have SceneRenderComponent and be attached to human view to make the actors visible.
 *	Actor is a container of various components and/or scripts which implement most of the logics of the real game object.
 *	Actor should be created by GameLogic using std::shared_ptr and destroyed by dispatching EvtDataRequestDestroyActor.
 *
 * \note
 *	All the component getters returns std::shared_ptr only for convinience, but not for ownership.
 *	If you need an ownership outside the actor, prefer using std::weak_ptr. But if you have to own an std::shared_ptr, be careful:
 *	- Avoid circular ownership.
 *	- Release the ownership on EvtDataRequestDestroyActor, otherwise you may use the component while the owner actor is destroyed.
 *
 * \author	Babygogogo
 * \date	2015.03
 */
class Actor final
{
	friend class BaseActorFactory;	//For init(), addComponent(), postInit().
	friend class BaseGameLogic;		//For update().
	friend class BaseHumanView;		//For setHumanView().

public:
	//Actor should be created by GameLogic using std::shared_ptr and destroyed by dispatching EvtDataRequestDestroyActor.
	//The ctor is public only to enable the use of std::make_shared().
	Actor();
	~Actor();

	ActorID getID() const;

	//Get an attached component by its type name. Returns nullptr if no such component attached.
	//Warning: Prefer using std::weak_ptr if you need ownership. See the comment for Actor class for details.
	std::shared_ptr<ActorComponent> getComponent(const std::string & type) const;

	//Convenient function for getting component, which automatically downcast the returned pointer.
	//Returns nullptr if no such component attached.
	//Warning: Prefer using std::weak_ptr if you need ownership. See the comment for Actor class for details.
	template<typename T>
	std::shared_ptr<T> getComponent() const	//T should derive from ActorComponent
	{
		return std::dynamic_pointer_cast<T>(getComponent(T::Type));
	}

	//Convenient function for getting the base of render component. An actor can have no more than one concrete render component.
	//If there is no render component attached, nullptr is returned.
	//Warning: Prefer using std::weak_ptr if you need ownership. See the comment for Actor class for details.
	std::shared_ptr<BaseRenderComponent> getRenderComponent() const;

	//Convenient function for getting concrete render component, which automatically downcast the teturned pointer.
	//Returns nullptr if no such component attached.
	//Warning: Prefer using std::weak_ptr if you need ownership. See the comment for Actor class for details.
	template<typename T>
	std::shared_ptr<T> getRenderComponent() const	//T should derive from BaseRenderComponent.
	{
		return std::dynamic_pointer_cast<T>(getRenderComponent());
	}

	//Stuff for organizing the Actors as trees.
	bool hasParent() const;
	std::shared_ptr<Actor> getParent() const;
	bool isAncestorOf(const Actor & child) const;
	const std::unordered_map<ActorID, std::weak_ptr<Actor>> & getChildren() const;

	//If the child has a render component, it will also be added to the parent's render component.
	//If the child has a parent already, nothing happens.
	void addChild(Actor & child);
	//If the actor has a render component, it will also be removed from its parent's render component.
	//If the actor has no parent, nothing happens.
	//This function doesn't destroy parent nor self.
	void removeFromParent();
	//The render components of the children (if exist) will be removed from the one of this actor (if exist).
	//If the actor has no child, nothing happens.
	//This function doesn't destroy self nor children.
	void removeAllChildren();

	//Disable copy/move constructor and operator=.
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	//Called by ActorFactory right after the actor is created and before any component is attached.
	//Loads the basic data of the actor from the xmlElement. Doesn't create or attach any component.
	bool init(ActorID id, const std::shared_ptr<Actor> & selfPtr, tinyxml2::XMLElement *xmlElement);

	//Called by ActorFactory after init() and before postInit().
	void addComponent(std::shared_ptr<ActorComponent> && component);

	//Called by ActorFactory after the actor is initialized and all its components are attached.
	//Calls vPostInit() on every attached component.
	void postInit();

	//Called by GameLogic on every game loop.
	//Calles vUpdate() on every attached component.
	void update(const std::chrono::milliseconds & delteTimeMs);

	//Implementation stuff.
	struct ActorImpl;
	std::unique_ptr<ActorImpl> pimpl;
};

#endif // !__ACTOR__
