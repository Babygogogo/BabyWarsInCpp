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
		DraggingScene,
		MakingMovePath,
		UnitActivated
	};
	TouchState m_TouchState{ TouchState::Idle }, m_PreviousTouchState{ TouchState::Idle };
	bool m_IsTouchInitialized{ false };
	cocos2d::Vec2 m_TouchOneByOneBeganPosition;
	cocos2d::EventListenerTouchOneByOne * m_TouchOneByOne{ cocos2d::EventListenerTouchOneByOne::create() };

	//////////////////////////////////////////////////////////////////////////
	//Other members.
	cocos2d::Vec2 convertToSceneSpace(const cocos2d::Vec2 & position) const;
	void setPositionWithOffsetAndBoundary(const cocos2d::Vec2 & offset);

	void makeMovePath();
	void clearMovePath();
	bool isMovePathValid() const;

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;

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
	m_TouchOneByOneBeganPosition = touch->getLocation();
	return true;
}

void WarSceneScript::WarSceneScriptImpl::onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (m_TouchState == TouchState::Idle){
		//The player just starts dragging the scene, without doing any other things.
		//Just set the touch state to DraggingScene and set the position of the scene.
		m_PreviousTouchState = m_TouchState;
		m_TouchState = TouchState::DraggingScene;

		setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
	}
	else if (m_TouchState == TouchState::DraggingScene){
		//The player continues dragging the scene, without doing any other things.
		//Just set the position of the scene.
		setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
	}
	else if (m_TouchState == TouchState::UnitActivated){
		//The player drag something while an unit is activated. There are some possible meanings of the drag.

		//Some variables to make the job easier.
		auto unitMapScript = m_ChildUnitMapScript.lock();
		auto convertedPosition = convertToSceneSpace(m_TouchOneByOneBeganPosition);

		//1. The player doesn't touch any units, or the unit he touches is not the activated unit. It means that he's dragging the scene.
		if (!unitMapScript->isActiveUnitAtPosition(convertedPosition)){
			//Just set the touch state to DraggingScene and set the position of the scene.
			m_PreviousTouchState = m_TouchState;
			m_TouchState = TouchState::DraggingScene;

			setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
			return;
		}
		else{
			//2. The player is dragging the activated unit. It means that he's making a move path for the unit.
			//Set the touch state to DrawingMovePath and show the path as player touch.
			m_TouchState = TouchState::MakingMovePath;

			makeMovePath();
		}
	}
	else if (m_TouchState == TouchState::MakingMovePath){
		//The player is continuing making move path for the active unit.
		//Show the path as he touch.
		makeMovePath();
	}
}

void WarSceneScript::WarSceneScriptImpl::onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (m_TouchState == TouchState::Idle){
		//The player just touch something without moving.
		//If he touches an unit, activate it and set the touch state to UnitActivated. Do nothing otherwise.
		if (m_ChildUnitMapScript.lock()->setActiveUnitAtPosition(true, convertToSceneSpace(m_TouchOneByOneBeganPosition)))
			m_TouchState = TouchState::UnitActivated;
	}
	else if (m_TouchState == TouchState::DraggingScene){
		//The player just finishes dragging the scene.
		//Just set the touch state to the previous state and do nothing else.
		m_TouchState = m_PreviousTouchState;

		return;
	}
	else if (m_TouchState == TouchState::UnitActivated){
		//The player touched something without moving. There are some possible meanings of the touch.
		//There will be much more conditions here. For now, we just assumes that the player is to activate another unit or deactivate the active unit.
		//#TODO: Add more conditions, such as the player touched a command and so on.

		auto unitMapScript = m_ChildUnitMapScript.lock();
		auto convertedPosition = convertToSceneSpace(touch->getLocation());

		if (unitMapScript->isActiveUnitAtPosition(convertedPosition)){
			//The player touched the active unit. It means that he'll deactivate it.
			unitMapScript->setActiveUnitAtPosition(false, convertedPosition);
			m_TouchState = TouchState::Idle;
		}
		else{
			//The player touched an inactive unit or an empty grid.
			//Activate the unit (if it exist) and set the touch state corresponding to the activate result.
			if (!unitMapScript->setActiveUnitAtPosition(true, convertedPosition))
				m_TouchState = TouchState::Idle;
		}
	}
	else if (m_TouchState == TouchState::MakingMovePath){
		//The player finishes making the move path.

		if (isMovePathValid()){
			m_ChildUnitMapScript.lock()->moveAndDeactivateUnit(convertToSceneSpace(m_TouchOneByOneBeganPosition), convertToSceneSpace(touch->getLocation()));

			m_TouchState = TouchState::Idle;
		}
		else{
			clearMovePath();
			m_ChildUnitMapScript.lock()->setActiveUnitAtPosition(false, convertToSceneSpace(m_TouchOneByOneBeganPosition));

			m_TouchState = TouchState::Idle;
		}
	}
}

cocos2d::Vec2 WarSceneScript::WarSceneScriptImpl::convertToSceneSpace(const cocos2d::Vec2 & position) const
{
	return m_RenderComponent.lock()->getSceneNode()->convertToNodeSpace(position);
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

void WarSceneScript::WarSceneScriptImpl::makeMovePath()
{
	//#TODO: Complete this function.
}

void WarSceneScript::WarSceneScriptImpl::clearMovePath()
{
	//#TODO: Complete this function.
}

bool WarSceneScript::WarSceneScriptImpl::isMovePathValid() const
{
	//#TODO: Complete this function.
	return true;
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
	loadWarScene("Data\\WarScene\\WarSceneData_TestWarScene.xml");
}

void WarSceneScript::loadWarScene(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "WarSceneScript::loadWarScene() failed to load xml file.");

	//Load everything on the scene.
	auto dataPathElement = rootElement->FirstChildElement("DataPath");
	pimpl->m_ChildTileMapScript.lock()->loadTileMap(dataPathElement->FirstChildElement("TileMap")->Attribute("Value"));
	pimpl->m_ChildUnitMapScript.lock()->loadUnitMap(dataPathElement->FirstChildElement("UnitMap")->Attribute("Value"));
	//#TODO: Load weather, commander, ...

	//Check if the size of unit map and tile map is the same.
	auto tileMapScript = pimpl->m_ChildTileMapScript.lock();
	auto unitMapScript = pimpl->m_ChildUnitMapScript.lock();
	assert(unitMapScript->getRowCount() == tileMapScript->getRowCount() && unitMapScript->getColumnCount() == tileMapScript->getColumnCount()
		&& "WarSceneScript::vPostInit() the size of the unit map is not the same as the tile map.");

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->m_ChildTileMapScript.lock()->getUntransformedMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	pimpl->m_RenderComponent.lock()->getSceneNode()->setPosition(posX, posY);

	//Listen to touch events for the scene.
	pimpl->enableTouch(true);
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
