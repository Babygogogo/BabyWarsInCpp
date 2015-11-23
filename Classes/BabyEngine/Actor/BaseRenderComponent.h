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
 *	Every BaseRenderComponent has one and only one cocos2d::Node* or its children as its internal renderer.
 *	BaseRenderComponents are automatically organized in the form of trees, just like actors.
 *	That is, BaseRenderComponent::addChild() is called within Actor::addChild().
 *	For simplicity, every actor can have no more than one concrete render component.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseRenderComponent : public ActorComponent
{
	friend class Actor;					//For addChild() and removeFromParent().
	friend class BaseController;		//For getSceneNode().
	friend class TransformComponent;	//For getSceneNode().

public:
	~BaseRenderComponent() = default;

	void setVisible(bool visible);

	//#TODO: Functions about action should be refactored (maybe extract to a ActionComponent).
	void runAction(cocos2d::Action * action);
	void stopAction(cocos2d::Action * action);
	void stopAllActions();

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
	//Getter for underlying scene node. Should only be used by classes in BabyEngine.
	cocos2d::Node * getSceneNode() const;

	//Called by the owner Actor.
	void addChild(const BaseRenderComponent & child);
	void removeFromParent();
};

#endif // !__BASE_RENDER_COMPONENT__
