#include "cocos2d.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "SceneRenderComponent.h"
#include "../Utilities/XMLToScene.h"
#include "../Utilities/XMLToTransitionScene.h"
#include "../Utilities/SetSceneNodePropertiesWithXML.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SceneRenderComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct SceneRenderComponent::SceneRenderComponentImpl
{
	SceneRenderComponentImpl() = default;
	~SceneRenderComponentImpl() = default;

	cocos2d::TransitionScene * m_TransitionScene{ nullptr };
};

//////////////////////////////////////////////////////////////////////////
//Implementation of SceneRenderComponent.
//////////////////////////////////////////////////////////////////////////
SceneRenderComponent::SceneRenderComponent() : pimpl{ std::make_unique<SceneRenderComponentImpl>() }
{
}

SceneRenderComponent::~SceneRenderComponent()
{
	if (m_Node) {
		m_Node->removeFromParent();
		m_Node->removeAllChildren();
		CC_SAFE_RELEASE_NULL(m_Node);
	}

	if (pimpl->m_TransitionScene) {
		pimpl->m_TransitionScene->removeFromParent();
		pimpl->m_TransitionScene->removeAllChildren();
		CC_SAFE_RELEASE_NULL(pimpl->m_TransitionScene);
	}
}

cocos2d::Scene * SceneRenderComponent::getScene() const
{
	return static_cast<cocos2d::Scene*>(m_Node);
}

cocos2d::TransitionScene * SceneRenderComponent::getTransitionScene() const
{
	return pimpl->m_TransitionScene;
}

bool SceneRenderComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	//Create the scene.
	assert(!m_Node && "SceneRenderComponent::vInit() the scene is already initialized.");
	m_Node = utilities::XMLToScene(xmlElement->FirstChildElement("Scene"));
	assert(m_Node && "SceneRenderComponent::vInit() can't create scene from xml.");
	m_Node->retain();

	if (auto scenePropertiesElement = xmlElement->FirstChildElement("SceneProperties"))
		utilities::setSceneNodePropertiesWithXML(m_Node, scenePropertiesElement);

	//Create the transition scene if necessary.
	assert(!pimpl->m_TransitionScene && "SceneRenderComponent::vInit() the transition scene is already initialized.");
	if (auto transitionSceneElement = xmlElement->FirstChildElement("TransitionScene")) {
		pimpl->m_TransitionScene = utilities::XMLToTransitionScene(transitionSceneElement, static_cast<cocos2d::Scene*>(m_Node));
		pimpl->m_TransitionScene->retain();

		if (auto transitionScenePropretiesElement = xmlElement->FirstChildElement("TransitionSceneProperties"))
			utilities::setSceneNodePropertiesWithXML(pimpl->m_TransitionScene, transitionScenePropretiesElement);
	}

	return true;
}

const std::string SceneRenderComponent::Type{ "SceneRenderComponent" };

const std::string & SceneRenderComponent::getType() const
{
	return Type;
}
