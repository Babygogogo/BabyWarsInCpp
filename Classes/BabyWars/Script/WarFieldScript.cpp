#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2DDimension.h"
#include "../Utilities/MovingPath.h"
#include "MovingAreaScript.h"
#include "MovingPathScript.h"
#include "TileMapScript.h"
#include "UnitMapScript.h"
#include "WarFieldScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarFieldScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WarFieldScript::WarFieldScriptImpl
{
	WarFieldScriptImpl() = default;
	~WarFieldScriptImpl() = default;

	bool onDragField(const EvtDataInputDrag & drag);

	void dragField(const cocos2d::Vec2 & offset);
	cocos2d::Size getMapSize() const;

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;
	static std::string s_MovingPathActorPath;
	static std::string s_MovingAreaActorPath;

	bool m_IsDraggingField{ false };

	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
	std::weak_ptr<MovingPathScript> m_ChildMovingPathScript;
	std::weak_ptr<MovingAreaScript> m_ChildMovingAreaScript;
};

std::string WarFieldScript::WarFieldScriptImpl::s_TileMapActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_UnitMapActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_MovingPathActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_MovingAreaActorPath;

bool WarFieldScript::WarFieldScriptImpl::onDragField(const EvtDataInputDrag & drag)
{
	const auto dragState = drag.getState();
	if (dragState == EvtDataInputDrag::DragState::Begin) {
		assert(!m_IsDraggingField && "WarFieldScriptImpl::onDragField() the drag is in begin state while the dragging field flag is already on.");
		m_IsDraggingField = true;

		dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
		return true;
	}

	if (m_IsDraggingField) {
		if (dragState == EvtDataInputDrag::DragState::End)
			m_IsDraggingField = false;

		dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
		return true;
	}

	return false;
}

void WarFieldScript::WarFieldScriptImpl::dragField(const cocos2d::Vec2 & offset)
{
	assert(!m_TransformComponent.expired() && "WarFieldScriptImpl::dragField() the transform component is expired.");
	auto transformComponent = m_TransformComponent.lock();
	auto newPosition = transformComponent->getPosition() + offset;

	//////////////////////////////////////////////////////////////////////////
	//Modify the new position so that the scene can't be set too far away.
	//Firstly, get the size of window, boundary and scene.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	auto warSceneSize = getMapSize();
	warSceneSize.width *= transformComponent->getScaleX();
	warSceneSize.height *= transformComponent->getScaleY();

	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(warSceneSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(warSceneSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;

	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX) {
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else { //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = transformComponent->getPosition().x;
	}
	if (maxY > minY) {
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else { //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = transformComponent->getPosition().y;
	}

	//////////////////////////////////////////////////////////////////////////
	//Make use of the newPosition.
	transformComponent->setPosition(newPosition);
}

cocos2d::Size WarFieldScript::WarFieldScriptImpl::getMapSize() const
{
	auto tileMapDimension = m_ChildTileMapScript.lock()->getMapDimension();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();

	return{ gridSize.width * tileMapDimension.colCount, gridSize.height * tileMapDimension.rowCount };
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarFieldScript.
//////////////////////////////////////////////////////////////////////////
WarFieldScript::WarFieldScript() : pimpl{ std::make_shared<WarFieldScriptImpl>() }
{
}

WarFieldScript::~WarFieldScript()
{
}

bool WarFieldScript::onInputTouch(const EvtDataInputTouch & touch)
{
	if (pimpl->m_ChildUnitMapScript.lock()->onInputTouch(touch))
		return true;

	return false;
}

bool WarFieldScript::onInputDrag(const EvtDataInputDrag & drag)
{
	if (pimpl->m_ChildUnitMapScript.lock()->onInputDrag(drag))
		return true;
	if (pimpl->m_ChildMovingPathScript.lock()->onInputDrag(drag))
		return true;

	return pimpl->onDragField(drag);
}

void WarFieldScript::loadWarField(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "WarFieldScript::loadWarField() the xml element is nullptr.");

	//Load everything on the scene.
	pimpl->m_ChildTileMapScript.lock()->loadTileMap(xmlElement->FirstChildElement("TileMap")->Attribute("Value"));
	pimpl->m_ChildUnitMapScript.lock()->loadUnitMap(xmlElement->FirstChildElement("UnitMap")->Attribute("Value"));
	//#TODO: Load weather, commander, ...

	//Check if the size of unit map and tile map is the same.
	auto tileMapScript = pimpl->m_ChildTileMapScript.lock();
	auto unitMapScript = pimpl->m_ChildUnitMapScript.lock();
	assert(unitMapScript->getMapDimension() == tileMapScript->getMapDimension() && "WarFieldScript::loadWarField() the size of the unit map is not the same as the tile map.");

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->getMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	pimpl->m_TransformComponent.lock()->setPosition({ posX, posY });
}

bool WarFieldScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarFieldScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarFieldScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");
	WarFieldScriptImpl::s_MovingPathActorPath = relatedActorElement->Attribute("MovePath");
	WarFieldScriptImpl::s_MovingAreaActorPath = relatedActorElement->Attribute("MovingRange");

	isStaticInitialized = true;
	return true;
}

void WarFieldScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();

	//////////////////////////////////////////////////////////////////////////
	//Get components.
	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "WarSceneScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	//////////////////////////////////////////////////////////////////////////
	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	//Create and add the tile map.
	auto tileMapActor = gameLogic->createActor(WarFieldScriptImpl::s_TileMapActorPath.c_str());
	pimpl->m_ChildTileMapScript = tileMapActor->getComponent<TileMapScript>();
	ownerActor->addChild(*tileMapActor);

	//Create and add the unit map.
	auto unitMapActor = gameLogic->createActor(WarFieldScriptImpl::s_UnitMapActorPath.c_str());
	pimpl->m_ChildUnitMapScript = unitMapActor->getComponent<UnitMapScript>();
	ownerActor->addChild(*unitMapActor);

	//Create and add the moving area.
	auto movingAreaActor = gameLogic->createActor(WarFieldScriptImpl::s_MovingAreaActorPath.c_str());
	auto movingAreaScript = movingAreaActor->getComponent<MovingAreaScript>();
	movingAreaScript->setTileMapScript(pimpl->m_ChildTileMapScript);
	movingAreaScript->setUnitMapScript(pimpl->m_ChildUnitMapScript);
	pimpl->m_ChildMovingAreaScript = std::move(movingAreaScript);
	ownerActor->addChild(*movingAreaActor);

	//Create and add the moving path.
	auto movingPathActor = gameLogic->createActor(WarFieldScriptImpl::s_MovingPathActorPath.c_str());
	auto movingPathScript = movingPathActor->getComponent<MovingPathScript>();
	movingPathScript->setMovingAreaScript(pimpl->m_ChildMovingAreaScript);
	pimpl->m_ChildMovingPathScript = std::move(movingPathScript);
	ownerActor->addChild(*movingPathActor);

	//#TODO: create and add weather and so on...
}

const std::string WarFieldScript::Type{ "WarFieldScript" };

const std::string & WarFieldScript::getType() const
{
	return Type;
}
