#ifndef __SCENE_RENDER_COMPONENT__
#define __SCENE_RENDER_COMPONENT__

#include "BaseRenderComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Scene;
	class TransitionScene;
}

class SceneRenderComponent : public BaseRenderComponent
{
public:
	SceneRenderComponent();
	~SceneRenderComponent();

	cocos2d::Scene * getScene() const;
	cocos2d::TransitionScene * getTransitionScene() const;

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	SceneRenderComponent(const SceneRenderComponent&) = delete;
	SceneRenderComponent(SceneRenderComponent&&) = delete;
	SceneRenderComponent& operator=(const SceneRenderComponent&) = delete;
	SceneRenderComponent& operator=(SceneRenderComponent&&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;

	//Implementation stuff.
	struct SceneRenderComponentImpl;
	std::unique_ptr<SceneRenderComponentImpl> pimpl;
};

#endif // !__SCENE_RENDER_COMPONENT__
