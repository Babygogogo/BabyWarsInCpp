#ifndef __PLAYER_SCRIPT__
#define __PLAYER_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"
#include "../Utilities/PlayerID.h"

class PlayerScript : public BaseScriptComponent
{
public:
	PlayerScript();
	~PlayerScript();

	void loadPlayer(const tinyxml2::XMLElement * xmlElement);

	PlayerID getID() const;

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	PlayerScript(const PlayerScript &) = delete;
	PlayerScript(PlayerScript &&) = delete;
	PlayerScript & operator=(const PlayerScript &) = delete;
	PlayerScript & operator=(PlayerScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct PlayerScriptImpl;
	std::unique_ptr<PlayerScriptImpl> pimpl;
};

#endif // __PLAYER_SCRIPT__
