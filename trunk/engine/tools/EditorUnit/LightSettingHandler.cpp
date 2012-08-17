#include "stdafx.h"
#include "LightSettingHandler.h"
#include "CLightSystem.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

namespace 
{
	CLightSystem* GetLightSystem()
	{
		return CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
	}
}

void LightSettingHandler::SetSceneDirColor( uint32 color )
{
	GetLightSystem()->SetGlobalLightColor(color, 1);
}

void LightSettingHandler::SetSceneAmbColor( uint32 color )
{
	GetLightSystem()->SetAmbient(color, 1);
}

void LightSettingHandler::SetNpcDirColor( uint32 color )
{
	GetLightSystem()->SetGlobalLightColor(color, 0);
}

void LightSettingHandler::SetNpcAmbColor( uint32 color )
{
	GetLightSystem()->SetAmbient(color, 0);
}

void LightSettingHandler::SetDir( const CVector3f& dir )
{
	GetLightSystem()->SetGlobalLightDir(dir);
}