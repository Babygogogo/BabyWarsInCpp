#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "SceneRenderComponent.h"
#include "../../BabyEngine/Utilities/XMLToScene.h"
#include "../../BabyEngine/Utilities/XMLToTransitionScene.h"

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
	CC_SAFE_RELEASE_NULL(m_Node);
	CC_SAFE_RELEASE_NULL(pimpl->m_TransitionScene);
}

cocos2d::TransitionScene * SceneRenderComponent::getTransitionScene() const
{
	return pimpl->m_TransitionScene;
}

bool SceneRenderComponent::vInit(tinyxml2::XMLElement * xmlElement)
{
	assert(!m_Node && "SceneRenderComponent::vInit() the node is already initialized.");

	m_Node = utilities::XMLToScene(xmlElement->FirstChildElement("Scene"));
	assert(m_Node && "SceneRenderComponent::vInit() can't create scene from xml.");
	m_Node->retain();

	if (auto transitionSceneElement = xmlElement->FirstChildElement("TransitionScene")) {
		pimpl->m_TransitionScene = utilities::XMLToTransitionScene(transitionSceneElement, static_cast<cocos2d::Scene*>(m_Node));
		pimpl->m_TransitionScene->retain();
	}

	return true;
}

const std::string SceneRenderComponent::Type{ "SceneRenderComponent" };

const std::string & SceneRenderComponent::getType() const
{
	return Type;
}
