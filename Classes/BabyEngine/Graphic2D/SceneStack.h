#ifndef __SCENE_STACK__
#define __SCENE_STACK__

#include <memory>

#include "../Actor/ActorID.h"

class Actor;

/*!
 * \class	SceneStack
 *
 * \brief	Manages the scenes of the game.
 *
 * \details
 *	A scene is an Actor that has no parent and owns an render component which holds a cocos2d::Scene.
 *	After a scene is created, you must explicitly attach it to this class to run it.
 *	The top scene of the stack is the one and only running scene.
 *	To save memory, don't make the size of the stack too large.
 *
 * \author	Babygogogo
 * \date	2015.3
 */
class SceneStack final
{
public:
	//You can't create two or more SceneStacks, otherwise an assertion will be triggered.
	SceneStack();
	~SceneStack();

	//Stuff for controlling the stack.
	//The param scene must be a scene and has no parent, or an assertion will be triggered.
	//If the scene already exists in the stack, all scenes above it in the stack will be destroyed.
	void pushAndRun(const Actor & scene);

	//Pop the current scene and run the next-top scene. The popped scene will be destroyed.
	//Don't call this if there is only one scene in the stack, or an assertion will be triggered.
	void popCurrentScene();

	//Replace the current scene with a new one. The scene being replaced will be destroyed.
	//If the new scene already exists in the stack, all scenes above it in the stack will be destroyed.
	void replaceAndRun(const Actor & scene);

	//Return the id of the current scene. The current scene is also the top scene in the stack.
	const ActorID & getCurrentSceneID() const;

	//Disable copy/move constructor and operator=.
	SceneStack(const SceneStack&) = delete;
	SceneStack(SceneStack&&) = delete;
	SceneStack& operator=(const SceneStack&) = delete;
	SceneStack& operator=(SceneStack&&) = delete;

private:
	//Implementation stuff.
	struct SceneStackImpl;
	std::unique_ptr<SceneStackImpl> pimpl;
};

#endif // !__SCENE_STACK__
