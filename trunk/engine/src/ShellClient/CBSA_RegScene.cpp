#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "ScriptX.h"
#include "CSceneConfigClient.h"
#include "CRenderSystem.h"
#include "IModelHandler.h"
#include "CRenderScene.h"
#include "CRenderObject.h"
#include "CObjectDialog.h"
#include "IEffectUnit.h"
#include "CEffectLoader.h"
#include "CEffectPlayer.h"
int32 CBaseScriptApp::RegisterScene(CScript& Script)
{
	REGIST_ABSTRACT_B_CLASS( Script, CSceneConfig );
	
	REGIST_STATICFUNCTION( Script, CSceneConfig , GetInst , ":(CSceneConfig)" );

	//注册CRenderSystem
	REGIST_ABSTRACT_B_CLASS( Script, CRenderSystem );
		
	REGIST_ABSTRACT_B_CLASS( Script, CEffectPlayer );
	//注册CRenderObject
	REGIST_ABSTRACT_D_CLASS_3( Script, CRenderObject, CRenderSpace , CModel ,CEffectPlayer );
	
	//注册CRenderScene
	REGIST_ABSTRACT_B_CLASS( Script, CRenderScene );

	//注册CObjectDialog
	REGIST_D_CLASS_2( Script, CRenderDialog, CRenderSpaceNode, SQRDialog );

	//注册CObjectDialog
	REGIST_D_CLASS_2( Script, CObjectDialog, CRenderSpace, SQRDialog );
	

	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultAntialias, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultShadowType, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultShaderTile, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultWaterType, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultFullScreen, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultVSync, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultBloom, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultRefract, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultMinSpeed, ":f" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultEffectLevel, ":f" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultCameraShake, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultTexQuality, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultTexHMem, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultTexMMen, ":I" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultUIScale, ":f" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultFreeCamera, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultAudioVolume, "s:f" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultVoiceSwitch, ":b" );
	REGIST_CLASSFUNCTION( Script, CSceneConfig , GetDefaultMaxShader, ":I" );

	

	//注册CRenderSystem相关函数
	REGIST_CLASSFUNCTION( Script, CRenderSystem , CreateObjectDialog,	":(CObjectDialog)" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , DestroyObjectDialog, "(CObjectDialog)" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ChangeRenderSettings, "(RenderSettings):b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetRenderSettings, "(RenderSettings)" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , Snapshot, ":s" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , IsBloomEnabled, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetBloomEnabled, "b:b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , IsRefractEnabled, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetRefractEnabled, "b:b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , IsUseShaderTerrain, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetUseShaderTerrain, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetShadowType, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetShadowType, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetWaterType, "I:b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetWaterType, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetAudioVolume, "sf" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetAudioVolume, "s:f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetVoiceSwitch, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetVoiceSwitch, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetEffectLevel, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetEffectLevel, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetCameraShake, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetCameraShake, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetTexQualityLevel, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetTexQualityLevel, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetSelectByCursor, "b");
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetScreenWidth, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetScreenHeight, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , IsInWindowState, ":b" );

	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetBackBufferNum, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetBackBufferNum, ":I" );

	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetUIZoom, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetUIZoom, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetRenderCyc, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetRenderCyc, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetNumResWidths, ":I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , GetResWidthAt, "I:I" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetClearColor, "III" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , Clear, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , Refresh, "" );

	//默认设置


	//系统控制
	//REGIST_CLASSFUNCTION( Script, CRenderSystem , SetAutoLimitFps,	 "b" );
	//REGIST_CLASSFUNCTION( Script, CRenderSystem , GetIsAutoLimitFps, ":b" );

	//#ifdef UseRuntimeDebug
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowFpsSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , IsShowFps, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowDebugSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , LockAspectSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowRenderObjSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowGUISwith, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SwitchHidenMode, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowPlayerSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SwitchIsRender, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , ShowASynLoadSwitch, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetIsRender,"b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystem , SetShowFog,"b" );



	// 注册RenderObject相关的函数

	REGIST_ENUMERATION( Script, RES_TYPE, RFT_ARP );
	REGIST_ENUMERATION( Script, RES_TYPE, RFT_ARA );

	REGIST_CLASSFUNCTION( Script, CEffectPlayer, ClearAllEfx, "" );
	REGIST_CLASSFUNCTION( Script, CEffectPlayer, AddEffect, "ssI(IModelHandler):I" ); // ( const char* szFxFile, const char* szFx, int32 nPlayTime, int32* pRangeTime );
	REGIST_CLASSFUNCTION( Script, CEffectPlayer, DelEffect, "I:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CEffectPlayer, void(CEffectPlayer::*)(uint32,IEffectUnitHandler*,const char*) ,SetEffectUnitHandler, SetEffectUnitHandler, "Its" );


	REGIST_CLASSFUNCTION( Script, CRenderObject, SetModelHandler, "(IModelHandler)" );

	REGIST_CLASSFUNCTION( Script, CRenderObject , CreateRenderDialog,	"Is:(CRenderDialog)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject , DestroyRenderDialog, "(CRenderDialog)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject , CreateRenderObject,	"Is:(CRenderObject)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject , DestroyRenderObject, "(CRenderObject)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, AddPiece, "Isss:b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, ShowPiece, "sb" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetPortraitPiece, "s" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, IsValid, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetAniMaxFrame, "s:i" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, RemovePiece, "s:b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetRenderStyle, "i" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetLapseTime, "I"	 );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetCanBeSelect, "b"	 );

	//REGIST_CLASSFUNCTION( Script, CRenderObject, AddSepSkeletal, "C" );
	//REGIST_CLASSFUNCTION( Script, CRenderObject, DelSepSkeletal, "C" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetDirection, "(CDir)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, AddSkeletal, "s:i" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, ClearAddSke, "" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, Delframework, "" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, RemoveAllPiece, "" );


	//REGIST_CLASSFUNCTION( Script, CRenderObject, GetDirection, ":(CDir)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, void(CRenderObject::*)(CDir&) ,GetDirection, GetDirection, "(CDir)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetRootPosition, "(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetSocketXYZ, "s(CVector3f):b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetSkeletalIdByName, "s:i" );
	//REGIST_CLASSFUNCTION( Script, CRenderObject, GetBox, "(CVector3f)(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetObjHeight, ":f" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, void (CRenderObject::*)(const CVector3f&), SetPosition, SetPosition, "(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetNormal, "(CVector3f)" );

	//REGIST_CLASSFUNCTION( Script, CRenderObject, SetRotationX, "f" );
	//REGIST_CLASSFUNCTION( Script, CRenderObject, SetRotationZ, "f" );
	//REGIST_CLASSFUNCTION( Script, CRenderObject, GetRotationX, ":f" );
	//REGIST_CLASSFUNCTION( Script, CRenderObject, GetRotationZ, ":f" );

	REGIST_CLASSFUNCTION( Script, CRenderObject, SetScale, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetScale, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetAlphaValue, "C" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetAlphaValueEx, "C" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetRenderVelocity, "fi" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetTerrainInfluence, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetAsynLoadPieceSwitch, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, CheckEffect, "ss:I" );
	REGIST_CLASSFUNCTION( Script, CRenderObject, GetRenderTime, ":(IRenderTime)");
	REGIST_CLASSFUNCTION( Script, CRenderObject, SetSize, "f");
	REGIST_CLASSFUNCTION( Script, CRenderObject, Update, "");


	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, bool(CRenderObject::*)(const char*, bool,  float ), DoAni, DoAni, "sbf:b");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, bool(CRenderObject::*)(const char*, int32, bool, float ), DoAni, DoAniSep, "sibf:b");

	//REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, void(CRenderObject::*)(const char*, int32, int32, uint32, int16, int16), DoAni, DoAniBase, "siiIhh");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, void(CRenderObject::*)(int32, int32), StopAni, StopAniPart, "ii");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderObject, void(CRenderObject::*)(int32), StopAni, StopAniSep, "i");


	// 注册CObjectDialog相关的函数
	REGIST_CLASSFUNCTION( Script, CObjectDialog , CreateRenderObject,	":(CRenderObject)" );
	REGIST_CLASSFUNCTION( Script, CObjectDialog , DestroyRenderObject,	"(CRenderObject)" );
	REGIST_CLASSFUNCTION( Script, CObjectDialog, SetUseSceneDepth, "b" );
	REGIST_CLASSFUNCTION( Script, CObjectDialog, SetViewPortrait, "bss" );
	REGIST_CLASSFUNCTION( Script, CObjectDialog, SwitchCamState, "" );

	
	// 注册ObjectWnd相关的函数
	REGIST_CLASSFUNCTION( Script, CRenderDialog, SetOff, "hh" );
	REGIST_CLASSFUNCTION( Script, CRenderDialog, SetUseSceneDepth, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderDialog, SetRenderWndHeight, "i" );



	//注册RenderScene相关函数

	//REGIST_CLASSFUNCTION( Script, CRenderScene , GetMetaScene, ":(CMetaSceneClient)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderScene , void(CRenderScene::*)(CVector3f&), GetSelectedPos, GetSelectedPos, "(CVector3f)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderScene , void(CRenderScene::*)(CVector3f&), GetCameraDest, GetCameraDest, "(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , GetCameraOffset, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetCameraDest, "ff:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRenderScene ,void(CRenderScene::*)(float),SetCameraOffset, SetCameraOffset, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SelectPosByCursor, "ff" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetTimeOfDay, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , IsShowDebugInfo, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , ShowDebugInfo, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , AttachRenderObject, "(CRenderObject)(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , AttachRenderObjectAbs, "(CRenderObject)(CVector3f)" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , IsSelectRenderObj , "ff(CRenderObject):b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetDist, "ff" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetFov, "ff" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetRadian, "ff" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetYOffset, "ff" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetXZDist, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetDistSpeed, "f" );
	
	REGIST_CLASSFUNCTION( Script, CRenderScene, PlayCameraPath, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderScene, SetLogicModelHandler, "s(IModelHandler):b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene, SetLogicModelNextAni, "ssbif:b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene, SetLogicModelEffect,  "sss:b" );
	REGIST_CLASSFUNCTION( Script, CRenderScene, InitSceneRes,  "" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetCameraXRotate, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetCameraYRotate, "f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , GetCameraXRotate, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , GetCameraYRotate, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , GetCameraYDegree, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , GetTerrainHeight, "ff:f" );
	REGIST_CLASSFUNCTION( Script, CRenderScene , SetSelectedObj, "(CRenderObject)" );

	//注册CEffectLoader
	REGIST_B_CLASS( Script, CEffectLoader );
	REGIST_STATICFUNCTION( Script, CEffectLoader, Inst, ":(CEffectLoader)" );

	REGIST_CLASSFUNCTION( Script, CEffectLoader, AddEftGroup, "s" );
	REGIST_CLASSFUNCTION( Script, CEffectLoader, FreeEftGroup, "s" );
	REGIST_CLASSFUNCTION( Script, CEffectLoader, Clear, "" );
	
	return 1;
}
