#ifndef __WAR_FIELD_SCRIPT__
#define __WAR_FIELD_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class EvtDataInputTouch;
class EvtDataInputDrag;

class WarFieldScript : public BaseScriptComponent
{
public:
	WarFieldScript();
	~WarFieldScript();

	//Deal with the input event. The return value indicates whether the function swallows the event or not.
	bool onInputTouch(const EvtDataInputTouch & touch);
	bool onInputDrag(const EvtDataInputDrag & drag);

	void loadWarField(const tinyxml2::XMLElement * xmlElement);

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	WarFieldScript(const WarFieldScript &) = delete;
	WarFieldScript(WarFieldScript &&) = delete;
	WarFieldScript & operator=(const WarFieldScript &) = delete;
	WarFieldScript & operator=(WarFieldScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct WarFieldScriptImpl;
	std::shared_ptr<WarFieldScriptImpl> pimpl;
};

#endif // __WAR_FIELD_SCRIPT__
