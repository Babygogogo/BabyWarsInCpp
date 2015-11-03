#include "AppDelegate.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "BabyEngine/Event/EventDispatcher.h"
#include "BabyEngine/UserInterface/BaseHumanView.h"
#include "BabyEngine/Utilities/SingletonContainer.h"
#include "BabyWars/GameLogic/BabyWarsGameLogic.h"
#include "BabyWars/Resource/ResourceLoader.h"
#include "BabyWars/UserInterface/BabyWarsHumanView.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
//Definition of AppDelegateImpl.
//////////////////////////////////////////////////////////////////////////
struct AppDelegate::AppDelegateImpl
{
	AppDelegateImpl(){};
	~AppDelegateImpl();

	void initGame();

	void update(float deltaSec);

	std::chrono::steady_clock::time_point lastUpdateTimePoint{ std::chrono::steady_clock::now() };
};

AppDelegate::AppDelegateImpl::~AppDelegateImpl()
{
	cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
}

void AppDelegate::AppDelegateImpl::initGame()
{
	//Create essential singleton components.
	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->set<IEventDispatcher>(std::make_unique<::EventDispatcher>());

	//Load resources.
	auto resourceLoader = singletonContainer->get<ResourceLoader>();
	resourceLoader->loadResources();

	//Init the game logic.
	auto gameLogic = std::make_shared<BabyWarsGameLogic>();
	gameLogic->init(gameLogic);
	singletonContainer->set<BaseGameLogic>(gameLogic);

	//Create a human view with an initial actor, and add the view to game logic.
	auto humanView = std::make_shared<BabyWarsHumanView>();
	humanView->init(humanView);
	humanView->addActor(gameLogic->createActor(resourceLoader->getInitialScenePath().c_str()));
	gameLogic->addView(humanView);

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
	singletonContainer->get<BaseGameLogic>()->vUpdate(deltaTimeMs);
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
	auto resourceLoader = SingletonContainer::getInstance()->set<ResourceLoader>(std::make_unique<ResourceLoader>());
	resourceLoader->loadGameSettings("GameSettings.xml");
	const auto designResolution = resourceLoader->getDesignResolution();

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
	director->setAnimationInterval(1.0 / resourceLoader->getFramesPerSecond());

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
