#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "WarSceneScript.h"
#include "TileMapScript.h"
#include "UnitMapScript.h"
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
	WarSceneScriptImpl();
	~WarSceneScriptImpl();

	//////////////////////////////////////////////////////////////////////////
	//Members for touch.
	void enableTouch(bool enable);
	bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	enum class TouchState{
		Idle,
		DraggingScene
	};
	TouchState m_TouchState{ TouchState::Idle };
	bool m_IsTouchInitialized{ false };
	cocos2d::EventListenerTouchOneByOne * m_TouchOneByOne{ cocos2d::EventListenerTouchOneByOne::create() };
	cocos2d::Vec2 m_TouchOneByOneBeginLocation;

	//////////////////////////////////////////////////////////////////////////
	//Other members.
	void setPositionWithOffsetAndBoundary(const cocos2d::Vec2 & offset);

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;

	std::string m_TileMapDataPath;
	std::string m_UnitMapDataPath;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
};

std::string WarSceneScript::WarSceneScriptImpl::s_TileMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_UnitMapActorPath;

WarSceneScript::WarSceneScriptImpl::WarSceneScriptImpl()
{
	m_TouchOneByOne->retain();
}

WarSceneScript::WarSceneScriptImpl::~WarSceneScriptImpl()
{
	m_TouchOneByOne->release();
}

void WarSceneScript::WarSceneScriptImpl::enableTouch(bool enable)
{
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	if (!enable){
		eventDispatcher->removeEventListener(m_TouchOneByOne);
		return;
	}

	if (!m_IsTouchInitialized){
		m_TouchOneByOne->onTouchBegan = [this](cocos2d::Touch * touch, cocos2d::Event * event){
			return onTouchOneByOneBegan(touch, event);
		};
		m_TouchOneByOne->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event){
			onTouchOneByOneMoved(touch, event);
		};
		m_TouchOneByOne->onTouchEnded = [this](cocos2d::Touch * touch, cocos2d::Event * event){
			onTouchOneByOneEnded(touch, event);
		};

		m_IsTouchInitialized = true;
	}

	auto underlyingNode = m_RenderComponent.lock()->getSceneNode();
	eventDispatcher->addEventListenerWithSceneGraphPriority(m_TouchOneByOne, underlyingNode);
}

bool WarSceneScript::WarSceneScriptImpl::onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	m_TouchOneByOneBeginLocation = touch->getLocation();
	return true;
}

void WarSceneScript::WarSceneScriptImpl::onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (m_TouchState == TouchState::Idle || m_TouchState == TouchState::DraggingScene){
		m_TouchState = TouchState::DraggingScene;

		setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
	}
}

void WarSceneScript::WarSceneScriptImpl::onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (m_TouchState == TouchState::DraggingScene){
		m_TouchState = TouchState::Idle;

		return;
	}

	//Player touched something without moving. Find what player touched.
	auto renderComponent = m_RenderComponent.lock();
	auto underlyingNode = renderComponent->getSceneNode();
	auto convertedLocation = underlyingNode->convertToNodeSpace(touch->getLocation());
	//cocos2d::log("WarSceneScriptImpl::onTouchOneByOneEnded() end in %f %f", convertedLocation.x, convertedLocation.y);
	m_ChildUnitMapScript.lock()->onSingleTouch(convertedLocation);
}

void WarSceneScript::WarSceneScriptImpl::setPositionWithOffsetAndBoundary(const cocos2d::Vec2 & offset)
{
	assert(!m_RenderComponent.expired() && "WarSceneScriptImpl::setPositionWithinBoundary() the render component is expired.");
	auto strongRenderComponent = m_RenderComponent.lock();
	auto underlyingNode = strongRenderComponent->getSceneNode();
	auto newPosition = underlyingNode->getPosition() + offset;

	//////////////////////////////////////////////////////////////////////////
	//Modify the new position so that the scene can't be set too far away.
	//Firstly, get the size of window, boundary and scene.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto warSceneSize = m_ChildTileMapScript.lock()->getUntransformedMapSize();
	warSceneSize.width *= underlyingNode->getScaleX();
	warSceneSize.height *= underlyingNode->getScaleY();

	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(warSceneSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(warSceneSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;

	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX){
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else{ //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = underlyingNode->getPosition().x;
	}
	if (maxY > minY){
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else{ //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = underlyingNode->getPosition().y;
	}

	//////////////////////////////////////////////////////////////////////////
	//Make use of the newPosition.
	underlyingNode->setPosition(newPosition);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneScript.
//////////////////////////////////////////////////////////////////////////
WarSceneScript::WarSceneScript() : pimpl{ std::make_unique<WarSceneScriptImpl>() }
{
}

WarSceneScript::~WarSceneScript()
{
}

bool WarSceneScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Get the warScene file path.
	//#TODO: Only for testing and should be removed.
	pimpl->m_TileMapDataPath = xmlElement->Attribute("TestTileMap");
	pimpl->m_UnitMapDataPath = xmlElement->Attribute("TestUnitMap");

	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarSceneScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");

	isStaticInitialized = true;
	return true;
}

void WarSceneScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto renderComponent = ownerActor->getRenderComponent();
	pimpl->m_RenderComponent = ownerActor->getRenderComponent();

	//////////////////////////////////////////////////////////////////////////
	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();

	//Firstly, create and add the tile map.
	auto tileMapActor = gameLogic->createActor(WarSceneScriptImpl::s_TileMapActorPath.c_str());
	pimpl->m_ChildTileMapScript = tileMapActor->getComponent<TileMapScript>();
	ownerActor->addChild(*tileMapActor);

	//Secondly, create and add the unit map.
	auto unitMapActor = gameLogic->createActor(WarSceneScriptImpl::s_UnitMapActorPath.c_str());
	pimpl->m_ChildUnitMapScript = unitMapActor->getComponent<UnitMapScript>();
	ownerActor->addChild(*unitMapActor);

	//#TODO: create and add, commander, weather and so on...

	//////////////////////////////////////////////////////////////////////////
	//Load the test warScene.
	//#TODO: Only for testing and should be removed.
	if (!pimpl->m_TileMapDataPath.empty())
		pimpl->m_ChildTileMapScript.lock()->loadTileMap(pimpl->m_TileMapDataPath.c_str());
	if (!pimpl->m_UnitMapDataPath.empty())
		pimpl->m_ChildUnitMapScript.lock()->loadUnitMap(pimpl->m_UnitMapDataPath.c_str());

	//Check if the units are in valid positions.
	auto unitMapScript = pimpl->m_ChildUnitMapScript.lock();
	auto tileMapScript = pimpl->m_ChildTileMapScript.lock();
	assert(unitMapScript->getRowCount() == tileMapScript->getRowCount() && unitMapScript->getColumnCount() == tileMapScript->getColumnCount()
		&& "WarSceneScript::vPostInit() the size of the unit map is not the same as the tile map.");

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->m_ChildTileMapScript.lock()->getUntransformedMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	renderComponent->getSceneNode()->setPosition(posX, posY);

	//Listen to touch events for the scene.
	pimpl->enableTouch(true);
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
