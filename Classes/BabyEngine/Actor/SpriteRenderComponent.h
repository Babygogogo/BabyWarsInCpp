#ifndef __SPRITE_RENDER_COMPONENT__
#define __SPRITE_RENDER_COMPONENT__

#include "BaseRenderComponent.h"

//Forward declaration.
namespace cocos2d
{
	class SpriteFrame;
}

class SpriteRenderComponent : public BaseRenderComponent
{
public:
	SpriteRenderComponent();
	~SpriteRenderComponent();

	void setSpriteFrame(cocos2d::SpriteFrame * frame);

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	SpriteRenderComponent(const SpriteRenderComponent &) = delete;
	SpriteRenderComponent(SpriteRenderComponent &&) = delete;
	SpriteRenderComponent & operator=(const SpriteRenderComponent &) = delete;
	SpriteRenderComponent & operator=(SpriteRenderComponent &&) = delete;

private:
	//Override functions.
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;

	//Implementation stuff.
	struct SpriteRenderComponentImpl;
	std::unique_ptr<SpriteRenderComponentImpl> pimpl;
};

#endif // !__SPRITE_RENDER_COMPONENT__
