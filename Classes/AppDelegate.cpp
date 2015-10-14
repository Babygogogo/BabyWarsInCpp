#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "Event/EventDispatcher.h"
#include "GameLogic/GameLogic.h"
#include "Graphic2D/SceneStack.h"
#include "Utilities/SingletonContainer.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
//Definition of AppDelegateImpl.
//////////////////////////////////////////////////////////////////////////
struct AppDelegate::AppDelegateImpl
{
	AppDelegateImpl(){};
	~AppDelegateImpl();

	void readGameSettingsFromXML(const char * xmlFilePath);

	void initGame();

	void update(float deltaSec);

	static cocos2d::Size s_DesignResolution;
	static float s_FramesPerSecnod;
	static std::string s_InitialScene;
	static std::string s_TextureList;

	std::chrono::steady_clock::time_point lastUpdateTimePoint{ std::chrono::steady_clock::now() };
};

cocos2d::Size AppDelegate::AppDelegateImpl::s_DesignResolution;
float AppDelegate::AppDelegateImpl::s_FramesPerSecnod{};
std::string AppDelegate::AppDelegateImpl::s_InitialScene;
std::string AppDelegate::AppDelegateImpl::s_TextureList;

AppDelegate::AppDelegateImpl::~AppDelegateImpl()
{
	cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
}

void AppDelegate::AppDelegateImpl::readGameSettingsFromXML(const char * xmlFilePath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlFilePath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "AppDelegateImpl::readDataFromXML() failed to load xml file.");

	//Load the design resolution.
	auto resolutionElement = rootElement->FirstChildElement("Resolution");
	s_DesignResolution.width = resolutionElement->FloatAttribute("DesignWidth");
	s_DesignResolution.height = resolutionElement->FloatAttribute("DesignHeight");

	//Load other settings.
	s_FramesPerSecnod = rootElement->FirstChildElement("FramesPerSecond")->FloatAttribute("Value");
	s_InitialScene = rootElement->FirstChildElement("InitialScenePath")->Attribute("Value");
	s_TextureList = rootElement->FirstChildElement("TextureListPath")->Attribute("Value");
}

void AppDelegate::AppDelegateImpl::initGame()
{
	//Create essential singleton components.
	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->set<IEventDispatcher>(std::make_unique<::EventDispatcher>());
	singletonContainer->set<GameLogic>(std::make_unique<GameLogic>());
	singletonContainer->set<SceneStack>(std::make_unique<SceneStack>());

	//Load textures, then run the initial scene.
	cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(s_TextureList);
	auto titleScene = singletonContainer->get<GameLogic>()->createActor(s_InitialScene.c_str());
	singletonContainer->get<SceneStack>()->pushAndRun(*titleScene);

	//Schedule update self so that we can update components once per frame in update().
	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

void AppDelegate::AppDelegateImpl::update(float deltaSec)
{
	//Calculate the delta time and update the time point.
	auto currentTimePoint = std::chrono::steady_clock::now();
	auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - lastUpdateTimePoint);
	lastUpdateTimePoint = std::move(currentTimePoint);

	//Update components.
	const auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->get<IEventDispatcher>()->vDispatchQueuedEvents();
	singletonContainer->get<GameLogic>()->vUpdate(deltaTimeMs);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of AppDelegate.
//////////////////////////////////////////////////////////////////////////
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() : pimpl{ std::make_unique<AppDelegateImpl>() }
{
}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
	//Read game settings from xml.
	pimpl->readGameSettingsFromXML("GameSettings.xml");
	const auto & designResolution = AppDelegateImpl::s_DesignResolution;

	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("BabyWars", Rect(0, 0, designResolution.width, designResolution.height));
#else
		glview = GLViewImpl::create("BabyWars");
#endif
		director->setOpenGLView(glview);
}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / AppDelegateImpl::s_FramesPerSecnod);

	// Set the design resolution
	glview->setDesignResolutionSize(designResolution.width, designResolution.height, ResolutionPolicy::NO_BORDER);
	Size frameSize = glview->getFrameSize();
	// if the frame's height is larger than the height of medium size.
	if (frameSize.height > mediumResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolution.height, largeResolutionSize.width / designResolution.width));
	}
	// if the frame's height is larger than the height of small size.
	else if (frameSize.height > smallResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolution.height, mediumResolutionSize.width / designResolution.width));
	}
	// if the frame's height is smaller than the height of medium size.
	else
	{
		director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolution.height, smallResolutionSize.width / designResolution.width));
	}

	register_all_packages();

	pimpl->initGame();

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
