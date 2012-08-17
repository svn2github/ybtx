#include "stdafx.h"
#include "CCtrlImpFactory.h"
#include "LogImp/CLogImp.h"
#include "CCoreViewImp.h"
#include "LightView/CLightViewImp.h"
#include "ColorCtrl/CColorCtrlImp.h"
#include "LightParameterCtrl/CLightParameterCtrlImp.h"
#include "FogParameterCtrl/CFogParameterCtrlImp.h"
#include "HDRSettingCtrl/CHDRSettingCtrlImp.h"
#include "FileProcessCtrl/FileProcessCtrlImp.h"
#include "ProgressCtrl/CProgressCtrlImp.h"
#include "ReadPkgCtrl/ReadPkgCtrlImp.h"
#include "PortraitView/PortraitViewCtrlImp.h"
#include "PathManager/CPathManager.h"
#include "PropCtrl/CPropCtrlImp.h"
//-----------BaseUICtrl------------
#include "BoolEditCtrlImp.h"
#include "NumEditCtrlImp.h"
#include "SingleStringEditCtrlImp.h"
#include "ComBoEditCtrlImp.h"
#include "Vector3EditCtrlImp.h"
#include "NumMapEditCtrlImp.h"
#include "Vector3MapCtrlImp.h"
#include "EStrMapEditCtrlImp.h"
//-----------effect----------------
#include "CEfxFileCtrlImp.h"
#include "CEfxEditPropImp.h"
#include "CEfxTrackCtrlImp.h"
#include "CEfxKeyUintCtrlImp.h"
#include "AutoPropToCtrlImp.h"
//---------------------------------
//------------MapEditor------------
#include "RecordCtrl/CRecordCtrlImp.h"
#include "GridCtrl/CGridCtrlImp.h"
#include "TextureViewCtrl/TextureViewCtrlImp.h"
#include "SetViewCtrl/SetViewCtrlImp.h"
#include "TileViewCtrl/TileViewCtrlImp.h"
#include "MapEffectCtrl/MapEffectCtrlImp.h"
#include "BlockViewCtrl/BlockViewCtrlImp.h"
#include "AudioViewCtrl/AudioViewCtrlImp.h"
#include "AudioViewCtrl/AmbientAudioCtrlImp.h"
#include "AudioViewCtrl/CueListCtrlImp.h"
//------------ModelEditor
#include "PieceGroupEditCtrl/CPieceGroupEditImp.h"
#include "PieceUnitEditCtrl/CPieceUnitEditImp.h"
#include "AnimationGroupEditCtrl/CAnimationGroupEditImp.h"
#include "BoundingBoxEditCtrl/CBoundingBoxEditImp.h"
#include "SkeletonEditCtrl/CSkeletonEditImp.h"
#include "KeyFrameCtrl/CKeyFrameCtrlImp.h"
#include "GroundSpeedCtrl/CGroundSettingCtrlImp.h"
#include "FrameScaleCtrl/CFrameScaleCtrlImp.h"
#include "BoneListCtrl/CBoneListCtrlImp.h"
#include "BoneBoxListCtrl/CBoneBoxListCtrlImp.h"
#include "BoneBoxEditCtrl/CBoneBoxEditCtrlImp.h"
#include "ColorAnimCtrl/CColorAnimImp.h"
//-----------Com-------------------
#include "ContextCtrl/CContextCtrl.h"
#include "CameraCtrl/CRTSCameraCtrl.h"
#include "CameraCtrl/CFPSCameraCtrl.h"
#include "CameraCtrl/CFREECameraCtrl.h"
//---------------------------------
#include "CCallBackCmd.h"

CCtrlBase*  CCtrlImpFactory::CreateImp( char* class_name, void* pParam )
{
	CtrlIDMap::iterator it = st_IDMap.find(class_name);
	if( it != st_IDMap.end() )
	{
		return (*it->second)(pParam);
	}
	return NULL;
}

