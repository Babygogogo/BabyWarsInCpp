#ifndef __BASE_PROCESS__
#define __BASE_PROCESS__

#include <memory>
#include <chrono>

/*!
 * \class BaseProcess
 *
 * \brief The base class of all other processes.
 *
 * \details
 *	Processes are used for anything that takes multiple game loops (moving actors for example).
 *	Inherit from this class and override vOnUpdate() to create any real process you want.
 *	Every process can have no more than one child process.
 *	Processes should be registered to ProcessRunner before they can do any real work.
 *	When registered, the update() will be called on every game loop with the delta time.
 *

 * \author Babygogogo
 * \date 2015.7
 */
class BaseProcess
{
public:
	virtual ~BaseProcess();

	//This method will be called on every game loop if the process is register to ProcessRunner.
	//This method calls vOnUpdate() internally, which you should override to do any real work.
	void update(const std::chrono::milliseconds & deltaTimeMs);

	//Change the state of the process.
	//These methods are mainly called from inside update()/vOnUpdate().
	//You can manually call them if you want to explicitly control the process.
	void init();		//Calls vOnInit().
	void abort();		//Calls vOnAbort().
	void succeed();		//Calls vOnSucceed().
	void fail();		//Calls vOnFail().
	void pause();		//Pause the process if running.
	void unPause();		//Resume the process from pause state.

	//Test the state of the process.
	bool isSucceeded() const;
	bool isFailed() const;
	bool isAborted() const;
	bool isEnded() const;	//Ended means that the process is either succeeded or failed or aborted.

	//Methods for managing child process.
	void attachChild(std::unique_ptr<BaseProcess> && child);	//For simplicity, any process can have no more than one child process. May be updated in the future:)
	std::unique_ptr<BaseProcess> detachChild();					//Detach the child process if any.
	const std::unique_ptr<BaseProcess> & getChild() const;		//Get the child process if any without detaching it.

protected:
	BaseProcess();

	//Override any of these methods as you need in the derived processes.
	virtual void vOnUpdate(const std::chrono::milliseconds & deltaTimeMs) = 0;	//Called from inside update(). You must override this to do the real work of process.
	virtual void vOnInit();							//Called from inside init(). Default behavior is empty. Override this if you need it.
	virtual void vOnSucceed();						//Called from inside succeed(). The same as above.
	virtual void vOnFail();							//Called from inside fail(). The same as above.
	virtual void vOnAbort();						//Called from inside abort(). The same as above.

private:
	//Implementation stuff.
	struct BaseProcessImpl;
	std::unique_ptr<BaseProcessImpl> pimpl;
};

#endif // !__BASE_PROCESS__
