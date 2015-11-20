#ifndef __BASE_RENDER_COMPONENT__
#define __BASE_RENDER_COMPONENT__

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Node;
	class Action;
}
class RelativePosition;

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
	friend class Actor;	//For addChild() and removeFromParent().

public:
	~BaseRenderComponent() = default;

	//Getter for underlying object which automatically downcasts the pointer to the type you specified.
	template <class T = cocos2d::Node>
	T * getSceneNode() const
	{
		return dynamic_cast<T*>(m_Node);
	}

	void setPosition(const RelativePosition & relativePosition);
	void setVisible(bool visible);

	//#TODO: Functions about action should be refactored (maybe extract to a ActionComponent).
	void runAction(cocos2d::Action * action);
	void stopAllActions();

	//Disable copy/move constructor and operator=.
	BaseRenderComponent(const BaseRenderComponent &) = delete;
	BaseRenderComponent(BaseRenderComponent &&) = delete;
	BaseRenderComponent & operator=(const BaseRenderComponent &) = delete;
	BaseRenderComponent & operator=(BaseRenderComponent &&) = delete;

protected:
	BaseRenderComponent() = default;

	//Concrete render components should create and retain their scene node here.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override = 0;

	cocos2d::Node *m_Node{ nullptr };

private:
	//Called by the owner Actor.
	void addChild(const BaseRenderComponent & child);
	void removeFromParent();
};

#endif // !__BASE_RENDER_COMPONENT__
