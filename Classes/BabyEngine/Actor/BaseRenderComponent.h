#ifndef __BASE_RENDER_COMPONENT__
#define __BASE_RENDER_COMPONENT__

#include <functional>

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
 *	To set up a tree of SceneNode, the recommended approach is:
 *	1. Get an actor namely "parent" with a SceneRenderComponent. The actor must not have a parent. If you don't have any, create one. (Assume that the name of the underlying Scene is "parentNode".)
 *	2. Create another actor namely "child" with a render component other than SceneRenderComponent. (Assume that the name of the underlying SceneNode is "childNode".)
 *	3. Call parent.addChild(child). This method internally calls parentNode->addChild(childNode) by default. You can customize the behavior by code or even by xml.
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
	friend class Actor;		//For onOwnerActorAddChild(), onOwnerActorRemoveChild().

public:
	using OwnerActorAddChildCallback = std::function<void(cocos2d::Node*, cocos2d::Node*)>;

	~BaseRenderComponent() = default;

	//This is mainly for addChild() and related functions. Use other component to modify the node when possible.
	//Warning: You should not call retain(), release(), or any functions that change the lifetime of the node.
	cocos2d::Node * getSceneNode() const;

	//Call this to customize the behavior for adding child for scene nodes.
	//The default behavior is parent->addChild(child). You can replace it something like:
	//static_cast<cocos2d::ui::ListView*>(parent)->pushBackCustomItem(static_cast<cocos2d::ui::Widget*>(child))
	//which is very useful when it comes to coding ui. Also, a utility function to convert a xml to a callback is provided.
	void setOnOwnerActorAddChildCallback(OwnerActorAddChildCallback && callback);

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

private:
	void onOwnerActorAddChild(Actor & child);
	void onOwnerActorRemoveChild(Actor & child);

	OwnerActorAddChildCallback m_OnOwnerActorAddChildCallback;
};

#endif // !__BASE_RENDER_COMPONENT__
