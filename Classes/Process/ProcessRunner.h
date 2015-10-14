#ifndef __PROCESS_INVOKER__
#define __PROCESS_INVOKER__

#include <memory>
#include <chrono>

class BaseProcess;

/*!
 * \class ProcessRunner
 *
 * \brief Manages the processes attached here and runs them.
 *
 * \details
 *	The runner doesn't run the processes automatically.
 *	You must call updateAllProcesses() (probably on every game loop) to run the processes.
 *	If a process ends in success, its child (if any) will be attached to the runner automatically.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class ProcessRunner
{
public:
	ProcessRunner();
	~ProcessRunner();

	//Attach a new process.
	void attachProcess(std::unique_ptr<BaseProcess> && process);

	//Call update() on all processes attached with the eclapsed time.
	//If a process ends in success, its child (if any) will be attached to the runner.
	void updateAllProcess(const std::chrono::milliseconds & deltaTimeMs);

	//Abort all processes attached and detach them.
	void abortAllProcess();

private:
	//Implementation stuff.
	struct ProcessRunnerImpl;
	std::unique_ptr<ProcessRunnerImpl> pimpl;
};

#endif // !__PROCESS_INVOKER__
