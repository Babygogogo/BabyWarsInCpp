#ifndef __TRANSFORM_COMPONENT__
#define __TRANSFORM_COMPONENT__

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Vec2;
	class Size;
}

/*!
 * \class TransformComponent
 *
 * \brief Position, rotation and so on of an actor.
 *
 * \details
 *	This component relies on render component. That is, the attached actor must have a render component.
 *	It should not be, but because the cocos2d engine binds nearly everything (including the transform) with the Node classes, I have no other choices.
 *	This component owns a std::shared_ptr to the render component, for convience and performance. Render components must not own std::shared_ptr to this component.
 *
 * \author Babygogogo
 * \date 2015.11
 */
class TransformComponent : public ActorComponent
{
public:
	TransformComponent();
	~TransformComponent();

	const cocos2d::Size & getContentSize() const;
	void setContentSize(const cocos2d::Size & size);

	int getLocalZOrder() const;
	void setLocalZOrder(int order);

	cocos2d::Vec2 getPosition() const;
	void setPosition(const cocos2d::Vec2 & position);

	float getScale() const;
	float getScaleX() const;
	float getScaleY() const;
	void setScaleToSize(const cocos2d::Size & size);

	float getRotation() const;
	void setRotation(float degree);

	cocos2d::Vec2 convertToLocalSpace(const cocos2d::Vec2 & positionInWorld) const;

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	TransformComponent(const TransformComponent &) = delete;
	TransformComponent(TransformComponent &&) = delete;
	TransformComponent & operator=(const TransformComponent &) = delete;
	TransformComponent & operator=(TransformComponent &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TransformComponentImpl;
	std::unique_ptr<TransformComponentImpl> pimpl;
};

#endif // !__TRANSFORM_COMPONENT__
