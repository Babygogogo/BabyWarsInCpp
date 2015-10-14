#ifndef __ACTOR_COMPONENT__
#define __ACTOR_COMPONENT__

#include <memory>
#include <string>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}

/*!
 * \brief The base class of every components and scripts.
 *
 * \details
 *	Refactor in progress...
 *
 * \author Babygogogo
 * \date 2015.3
 */
class ActorComponent
{
	friend class Actor;
	friend class ActorFactory;

public:
	virtual ~ActorComponent();

	virtual const std::string & getType() const = 0;

protected:
	//Called by ActorFactory, after the creation of the component and before attaching to an actor.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement);
	//Called by by Actor by ActorFactory, after attaching all components to an actor. No default behavior.
	virtual void vPostInit();
	virtual void vOnChanged();

	std::weak_ptr<Actor> m_Actor;

private:
	void setOwner(std::weak_ptr<Actor> && owner);
};

#endif // !__ACTOR_COMPONENT__
