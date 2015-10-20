#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "WarSceneScript.h"
#include "TileMapScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../GameLogic/GameLogic.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WarSceneScript::WarSceneScriptImpl
{
	WarSceneScriptImpl(){};
	~WarSceneScriptImpl(){};

	void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);

	static std::string s_TileMapActorPath;

	std::string m_TileMapDataPath;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
};

std::string WarSceneScript::WarSceneScriptImpl::s_TileMapActorPath;

void WarSceneScript::WarSceneScriptImpl::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	assert(!m_RenderComponent.expired() && "WorldScriptImpl::onTouchMoved() the render component is expired.");
	auto strongRenderComponent = m_RenderComponent.lock();
	auto underlyingNode = strongRenderComponent->getSceneNode();

	//Calculate the new position.
	auto newPosition = touch->getLocation() - touch->getPreviousLocation() + underlyingNode->getPosition();

	//Modify the new position so that the player can't drag the map too far away.
	//Firstly, get the size of window, boundary and world.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto worldSize = m_ChildTileMapScript.lock()->getUntransformedMapSize();
	worldSize.width *= underlyingNode->getScaleX();
	worldSize.height *= underlyingNode->getScaleY();
	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(worldSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(worldSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;
	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX){
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = underlyingNode->getPosition().x;

	if (maxY > minY){
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = underlyingNode->getPosition().y;

	//Make use of the newPosition.
	underlyingNode->setPosition(newPosition);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
WarSceneScript::WarSceneScript() : pimpl{ std::make_unique<WarSceneScriptImpl>() }
{
}

WarSceneScript::~WarSceneScript()
{
}

bool WarSceneScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Get the world file path.
	//#TODO: Only for testing and should be removed.
	pimpl->m_TileMapDataPath = xmlElement->Attribute("TestTileMap");

	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");

	isStaticInitialized = true;
	return true;
}

void WarSceneScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto renderComponent = ownerActor->getRenderComponent();
	pimpl->m_RenderComponent = ownerActor->getRenderComponent();

	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	//Firstly, the tile map.
	auto tileMapActor = gameLogic->createActor(WarSceneScriptImpl::s_TileMapActorPath.c_str());
	pimpl->m_ChildTileMapScript = tileMapActor->getComponent<TileMapScript>();
	ownerActor->addChild(*tileMapActor);
	//#TODO: create and add unit map, commander, weather and so on...

	//Load the test world.
	//#TODO: Only for testing and should be removed.
	if (!pimpl->m_TileMapDataPath.empty())
		pimpl->m_ChildTileMapScript.lock()->loadTileMap(pimpl->m_TileMapDataPath.c_str());

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->m_ChildTileMapScript.lock()->getUntransformedMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	renderComponent->getSceneNode()->setPosition(posX, posY);

	//Enable dragging the scene.
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [](cocos2d::Touch * touch, cocos2d::Event * event){return true; };
	touchListener->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event){
		pimpl->onTouchMoved(touch, event);
	};
	auto underlyingNode = pimpl->m_RenderComponent.lock()->getSceneNode();
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, underlyingNode);
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
