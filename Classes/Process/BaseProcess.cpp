#include <cassert>

#include "BaseProcess.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Enum of possible process states.
//////////////////////////////////////////////////////////////////////////
enum class ProcessState{
	UnRun,			//Created but not run
	Removed,		//Registered to IProcessExecutor, then unregistered before finishing running

	Running,		//Running
	Paused,			//Paused before finishing running

	Succeeded,		//Ended in success
	Failed,			//Ended in failure
	Aborted			//Ended because of internal issue; may never run at all!
};

//////////////////////////////////////////////////////////////////////////
//Definition of BaseProcess::BaseProcessImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseProcess::BaseProcessImpl
{
public:
	BaseProcessImpl();
	~BaseProcessImpl();

	//Current state of the process.
	ProcessState m_State{ ProcessState::UnRun };

	//Child process. May be empty.
	std::unique_ptr<BaseProcess> m_Child;
};

BaseProcess::BaseProcessImpl::BaseProcessImpl()
{
}

BaseProcess::BaseProcessImpl::~BaseProcessImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseProcess.
//////////////////////////////////////////////////////////////////////////
BaseProcess::BaseProcess() : pimpl{ std::make_unique<BaseProcessImpl>() }
{
}

BaseProcess::~BaseProcess()
{
}

void BaseProcess::update(const std::chrono::milliseconds & deltaTimeMs)
{
	//If the process is not run, initialize it.
	if (pimpl->m_State == ProcessState::UnRun)
		init();

	//Update the process if running.
	if (pimpl->m_State == ProcessState::Running)
		vOnUpdate(deltaTimeMs);
}

void BaseProcess::init()
{
	//Make sure that the process is not run.
	assert(pimpl->m_State == ProcessState::UnRun);

	//Call the virtual method and update the state of the process.
	vOnInit();
	pimpl->m_State = ProcessState::Running;
}

void BaseProcess::abort()
{
	//Make sure that the process is not ended.
	assert(!isEnded());

	//Call the virtual method and update the state of the process.
	vOnAbort();
	pimpl->m_State = ProcessState::Aborted;
}

void BaseProcess::succeed()
{
	//Make sure that the process is not ended.
	assert(!isEnded());

	//Call the virtual method and update the state of the process.
	vOnSucceed();
	pimpl->m_State = ProcessState::Succeeded;
}

void BaseProcess::fail()
{
	//Make sure that the process is not ended.
	assert(!isEnded());

	//Call the virtual method and update the state of the process.
	vOnFail();
	pimpl->m_State = ProcessState::Failed;
}

void BaseProcess::pause()
{
	assert(pimpl->m_State == ProcessState::Running);
	pimpl->m_State = ProcessState::Paused;
}

void BaseProcess::unPause()
{
	assert(pimpl->m_State == ProcessState::Paused);
	pimpl->m_State = ProcessState::Running;
}

bool BaseProcess::isSucceeded() const
{
	return pimpl->m_State == ProcessState::Succeeded;
}

bool BaseProcess::isFailed() const
{
	return pimpl->m_State == ProcessState::Failed;
}

bool BaseProcess::isAborted() const
{
	return pimpl->m_State == ProcessState::Aborted;
}

bool BaseProcess::isEnded() const
{
	return isSucceeded() || isFailed() || isAborted();
}

void BaseProcess::attachChild(std::unique_ptr<BaseProcess> && child)
{
	if (pimpl->m_Child)
		cocos2d::log("BaseProcess::attachChild overwritting an existing child process.");

	pimpl->m_Child = std::move(child);
}

std::unique_ptr<BaseProcess> BaseProcess::detachChild()
{
	return std::move(pimpl->m_Child);
}

const std::unique_ptr<BaseProcess> & BaseProcess::getChild() const
{
	return pimpl->m_Child;
}

void BaseProcess::vOnInit()
{
}

void BaseProcess::vOnSucceed()
{
}

void BaseProcess::vOnFail()
{
}

void BaseProcess::vOnAbort()
{
}
