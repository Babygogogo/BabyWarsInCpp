#ifndef __ACTOR_FACTORY__
#define __ACTOR_FACTORY__

#include <memory>
#include <vector>

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class Actor;

/*!
 * \class ActorFactory
 *
 * \brief A factory for creating Actors.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class ActorFactory
{
public:
	ActorFactory();
	~ActorFactory();

	//Create an Actor and its children actors. The resourceFile is the file name of the corresponding .xml file.
	//modifyActor() will be called after the creation of the actors.
	//If fail to create any actor, an empty vector is returned.
	//Otherwise, an vector of created actors is returned. The front actor in the vector is the most parent one.
	std::vector<std::shared_ptr<Actor>> createActorAndChildren(const char *resourceFile, tinyxml2::XMLElement *overrides = nullptr);

	//Modify an Actor with some xml data.
	//It will re-initialize its components and/or attach new ones to it.
	void modifyActor(const std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides);

	//Disable copy/move constructor and operator=.
	ActorFactory(const ActorFactory&) = delete;
	ActorFactory(ActorFactory&&) = delete;
	ActorFactory& operator=(const ActorFactory&) = delete;
	ActorFactory& operator=(ActorFactory&&) = delete;

private:
	//Implementation stuff.
	struct ActorFactoryImpl;
	std::unique_ptr<ActorFactoryImpl> pimpl;
};

#endif // !__ACTOR_FACTORY__
