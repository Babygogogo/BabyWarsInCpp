#include <vector>
#include <cassert>

#include "cocos2d.h"

#include "SceneStack.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SceneStackImpl.
//////////////////////////////////////////////////////////////////////////
struct SceneStack::SceneStackImpl
{
	SceneStackImpl();
	~SceneStackImpl();

	void validateScene(const Actor & scene) const;

	cocos2d::Scene * getInternalScene(const Actor & actor) const;

	void requestDestroyActor(const ActorID & id) const;

	//Search for 'id' in the stack. If find, pop 'id' and all ids above it.
	//All the popped scenes will be destroyed.
	void findAndPopStack(const ActorID & id);

	std::vector<ActorID> m_SceneIDs;
};

SceneStack::SceneStackImpl::SceneStackImpl()
{
}

SceneStack::SceneStackImpl::~SceneStackImpl()
{
}

void SceneStack::SceneStackImpl::validateScene(const Actor & scene) const
{
	assert(getInternalScene(scene) && "SceneStackImpl::validateScene() the scene actor has no internal scene.");
	assert(!scene.hasParent() && "SceneStackImpl::validateScene() the scene actor has a parent.");
}

cocos2d::Scene * SceneStack::SceneStackImpl::getInternalScene(const Actor & actor) const
{
	if (auto renderComponent = actor.getRenderComponent())
		return static_cast<cocos2d::Scene*>(renderComponent->getSceneNode());

	return nullptr;
}

void SceneStack::SceneStackImpl::requestDestroyActor(const ActorID & id) const
{
	if (const auto & singletonContainer = SingletonContainer::getInstance()){
		auto request = std::make_unique<EvtDataRequestDestoryActor>(id);
		singletonContainer->get<IEventDispatcher>()->vQueueEvent(std::move(request));
	}
}

void SceneStack::SceneStackImpl::findAndPopStack(const ActorID & id)
{
	for (auto sceneIndex = 0U; sceneIndex < m_SceneIDs.size(); ++sceneIndex)
		if (m_SceneIDs[sceneIndex] == id){
			//Request destroy the scenes that are going to be popped (excluding the scene of 'id').
			for (auto indexToDestroy = sceneIndex + 1; indexToDestroy < m_SceneIDs.size(); ++indexToDestroy)
				requestDestroyActor(m_SceneIDs[indexToDestroy]);

			//Pop the scenes.
			m_SceneIDs.resize(sceneIndex);
		}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of SceneStack.
//////////////////////////////////////////////////////////////////////////
SceneStack::SceneStack() : pimpl{ std::make_unique<SceneStackImpl>() }
{
	static int InstanceCount{ 0 };
	assert((InstanceCount++ == 0) && "SceneStack is created more than once!");
}

SceneStack::~SceneStack()
{
}

void SceneStack::pushAndRun(const Actor & scene)
{
	pimpl->validateScene(scene);
	pimpl->findAndPopStack(scene.getID());

	pimpl->m_SceneIDs.emplace_back(scene.getID());

	//Run the scene.
	auto director = cocos2d::Director::getInstance();
	if (director->getRunningScene())
		director->replaceScene(pimpl->getInternalScene(scene));
	else
		director->runWithScene(pimpl->getInternalScene(scene));
}

void SceneStack::popCurrentScene()
{
	assert(pimpl->m_SceneIDs.size() > 1 && "SceneStack::popCurrentScene() when the size of scenes <= 1");

	pimpl->requestDestroyActor(getCurrentSceneID());
	pimpl->m_SceneIDs.pop_back();

	//Search for a living scene from the top of the stack to the bottom.
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	while (!pimpl->m_SceneIDs.empty()){
		//If the current scene is living, run it and end this function.
		if (auto nextScene = gameLogic->getActor(getCurrentSceneID())){
			cocos2d::Director::getInstance()->replaceScene(pimpl->getInternalScene(*nextScene));
			return;
		}

		//If the current scene is not living, pop it from the stack.
		pimpl->m_SceneIDs.pop_back();
	}

	assert(false && "SceneStack::popCurrentScene() when there are no more alive scenes in the scene list.");
}

void SceneStack::replaceAndRun(const Actor & scene)
{
	assert(!pimpl->m_SceneIDs.empty() && "SceneStack::replaceAndRun() when there are no scenes at all.");

	pimpl->validateScene(scene);
	pimpl->findAndPopStack(scene.getID());

	pimpl->requestDestroyActor(getCurrentSceneID());
	pimpl->m_SceneIDs.pop_back();

	pimpl->m_SceneIDs.emplace_back(scene.getID());

	cocos2d::Director::getInstance()->replaceScene(pimpl->getInternalScene(scene));
}

const ActorID & SceneStack::getCurrentSceneID() const
{
	assert(!pimpl->m_SceneIDs.empty() && "SceneStack::getCurrentSceneID() called when there's no scene in the stack.");

	return pimpl->m_SceneIDs.back();
}
