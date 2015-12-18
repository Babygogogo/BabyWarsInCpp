#ifndef __PLAYER_MANAGER_SCRIPT__
#define __PLAYER_MANAGER_SCRIPT__

#include "../Utilities/PlayerID.h"
#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class PlayerManagerScript : public BaseScriptComponent
{
public:
	PlayerManagerScript();
	~PlayerManagerScript();

	void loadPlayers(const tinyxml2::XMLElement * xmlElement);

	bool hasPlayerID(PlayerID id) const;
	bool hasPlayerNextOfID(PlayerID id) const;
	PlayerID getFirstPlayerID() const;
	PlayerID getNextPlayerID(PlayerID currentPlayerID) const;

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	PlayerManagerScript(const PlayerManagerScript &) = delete;
	PlayerManagerScript(PlayerManagerScript &&) = delete;
	PlayerManagerScript & operator=(const PlayerManagerScript &) = delete;
	PlayerManagerScript & operator=(PlayerManagerScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct PlayerManagerScriptImpl;
	std::unique_ptr<PlayerManagerScriptImpl> pimpl;
};

#endif // __PLAYER_MANAGER_SCRIPT__
