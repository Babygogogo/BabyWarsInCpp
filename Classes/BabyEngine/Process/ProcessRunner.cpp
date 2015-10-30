#include <list>
#include <cassert>

#include "ProcessRunner.h"
#include "BaseProcess.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ProcessRunnerImpl.
//////////////////////////////////////////////////////////////////////////
struct ProcessRunner::ProcessRunnerImpl
{
	ProcessRunnerImpl();
	~ProcessRunnerImpl();

	std::list<std::unique_ptr<BaseProcess>> m_ProcessList;
};

ProcessRunner::ProcessRunnerImpl::ProcessRunnerImpl()
{
}

ProcessRunner::ProcessRunnerImpl::~ProcessRunnerImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ProcessRunner.
//////////////////////////////////////////////////////////////////////////
ProcessRunner::ProcessRunner() : pimpl{ std::make_unique<ProcessRunnerImpl>() }
{
}

ProcessRunner::~ProcessRunner()
{
}

void ProcessRunner::attachProcess(std::unique_ptr<BaseProcess> && process)
{
	assert(process);
	pimpl->m_ProcessList.emplace_back(std::move(process));
}

void ProcessRunner::updateAllProcess(const std::chrono::milliseconds & deltaTimeMs)
{
	//Iterate through the process list.
	//It's possible to remove a process during the iteration, so we must explicitly use iterator.
	auto processIter = pimpl->m_ProcessList.begin();
	while (processIter != pimpl->m_ProcessList.end()){
		//Clone the current iterator for the possible deletion and tick the original one.
		auto currentIter = processIter++;

		//Update the process.
		(*currentIter)->update(deltaTimeMs);

		//If the current process is not ended, just continue updating the next process.
		if (!(*currentIter)->isEnded())
			continue;

		//If the current process is succeeded and it has a child, attach the child to the list.
		if ((*currentIter)->isSucceeded() && (*currentIter)->getChild())
			attachProcess((*currentIter)->detachChild());

		//Remove the process from the list.
		pimpl->m_ProcessList.erase(currentIter);
	}
}

void ProcessRunner::abortAllProcess()
{
	for (auto & process : pimpl->m_ProcessList)
		process->abort();

	pimpl->m_ProcessList.clear();
}
