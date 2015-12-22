#ifndef __TURN_AND_PLAYER_LABEL_SCRIPT__
#define __TURN_AND_PLAYER_LABEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class TurnAndPlayerLabelScript : public BaseScriptComponent
{
public:
	TurnAndPlayerLabelScript();
	~TurnAndPlayerLabelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	TurnAndPlayerLabelScript(const TurnAndPlayerLabelScript &) = delete;
	TurnAndPlayerLabelScript(TurnAndPlayerLabelScript &&) = delete;
	TurnAndPlayerLabelScript & operator=(const TurnAndPlayerLabelScript &) = delete;
	TurnAndPlayerLabelScript & operator=(TurnAndPlayerLabelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TurnAndPlayerLabelScriptImpl;
	std::shared_ptr<TurnAndPlayerLabelScriptImpl> pimpl;
};

#endif // __TURN_AND_PLAYER_LABEL_SCRIPT__
