#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "WarFieldScript.h"
#include "WarSceneScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneScriptImpl and the touch state classes.
//////////////////////////////////////////////////////////////////////////
struct WarSceneScript::WarSceneScriptImpl
{
	WarSceneScriptImpl() = default;
	~WarSceneScriptImpl() = default;

	void onInputDrag(const IEventData & e);
	void onInputTouch(const IEventData & e);

	static std::string s_WarFieldActorPath;

	ActorID m_ActorID{ INVALID_ACTOR_ID };

	std::weak_ptr<WarFieldScript> m_WarFieldScript;
};

std::string WarSceneScript::WarSceneScriptImpl::s_WarFieldActorPath;

void WarSceneScript::WarSceneScriptImpl::onInputDrag(const IEventData & e)
{
	const auto & eventDrag = static_cast<const EvtDataInputDrag &>(e);
	if (m_ActorID != eventDrag.getActorID())
		return;

	//#TODO: Dispatch the input event to children scripts.
	m_WarFieldScript.lock()->onInputDrag(eventDrag);
}

void WarSceneScript::WarSceneScriptImpl::onInputTouch(const IEventData & e)
{
	const auto & eventTouch = static_cast<const EvtDataInputTouch &>(e);
	if (m_ActorID != eventTouch.getActorID())
		return;

	//#TODO: Dispatch the input event to children scripts.
	m_WarFieldScript.lock()->onInputTouch(eventTouch);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneScript.
//////////////////////////////////////////////////////////////////////////
WarSceneScript::WarSceneScript() : pimpl{ std::make_shared<WarSceneScriptImpl>() }
{
}

WarSceneScript::~WarSceneScript()
{
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
	pimpl->m_WarFieldScript.lock()->loadWarField(dataPathElement);
	//#TODO: Load weather, commander, ...
}

bool WarSceneScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_WarFieldActorPath = relatedActorElement->Attribute("WarField");

	isStaticInitialized = true;
	return true;
}

void WarSceneScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	pimpl->m_ActorID = ownerActor->getID();

	//////////////////////////////////////////////////////////////////////////
	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	//Create and add the war field.
	auto warFieldActor = gameLogic->createActor(WarSceneScriptImpl::s_WarFieldActorPath.c_str());
	pimpl->m_WarFieldScript = warFieldActor->getComponent<WarFieldScript>();
	ownerActor->addChild(*warFieldActor);

	//#TODO: create and add, commander, weather and so on...

	//////////////////////////////////////////////////////////////////////////
	//Attach to event dispatcher to listen to the input event.
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataInputTouch::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onInputTouch(e);
	});
	eventDispatcher->vAddListener(EvtDataInputDrag::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onInputDrag(e);
	});

	//////////////////////////////////////////////////////////////////////////
	//Load the test warScene.
	//#TODO: Only for testing and should be removed.
	loadWarScene("Data\\WarScene\\WarSceneData_TestWarScene.xml");
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
