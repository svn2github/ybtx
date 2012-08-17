#include "stdafx.h"
#include "RegionLightSettingHandler.h"
#include "LightHelper.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"

void RegionLightSettingHandler::SetSceneDirColor( uint32 color )
{
	CSceneEnvMgr::GetInst()->SetSceneDirColor(ToCColor(color));
}

void RegionLightSettingHandler::SetSceneAmbColor( uint32 color )
{
	CSceneEnvMgr::GetInst()->SetSceneAmbientColor(ToCColor(color));
}

void RegionLightSettingHandler::SetNpcDirColor( uint32 color )
{
	CSceneEnvMgr::GetInst()->SetPlayerDirColor(ToCColor(color));
}

void RegionLightSettingHandler::SetNpcAmbColor( uint32 color )
{
	CSceneEnvMgr::GetInst()->SetPlayerAmbientColor(ToCColor(color));
}

void RegionLightSettingHandler::SetDir( const CVector3f& dir )
{	
	CSceneEnvMgr::GetInst()->SetDirDir(dir);
}

