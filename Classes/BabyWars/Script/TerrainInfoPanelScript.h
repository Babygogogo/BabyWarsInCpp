#ifndef __TERRAIN_INFO_PANEL_SCRIPT__
#define __TERRAIN_INFO_PANEL_SCRIPT__

#include "../../BabyEngine/Actor/BaseScriptComponent.h"

class TerrainInfoPanelScript : public BaseScriptComponent
{
public:
	TerrainInfoPanelScript();
	~TerrainInfoPanelScript();

	//Type name of the class. Used by the ActorFactory and can not be removed.
	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move ctor and operator=.
	TerrainInfoPanelScript(const TerrainInfoPanelScript &) = delete;
	TerrainInfoPanelScript(TerrainInfoPanelScript &&) = delete;
	TerrainInfoPanelScript & operator=(const TerrainInfoPanelScript &) = delete;
	TerrainInfoPanelScript & operator=(TerrainInfoPanelScript &&) = delete;

private:
	virtual bool vInit(const tinyxml2::XMLElement * xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TerrainInfoPanelScriptImpl;
	std::unique_ptr<TerrainInfoPanelScriptImpl> pimpl;
};

#endif // __TERRAIN_INFO_PANEL_SCRIPT__
