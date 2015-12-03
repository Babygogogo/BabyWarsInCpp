#ifndef __ActionListScript__
#define __ActionListScript__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class ActionListScript : public BaseScriptComponent
{
public:
	ActionListScript();
	~ActionListScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	ActionListScript(const ActionListScript &) = delete;
	ActionListScript(ActionListScript &&) = delete;
	ActionListScript & operator=(const ActionListScript &) = delete;
	ActionListScript & operator=(ActionListScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ActionListScriptImpl;
	std::unique_ptr<ActionListScriptImpl> pimpl;
};

#endif // __ActionListScript__
