#ifndef __BEGIN_TURN_EFFECT_SCRIPT__
#define __BEGIN_TURN_EFFECT_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class EvtDataInputTouch;

class BeginTurnEffectScript : public BaseScriptComponent
{
public:
	BeginTurnEffectScript();
	~BeginTurnEffectScript();

	bool onInputTouch(const EvtDataInputTouch & touch);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	BeginTurnEffectScript(const BeginTurnEffectScript &) = delete;
	BeginTurnEffectScript(BeginTurnEffectScript &&) = delete;
	BeginTurnEffectScript & operator=(const BeginTurnEffectScript &) = delete;
	BeginTurnEffectScript & operator=(BeginTurnEffectScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct BeginTurnEffectScriptImpl;
	std::shared_ptr<BeginTurnEffectScriptImpl> pimpl;
};

#endif // __BEGIN_TURN_EFFECT_SCRIPT__
