#ifndef __EVENT_DATA_LOAD_WAR_SCENE_END__
#define __EVENT_DATA_LOAD_WAR_SCENE_END__

#include <memory>

#include "../../BabyEngine/Event/BaseEventData.h"

class EvtDataLoadWarSceneEnd : public BaseEventData
{
public:
	EvtDataLoadWarSceneEnd() = default;
	virtual ~EvtDataLoadWarSceneEnd() = default;

	static const EventType s_EventType;
	virtual const EventType & vGetType() const override;

	//Disable copy/move constructor and operator=.
	EvtDataLoadWarSceneEnd(const EvtDataLoadWarSceneEnd &) = delete;
	EvtDataLoadWarSceneEnd(EvtDataLoadWarSceneEnd &&) = delete;
	EvtDataLoadWarSceneEnd & operator=(const EvtDataLoadWarSceneEnd &) = delete;
	EvtDataLoadWarSceneEnd & operator=(EvtDataLoadWarSceneEnd &&) = delete;
};

#endif // !__EVENT_DATA_LOAD_WAR_SCENE_END__
