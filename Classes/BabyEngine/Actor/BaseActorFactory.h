#ifndef __BASE_ACTOR_FACTORY__
#define __BASE_ACTOR_FACTORY__

#include <memory>
#include <vector>
#include <functional>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class Actor;
class ActorComponent;

/*!
 * \class BaseActorFactory
 *
 * \brief A factory for creating Actors with their components, using the data from xml.
 *
 * \details
 *	Before you can create actors, you must call init() to make the factory aware of the existence of the actor components.
 *	This base class register general components already. If the general components are modified, you must come here to change the register calls.
 *	To register the game-specific components, you should create a game-specific factory inheriting from this class, and register components there.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseActorFactory
{
public:
	BaseActorFactory();
	~BaseActorFactory();

	//Call this before you call any other members.
	void init();

	//Create an Actor and its children actors. The resourceFile is the file name of the corresponding .xml file.
	//modifyActor() will be called after the creation of the actors.
	//If fail to create any of the actors, an empty vector is returned.
	//Otherwise, an vector of created actors is returned. The front actor in the vector is the most parent one.
	std::vector<std::shared_ptr<Actor>> createActorAndChildren(const char *resourceFile, tinyxml2::XMLElement *overrides = nullptr);
	std::vector<std::shared_ptr<Actor>> createActorAndChildren(tinyxml2::XMLElement * actorElement, tinyxml2::XMLElement * overrides = nullptr);

	//Modify an Actor with some xml data.
	//It will re-initialize its components and/or attach new ones to it.
	void modifyActor(const std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides);

	//Disable copy/move constructor and operator=.
	BaseActorFactory(const BaseActorFactory&) = delete;
	BaseActorFactory(BaseActorFactory&&) = delete;
	BaseActorFactory& operator=(const BaseActorFactory&) = delete;
	BaseActorFactory& operator=(BaseActorFactory&&) = delete;

protected:
	//Register general actor components. Called within init().
	void registerGeneralComponents();
	//Subclasses must override this to register game specific actor components. Called within init().
	virtual void vRegisterSpecificComponents() = 0;

	template<class ConcreteActorComponent>
	void registerComponent() {
		registerComponentHelper(ConcreteActorComponent::Type, std::make_unique<ConcreteActorComponent>, std::make_shared<ConcreteActorComponent>);
	}

private:
	void registerComponentHelper(const std::string & typeName, std::function<std::unique_ptr<ActorComponent>()> makeUnique, std::function<std::shared_ptr<ActorComponent>()> makeShared);

	//Implementation stuff.
	struct BaseActorFactoryImpl;
	std::unique_ptr<BaseActorFactoryImpl> pimpl;
};

#endif // !__BASE_ACTOR_FACTORY__
