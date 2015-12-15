#ifndef __TURN_MANAGER__
#define __TURN_MANAGER__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class PlayerManagerScript;

class TurnManagerScript : public BaseScriptComponent
{
public:
	TurnManagerScript();
	~TurnManagerScript();

	void loadTurn(const tinyxml2::XMLElement * xmlElement);

	void run();

	void setPlayerManager(const std::shared_ptr<PlayerManagerScript> & playerManagerScript);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	TurnManagerScript(const TurnManagerScript &) = delete;
	TurnManagerScript(TurnManagerScript &&) = delete;
	TurnManagerScript & operator=(const TurnManagerScript &) = delete;
	TurnManagerScript & operator=(TurnManagerScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TurnManagerScriptImpl;
	std::shared_ptr<TurnManagerScriptImpl> pimpl;
};

#endif // __TURN_MANAGER__