#define REG_CTRL_IMP(class_name) \
	st_IDMap.insert(CtrlIDMap::value_type(GET_IMP_NAME(class_name),class_name::CreateNewCtrl));

CCtrlImpFactory::CCtrlImpFactory(void)
{
	REG_CTRL_IMP(CLogCtrlImp);
	REG_CTRL_IMP(CSystemLogImp);
	REG_CTRL_IMP(CCoreViewImp);
	REG_CTRL_IMP(CLightViewImp);
	REG_CTRL_IMP(CPropCtrlImp);

	REG_CTRL_IMP(CFileProcessCtrlImp);
	REG_CTRL_IMP(CProgressCtrlImp);

	REG_CTRL_IMP(CColorCtrlImp);
	REG_CTRL_IMP(CLightParameterCtrlImp);
	REG_CTRL_IMP(CFogParameterCtrlImp);
	
	REG_CTRL_IMP(CHDRSettingCtrlImp);
	
	REG_CTRL_IMP(CReadPkgCtrlImp);
	REG_CTRL_IMP(CPortraitViewCtrlImp);

	REG_CTRL_IMP(CPathSetImp);
	
	REG_CTRL_IMP(CRecordCtrlImp);
	REG_CTRL_IMP(CGridCtrlImp);
	//------------------BaseUICtrl---------
	REG_CTRL_IMP(BoolEditCtrlImp);
	REG_CTRL_IMP(NumEditCtrlImp);
	REG_CTRL_IMP(SingleStringEditCtrlImp);
	REG_CTRL_IMP(ComBoEditCtrlImp);
	REG_CTRL_IMP(Vector3EditCtrlImp);
	REG_CTRL_IMP(NumMapEditCtrlImp);
	REG_CTRL_IMP(Vector3MapCtrlImp);
	REG_CTRL_IMP(EStrMapEditCtrlImp);
	//------------------Effect-------------
	REG_CTRL_IMP(CEfxFileCtrlImp);
	REG_CTRL_IMP(CEfxEditPropImp);
	REG_CTRL_IMP(CEfxTrackCtrlImp);
	REG_CTRL_IMP(CEfxKeyUintCtrlImp);
	REG_CTRL_IMP(AutoPropToCtrlImp);

	REG_CTRL_IMP(CTextureViewCtrlImp);
	REG_CTRL_IMP(CSetViewCtrlImp);
	REG_CTRL_IMP(CTileViewCtrlImp);
	REG_CTRL_IMP(CMapEffectCtrlImp);
	REG_CTRL_IMP(CBlockViewCtrlImp);

	REG_CTRL_IMP(CAudioViewCtrlImp);
	REG_CTRL_IMP(CAmbientAudioCtrlImp);
	REG_CTRL_IMP(CCueListCtrlImp);
	//------------------ModelEditor--------
	REG_CTRL_IMP(CPieceGroupEditImp);
	REG_CTRL_IMP(CPieceUnitEditImp);
	REG_CTRL_IMP(CAnimationGroupEditImp);
	REG_CTRL_IMP(CBoundingBoxEditImp);
	REG_CTRL_IMP(CSkeletonEditImp);
	REG_CTRL_IMP(CKeyFrameCtrlImp);
	REG_CTRL_IMP(CGroundSettingCtrlImp);
	REG_CTRL_IMP(CFrameScaleCtrlImp);
	REG_CTRL_IMP(CBoneListCtrlImp);
	REG_CTRL_IMP(CBoneBoxListCtrlImp);
	REG_CTRL_IMP(CBoneBoxEditCtrlImp);
	REG_CTRL_IMP(CColorAnimImp);
	//------------------Com--------
	REG_CTRL_IMP(CContextCtrlImp);
	REG_CTRL_IMP(CRTSCameraCtrl);
	REG_CTRL_IMP(CFPSCameraCtrl);
	REG_CTRL_IMP(CFREECameraCtrl);

	REG_CTRL_IMP(CCallBackCmd);
}
