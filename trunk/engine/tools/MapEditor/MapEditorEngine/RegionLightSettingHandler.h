#pragma once
#include "LightSettingHandler.h"

class RegionLightSettingHandler : public LightSettingHandler
{
public:
	void			SetSceneDirColor(uint32 color);
	void			SetSceneAmbColor(uint32 color);
	void			SetNpcDirColor(uint32 color);
	void			SetNpcAmbColor(uint32 color);
	void			SetDir(const CVector3f& dir);
};