#ifndef __ACTOR_COMPONENT__
#define __ACTOR_COMPONENT__

#include <memory>
#include <string>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class BaseRenderComponent;

/*!
 * \brief The base class of every components and scripts.
 *
 * \details
 *
 * \author Babygogogo
 * \date 2015.3
 */
class ActorComponent
{
	friend class Actor;				//For vPostInit().
	friend class BaseActorFactory;	//For vInit(), setOwnerActor().

public:
	virtual ~ActorComponent() = default;

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

	//Convenient function for getting concrete render component, which automatically downcast the returned pointer.
	//Returns nullptr if no such component attached.
	//Warning: Prefer using std::weak_ptr if you need ownership. See the comment for Actor class for details.
	template<typename T>
	std::shared_ptr<T> getRenderComponent() const	//T should derive from BaseRenderComponent.
	{
		return std::dynamic_pointer_cast<T>(getRenderComponent());
	}

	virtual const std::string & getType() const = 0;

protected:
	//Called by ActorFactory, after the creation of the component and before attaching to an actor.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement);
	//Called by by Actor by ActorFactory, after attaching all components to an actor. No default behavior.
	virtual void vPostInit();
	virtual void vOnChanged();

	std::weak_ptr<Actor> m_OwnerActor;

private:
	void setOwner(const std::shared_ptr<Actor> & owner);
};

#endif // !__ACTOR_COMPONENT__
