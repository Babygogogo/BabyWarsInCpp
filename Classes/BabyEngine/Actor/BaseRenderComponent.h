#ifndef __BASE_RENDER_COMPONENT__
#define __BASE_RENDER_COMPONENT__

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Node;
	class Action;
}

/*!
 * \class BaseRenderComponent
 *
 * \brief The base class of all other render components.
 *
 * \details
 *	Every BaseRenderComponent has one and only one cocos2d::Node* (which is called SceneNode in BabyEngine) as its internal renderer.
 *	The SceneNode is organized in the form of trees. The root of the tree is called Scene and you must use SceneRenderComponent to create them.
 *	To set up a tree of SceneNode, you must:
 *	- Get an actor with a SceneRenderComponent. The actor must not have a parent. If you don't have any, create one. (Assume that the name of the underlying Scene is "Root".)
 *	- Create another actor with a render component other than SceneRenderComponent. (Assume that the name of the underlying SceneNode is "Child".)
 *	- Call Root->addChild(Child);
 *	Any node in the tree can have their own children. The method to add child is the same.
 *	(The addChild() is provided by cocos2d-x and I find it hard to encapsulate it in BabyEngine, so I just leave it to the game-sepcific code.)
 *
 *	To make the tree visible, you must:
 *	- Attach the root actor to a HumanView.
 *	- Attach the HumanView to the GameLogic.
 *
 *	For simplicity, every actor can have no more than one concrete render component.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseRenderComponent : public ActorComponent
{
public:
	~BaseRenderComponent() = default;

	//This is mainly for addChild() and related functions. Use other component to modify the node when possible.
	//Warning: You should not call retain(), release(), or any functions that change the lifetime of the node.
	cocos2d::Node * getSceneNode() const;

	//Disable copy/move constructor and operator=.
	BaseRenderComponent(const BaseRenderComponent &) = delete;
	BaseRenderComponent(BaseRenderComponent &&) = delete;
	BaseRenderComponent & operator=(const BaseRenderComponent &) = delete;
	BaseRenderComponent & operator=(BaseRenderComponent &&) = delete;

protected:
	BaseRenderComponent() = default;

	//Concrete render components should create and retain their scene node here.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) = 0;

	cocos2d::Node *m_Node{ nullptr };
};

#endif // !__BASE_RENDER_COMPONENT__
