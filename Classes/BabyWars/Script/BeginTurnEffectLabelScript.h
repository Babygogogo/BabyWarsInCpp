#ifndef __BEGIN_TURN_EFFECT_LABEL_SCRIPT__
#define __BEGIN_TURN_EFFECT_LABEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class BeginTurnEffectLabelScript : public BaseScriptComponent
{
public:
	BeginTurnEffectLabelScript();
	~BeginTurnEffectLabelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	BeginTurnEffectLabelScript(const BeginTurnEffectLabelScript &) = delete;
	BeginTurnEffectLabelScript(BeginTurnEffectLabelScript &&) = delete;
	BeginTurnEffectLabelScript & operator=(const BeginTurnEffectLabelScript &) = delete;
	BeginTurnEffectLabelScript & operator=(BeginTurnEffectLabelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct BeginTurnEffectLabelScriptImpl;
	std::shared_ptr<BeginTurnEffectLabelScriptImpl> pimpl;
};

#endif // __BEGIN_TURN_EFFECT_LABEL_SCRIPT__
