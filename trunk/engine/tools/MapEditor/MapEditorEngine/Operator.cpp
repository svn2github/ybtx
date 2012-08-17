#include "stdafx.h"
#include "Operator.h"
#include "Operator.inl"
#include "WaterOperator.h"
#include "UserMsg.h"
#include "TerrainMesh.h"
#include "Picker.h"
#include "engine.h"
#include "CMapEditApp.h"

#include "Cmd.h"
#include "CMapEditCamera.inl"
#include "ToolSetModel.h"
#include "CMapEditObject.h"
#include "River.h"
#include "TerrainMeshGrid.h"
#include "ModelGroup.h"
#include "IEffectMgrModule.h"
#include "BaseHelper.h"
#include "IoSystem.h"
#include "OperationLog.h"
#include "CCameraManager.h"
#include "CMapEditModelObjPro.h"
#include "Console.h"
#include "OperatorProperty.h"
#include "EditStateOpertiaonMgr.h"
#include "CEditTerrainMgr.h"
#include "SceneAudioMgr.h"
#include "SceneAudio.h"
#include "CEditDataScene.h"
#include "CEditTerrainMgr.h"
#include "CEventState.h"

float 	STDYAW;		
float 	STDPITCH;	
float 	STDDISTANCE;	

//////////////////////////////////////////////////////////////////////////

namespace sqr
{
	COperator::COperator(void)
	{
		pOperatorPropertyCtrl = new COperatorPropertyCtrl;
		
		m_bShowEffectBase = true;
		m_bReceiveShadow  = false;

		pOperatorPropertyCtrl->bSnapMode = false;

		m_bRebirthResult = false;//%%%%%

		m_bPreViewModel = false;

		m_pTitleEffect = NULL;

		m_bLoadModel = false;
		m_bCliffLayerOne = false;

		m_bCreateDynamicLight = false;
		m_bRebirthEffectSuc = true;

		m_bPreviewTerrainBaking = false;
		m_bShowSpecialGridCueMat = false;

		m_bCurSetSpotLight = false;
		m_bMoveSpotLightTargetPos = false;

		m_bSceneLightEffectTerrain = false;

		m_bReSizeMap	= false;

		m_bUseSpecular = true;
		m_bSetVertexDiffuseColor = true;

		m_bRgnAmbientFxBase = false;
		m_bShowWireBox		= false;
	}

	COperator::~COperator() 
	{
		m_pTitleEffect = NULL;
		SafeDelete(pOperatorPropertyCtrl);

		m_vecIndividualEffectObjects.clear();

		//CEditStateOpertiaonMgr::Destroy();
		//CSceneAudioMgr::Destroy();
		//CPicker::Destroy();
		//COpearationLog::Destroy();
	}

	void COperator::SetWireFrame( bool b )
	{
		CMapEditApp::GetInst()->GetTerrain()->m_pTerrain->m_Param.m_RS.m_FillMode =3 - b;
	}

	bool COperator::GetWireFrame()
	{
		return CMapEditApp::GetInst()->GetTerrain()->m_pTerrain->m_Param.m_RS.m_FillMode == 2;
	}

	void COperator::Create( HWND hMainFrame, HWND hResSetView, HWND hParamsView, 
							HWND hParamsExtraView, HWND hTextureView, HWND hMeshView, 
							HWND hFXSetView, HWND hVertexView, HWND hLoginView, 
							HWND hRecordView, HWND hAudioView, HWND hRegionView, HWND hTileRegionView)
	{
		//////////////////////////////////////////////////////////////////////////
		LogConsole("binding modules...");

		LogConsole("creating graphicmgr...");
		CGraphic::GetInst()->SetAsynSwitch(false);

		// save gui data
		pOperatorPropertyCtrl->hMainFrame		= hMainFrame;
		pOperatorPropertyCtrl->hResSetView		= hResSetView;
		pOperatorPropertyCtrl->hFXSetView		= hFXSetView;
		pOperatorPropertyCtrl->hParamsView		= hParamsView;
		pOperatorPropertyCtrl->hParamsExtraView	= hParamsExtraView;
		pOperatorPropertyCtrl->hTextureView		= hTextureView;
		pOperatorPropertyCtrl->hMeshView		= hMeshView;
		pOperatorPropertyCtrl->hVertexView		= hVertexView;
		pOperatorPropertyCtrl->hLoginSetView	= hLoginView;
		pOperatorPropertyCtrl->hRecordView		= hRecordView;
		pOperatorPropertyCtrl->hAudioView		= hAudioView;
		pOperatorPropertyCtrl->hRegionView		= hRegionView;
		pOperatorPropertyCtrl->hTileRegionView  = hTileRegionView;

		// 逻辑
		CSceneAudioMgr::Create();
		CPicker::Create();
		COpearationLog::Create();
						
		::SendMessage(hMeshView,WM_MESHVIEW_INIT,0,0);
		::SendMessage(hVertexView,WM_VERTEXVIEW_INIT,0,0);

		LogConsole("creating operator...finished");
	}

	void COperator::PickHotGrid()
	{
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		CTObject* p = NULL;
		CVector3f v;

		DWORD dwGridIndexHot = CPicker::GetInst()->GetPickPos(&p,&v);

		if( pTerrainMesh->IsValid(dwGridIndexHot) )
			CPicker::GetInst()->PickSceneObject(dwGridIndexHot);

		if ( pTerrainMesh->IsValid(dwGridIndexHot) )
		{
			SGrid & grid = pTerrainMesh->GetGrid(dwGridIndexHot);

			if ( int(v.x) % int(GRID_SPACE) <= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) <= GRID_SPACE/2 )
			{
				this->SetVertexIndexHot(grid.dwVertexIndex[0]);
			}
			else if ( int(v.x) % int(GRID_SPACE) <= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) >= GRID_SPACE/2 )
			{
				this->SetVertexIndexHot(grid.dwVertexIndex[3]);
			}
			else if ( int(v.x) % int(GRID_SPACE) >= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) <= GRID_SPACE/2 )
			{
				this->SetVertexIndexHot(grid.dwVertexIndex[1]);
			}
			else if ( int(v.x) % int(GRID_SPACE) >= GRID_SPACE/2 && int(v.z) % int(GRID_SPACE) >= GRID_SPACE/2 )
			{
				this->SetVertexIndexHot(grid.dwVertexIndex[2]);
			}

			// valid only and only if the gridindexhot is valid
			this->SetGridIndexHot(dwGridIndexHot);
			this->SetHitPoint(*((CVector3f*)&v));	

			CEditStateOpertiaonMgr::GetInst()->SetHotGridIndex(dwGridIndexHot);
		}

		this->SetObjectHot(p);

		::SetFocus(CEngine::GetInst()->GetD3DWindow());
	}

	void COperator::OnEvent(const CMsg& msg)
	{
		switch ( msg.msgSource )
		{
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_MOVE:
					{
						this->PickHotGrid();
					}
					break;
				}
				break;
			}
		case MGS_KEYMSG:
			{

				if ( ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN ) )
				{
					switch(msg.msgInfo.key_info)
					{
						///隐藏effect盒子
					case MGI_KEY_H:
						{
							static bool bShowEffect = true;
							bShowEffect = !bShowEffect;

							if ( bShowEffect )
								this->ShowAll();
							else
								this->HideAll();

							break;
						}

					case MGI_KEY_F1:
						CSceneAudioMgr::GetInst()->SwitchSoundPlay();
						return;

					case MGI_KEY_F2:
						CSceneAudioMgr::GetInst()->SwitchSoundObjectsShow();
						return;

						return;
					}

					default:
						break;
				}


				switch(msg.msgInfo.key_info)
				{
					// 隐藏纹理框和字体
				case MGI_KEY_J:
					this->SwitchSnapMode();
					break;

 					// 隐藏显示水开关
 				case MGI_KEY_U:
					{
						bool bVisualizeWaterBase = CMapEditApp::GetInst()->GetWaterOperator()->GetDoesVisualizeWaterBase();
						CMapEditApp::GetInst()->GetWaterOperator()->SetDoesVisualizeWaterBase(!bVisualizeWaterBase);
					}
 					return;

					///？问号，切换地表线框模式
				case MGI_KEY_Question:
					{
						this->SetWireFrame(!this->GetWireFrame());
						return;
					}

				default:
					break;
				}
				break;
			}
		}

		CEditStateOpertiaonMgr::GetInst()->OnEvent(msg);
		CSceneAudioMgr::GetInst()->UpdateAudio();
	}

	void COperator::InitProcessParams(bool bFirst, DWORD dwVersion)
	{
		return ;
		STDYAW		= DT1_STDYAW;
		STDPITCH	= DT1_STDPITCH;
		STDDISTANCE	= DT1_STDDISTANCE;

		m_processParams.dwGridIndexHot	 = 0;
		m_processParams.pObjectHot		 = NULL;
		m_processParams.dwVertexIndexHot = 0;
		
		m_bShowEffectBase = true;
		
		::SendMessage(pOperatorPropertyCtrl->hParamsView,WM_PARAMSVIEW_INIT,0,0);

		string path  = IoSystem::GetInst()->GetFullPath("home:Log");
		bool   bExit = IoSystem::GetInst()->DirectoryExists(path);
		if( !bExit )
			IoSystem::GetInst()->CreateDirectory(path);

		string mapname = CMapEditApp::GetInst()->GetDataScene()->GetSceneName();
		if( mapname == "" )
			mapname = CEditTerrainMgr::st_DefaultMapName;

		COpearationLog::GetInst()->SetLogFileName(mapname.c_str(), path.c_str());

		path  = IoSystem::GetInst()->GetFullPath("home:Auto");
		bExit = IoSystem::GetInst()->DirectoryExists(path);
		if( !bExit )
			IoSystem::GetInst()->CreateDirectory(path);

		CEditStateOpertiaonMgr::GetInst()->Initialize(bFirst, pOperatorPropertyCtrl->hMainFrame, pOperatorPropertyCtrl->hParamsView, pOperatorPropertyCtrl->hTextureView,
			pOperatorPropertyCtrl->hVertexView, pOperatorPropertyCtrl->hMeshView, pOperatorPropertyCtrl->hResSetView, pOperatorPropertyCtrl->hLoginSetView, pOperatorPropertyCtrl->hRegionView, pOperatorPropertyCtrl->hTileRegionView);
	
		CMapEditApp::GetInst()->GetWaterOperator()->SetTextureHwnd(pOperatorPropertyCtrl->hTextureView);
	}

	void COperator::MoveTo(CTObject * p)
	{
		int nGridIndex = p->GetCenterGridIndex();
		if ( CMapEditApp::GetInst()->GetEditingMesh()->IsValid(nGridIndex) ) 
		{
			SGrid & grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nGridIndex);
			
			CCameraManager::GetInst()->GetMapEditCamera()->MoveToRTSByView(grid.vCenter);
			CToolSetModel::GetInst()->SetLastLockedObject(p);
		}
	}

	int COperator::MoveTo(int x, int z)
	{
		int nGridIndex = CMapEditApp::GetInst()->GetEditingMesh()->GetWidth()*z + x;
		
		if ( CMapEditApp::GetInst()->GetEditingMesh()->IsValid(nGridIndex) ) 
		{
			SGrid & grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nGridIndex);
			CCameraManager::GetInst()->GetMapEditCamera()->MoveToRTSByView(grid.vCenter);
		}

		return nGridIndex;
	}

	void COperator::GetMapCoordinate(const int nGridIndex, int& nX, int& nZ)
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain )
		{
			int nTerrainWidth = pTerrain->GetWidth();

			nX = nGridIndex % nTerrainWidth; // 列
			nZ = nGridIndex / nTerrainWidth; // 行
		}
	}

	void COperator::CreateActiveObject(BYTE bResGroupMark, bool bVisibleByConfigure, bool bShadow, bool bTranslucent, string strItemName, vector<string> & veCMapEditModelFileNames, bool bSubUnit, sqr::ECenter_Type eCenterType, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation, int nRotationCnt, 
									   float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float xScale, float yScale, float zScale, CVector3f vGroupOffset, bool bCameraOriented, bool bEffectBase, bool bAudio, bool bLogin)
	{
		CToolSetModel::GetInst()->CreateActiveObject(bResGroupMark, bVisibleByConfigure, bShadow, bTranslucent, strItemName, veCMapEditModelFileNames, bSubUnit, eCenterType, fLift, fShiftX, fShiftZ, fLiftEx, fShiftXEx, fShiftZEx, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale,
											vGroupOffset, bCameraOriented, bEffectBase, bAudio, bLogin);
	}

	bool COperator::Rebirth(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,
							CVector3f * pV, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart, float fPlus, sqr::EAUTO_GENERATION_TYPE e, int nRenderStyle,int nAtten, bool bVisibleByConfigure,bool bShadow,bool bTrans,bool bZTest, bool bSelfZTest,bool bExport,string strResSetName)
	{
 		::SendMessage(pOperatorPropertyCtrl->hResSetView,WM_RESSETVIEW_REBIRTH,(WPARAM)(strItemName+"@"+strResSetName).c_str(),(LPARAM)pV);
 
 		if(this->GetRebirthResult() == false) 
 			return false;
 
		CVector3f pos= CVector3f(0.0f, 0.0f, 0.0f);
 		if( !CToolSetModel::GetInst()->Rebirth(nGridIndex, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ, pos,
 			nAniStart, fPlus, e, nRenderStyle,nAtten,bVisibleByConfigure,bShadow,bTrans,bZTest,bSelfZTest,bExport) )
 			return false;

		return true;
	}

	bool COperator::Rebirth(const MoldeSaveProperty* pModelSavePro, const string& strItemName, const string strResSetName, int nAffectRatio, const string strCurItemName, const string strCurName )
	{
		CVector3f * pV = NULL;
		CVector3f vUserOffsetEx2(pModelSavePro->vUserOffsetEx.y,pModelSavePro->vUserOffsetEx.x,pModelSavePro->vUserOffsetEx.z);
		pV = &vUserOffsetEx2;
		::SendMessage(pOperatorPropertyCtrl->hResSetView,WM_RESSETVIEW_REBIRTH,(WPARAM)(strItemName+"@"+strResSetName).c_str(),(LPARAM)pV);

		if(this->GetRebirthResult() == false) 
			return false;

 		if( !CToolSetModel::GetInst()->Rebirth(pModelSavePro, nAffectRatio, strCurName) )
 			return false;

		return true;
	}

	bool COperator::Rebirth(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,CVector3f * pV, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart, float fPlus, 
							sqr::EAUTO_GENERATION_TYPE e, int nRenderStyle,int nAtten, bool bVisibleByConfigure,bool bShadow,bool bTrans, BYTE bTransState, int nMirrorType, int nAffectRatio,string strResSetName,string strCurItemName,string strCurName)
	{
		::SendMessage(pOperatorPropertyCtrl->hResSetView,WM_RESSETVIEW_REBIRTH,(WPARAM)(strItemName+"@"+strResSetName+"#"+strCurItemName).c_str(),(LPARAM)pV);

		if(this->GetRebirthResult() == false) 
			return false;

		if( !CToolSetModel::GetInst()->Rebirth(nGridIndex, bTransState,nMirrorType,nAffectRatio, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ,
			nAniStart, fPlus, e, nRenderStyle,nAtten,bVisibleByConfigure,bShadow,bTrans, strCurName) )
			return false;

		return true;
	}

	bool COperator::RebirthLogin(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ, 
		sqr::EAUTO_GENERATION_TYPE e, int nRenderStyle,int nAtten, bool bVisibleByConfigure,bool bShadow,bool bTrans, BYTE bTransState, int nMirrorType, int nAffectRatio)
	{
		::SendMessage(pOperatorPropertyCtrl->hLoginSetView,WM_LOGINVIEW_REBIRTH,(WPARAM)(strItemName).c_str(),0);

		if( !CToolSetModel::GetInst()->Rebirth(nGridIndex, bTransState,nMirrorType,nAffectRatio, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ,
			0, 1.0f, e, nRenderStyle,nAtten,bVisibleByConfigure,bShadow,bTrans) )
			return false;

		return true;
	}

	void COperator::CopyRebirth(int nGridIndex, CEditModelGroup *pModelGroup, sqr::EAUTO_GENERATION_TYPE e, CVector3f vGroupOffset)
	{
		if( pModelGroup == NULL )
			return;

 		bool bSubUnit = false, bVisible = false, bShadow = false, bReceiveShadow = false, bTrans = false, bZTest = false;
		bool bSelfZTest = false, bCameraOriented = false, bEffectBase = false, bExport = false;
		
		string strItemName = "", aniName = "", areName = "", strCueName(""), effectname("");
		vector<string> veCEditModelFileNames;
		
		vector< vector<sqr::SBlockInfo> > vvBlock;
		vector< vector<sqr::SLogicHeightInfo> > vvHeight;
	 	
		sqr::ECenter_Type eCenterType; 
	 
		float fLift, fShiftX, fShiftZ, fLiftEx, fShiftXEx, fShiftZEx,fRotation, fPitch, fYaw, scaleX, scaleY, scaleZ;
		
		int nRotationCnt = 0, nPitchCnt = 0, nYawCnt = 0, nAniStart = 0, nRenderStyle = 0, nAtten = 0;

		//derive face value
		BYTE bResGroupMark	= pModelGroup->GetResGroupMark();
		bSubUnit		= pModelGroup->GetIsSubUnit();
		bVisible	    = pModelGroup->GetShadowSecondPass();
		bShadow			= pModelGroup->IsCastShadow();
		bReceiveShadow  = pModelGroup->IsReceiveShadow();
		bTrans			= pModelGroup->GetIsTranslucent();
		bCameraOriented = pModelGroup->GetIsCameraOriented();
		bEffectBase		= pModelGroup->GetIsEffectBased();
		bool bLogin		= false;
		bool bAudio     = pModelGroup->m_pSceneAudio != NULL;

		strItemName		= pModelGroup->GetItemName();
		aniName			= pModelGroup->GetAgpFileName();
		pModelGroup->GetEffectName(areName);
		veCEditModelFileNames = pModelGroup->GetModelFileNames();
		strCueName		= pModelGroup->GetSceneCueName();
		bool bEffect    = pModelGroup->GetEffectFileName(effectname);

		size_t size = pModelGroup->m_vecSelfPresentation.size();

		eCenterType = pModelGroup->GetCenterType();

		fLift = pModelGroup->GetLiftx();
		fShiftX = pModelGroup->GetShiftX();
		fShiftZ = pModelGroup->GetShiftZ();

		fLiftEx = pModelGroup->GetLiftEx();
		fShiftXEx = pModelGroup->GetShiftXEx();
		fShiftZEx = pModelGroup->GetShiftZEx();

		fPitch = pModelGroup->GetPitch(); nPitchCnt = pModelGroup->GetPitchCount();
		fRotation = pModelGroup->GetRotation(); nRotationCnt = pModelGroup->GetRotationCount();
		fYaw = pModelGroup->GetYaw(); nYawCnt = pModelGroup->GetYawCount();

		scaleX = pModelGroup->GetScaleX(); scaleY = pModelGroup->GetScaleY(); scaleZ = pModelGroup->GetScaleZ();

		nRenderStyle = pModelGroup->GetRenderStyle();
		nAtten = pModelGroup->GetAtten();

		//born a model
		this->CreateActiveObject(bResGroupMark, bVisible, bShadow, bTrans, strItemName, veCEditModelFileNames, 
				bSubUnit, eCenterType, fLift, fShiftX,fShiftZ, fLiftEx, fShiftXEx, fShiftZEx, 
				fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ, 
				vGroupOffset,bCameraOriented,bEffectBase, bAudio, bLogin);

		CTObject *pTObject =NULL;
		pTObject = CToolSetModel::GetInst()->GetLastActiveObject();
		if( pTObject != NULL )
		{
			CEditModelGroup *pCopyModelGroup = NULL;
			pCopyModelGroup = pTObject->GetModelGroup();
			if( pCopyModelGroup == NULL )
				return;

			pCopyModelGroup->SetReceiveShadow(bReceiveShadow);

			//bind ani
			if ( aniName != "" )
				pCopyModelGroup->SetAnimationByAgpFileName(aniName);

			//bind audio
			if( bAudio )
			{
				pCopyModelGroup->m_pSceneAudio = new SceneAudio;
				pCopyModelGroup->SetSceneCueName(strCueName);
			}
		}

		//bind effect
		//模型+特效
		if ( !bEffectBase && !effectname.empty() )
		{
			::SendMessage(pOperatorPropertyCtrl->hResSetView,WM_RESSETVIEW_REBIRTHEFFECT,(WPARAM)areName.c_str(),0);
		}
		else
		{
			if( bEffectBase && !pModelGroup->GetIsLightEffect() ) 
				::SendMessage(pOperatorPropertyCtrl->hResSetView,WM_RESSETVIEW_REBIRTHEFFECT,(WPARAM)areName.c_str(),0);
		}

		this->SetRebirthResult(true);
	}

	void COperator::RebirthIndividualEffect(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,
											CVector3f * pV, const CVector3f& pos, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart, float fPlus, sqr::EAUTO_GENERATION_TYPE e, int nRenderStyle,int nAtten, 
											bool bVisible, bool bShadow, bool bTrans, bool bZTest, bool bSelfZTest, bool bExport, string strResSetName)
	{
		CVector3f * pNewV = NULL;
		CVector3f vUserOffsetEx2(pV->y,pV->x,pV->z);
		pNewV = &vUserOffsetEx2;

		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REBIRTH,(WPARAM)strItemName.c_str(),(LPARAM)pNewV);

		if( m_bRebirthEffectSuc )
		{
			CToolSetModel::GetInst()->Rebirth(nGridIndex, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ, pos,
				nAniStart, fPlus, sqr::EAGT_NONE);
		}
	}

	void COperator::RefreshEffectViewSceneLightSliderState( float vRang, float vAttenuatuin0, float vAttenuatuin1, float vAttenuatuin2, byte vBrightTime, byte vDarkTime, byte vChangeTime, byte vChangeProbability, bool vDynamic)
	{
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_RANG, 0,(LPARAM)&vRang);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION0, 0,(LPARAM)&vAttenuatuin0);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION1, 0,(LPARAM)&vAttenuatuin1);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION2, 0,(LPARAM)&vAttenuatuin2);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_BRIGHTTIME, 0,  (LPARAM)&vBrightTime);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_DARKTIME, 0,    (LPARAM)&vDarkTime);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_CHANGETIME, 0,  (LPARAM)&vChangeTime);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_CHANGEPROBABILITY, 0,(LPARAM)&vChangeProbability);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_DYNAMIC, 0,(LPARAM)&vDynamic);
	}

	void COperator::RefreshAmbientAudioPro()
	{
		::SendMessage(pOperatorPropertyCtrl->hAudioView ,WM_AUDIOVIEW_SELECT, 0 ,0);
	}

	void COperator::RefreshEffectViewSpotLightSliderState( float fRang, float fAttenuatuin0, float fAttenuatuin1, float fAttenuatuin2, float fFallOff, float fTheta, float fPhi)
	{
		fFallOff *= 0.1f;
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_RANG,		  0,(LPARAM)&fRang);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION0, 0,(LPARAM)&fAttenuatuin0);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION1, 0,(LPARAM)&fAttenuatuin1);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSCENELIGHTSLIDER_ATTENUATION2, 0,(LPARAM)&fAttenuatuin2);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSPOTLIGHTSLIDER_FALLOFF,		  0,(LPARAM)&fFallOff);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSPOTLIGHTSLIDER_THETA,		  0,(LPARAM)&fTheta);
		::SendMessage(pOperatorPropertyCtrl->hFXSetView,WM_FXSETVIEW_REFRESHSPOTLIGHTSLIDER_PHI,			  0,(LPARAM)&fPhi);
	}

	void COperator::ToolSetTitleModel_LinkEffect(string str, void *p)
	{
		if ( m_pTitleEffect )
		{
			if ( m_pTitleObject ) 
				m_pTitleObject->GetModelGroup()->TitleUnlinkEffect();

			m_pTitleEffect->Release();
		}

		IEffect * pEffect		= NULL;
		EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;
		IEffectManager::GetInst()->CreateEffect((IEffectGroup*)p, str.c_str(), &pEffect);
		if ( pEffect )
		{
			m_pTitleEffect = pEffect;
			if ( m_pTitleObject )
			{
				m_pTitleObject->GetModelGroup()->TitleLinkEffect(m_pTitleEffect,(IEffectGroup*)p);
			}
		}
		else
		{
			string log = str + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}

	void COperator::HideAll()
	{
		// INDIVIDUAL
		for ( size_t i = 0; i < m_vecIndividualEffectObjects.size(); ++i )
		{
			CTObject * pT = m_vecIndividualEffectObjects[i];
			pT->GetModelGroup()->HideSelf();
		}
	}

	void COperator::ShowAll()
	{
		// INDIVIDUAL
		for ( size_t i = 0; i < m_vecIndividualEffectObjects.size(); ++i )
		{
			CTObject * pT = m_vecIndividualEffectObjects[i];
			pT->GetModelGroup()->ShowSelf();
		}
	}

	//------------------------------------------------------------------------------
	void COperator::DrawTextInfo()
	{
 		if ( !this->GetSnapMode() )
 		{
			CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
			int nHotGridIndex = this->GetGridIndexHot();
			if ( pTerrain == NULL || !pTerrain->IsValid(nHotGridIndex) )
				return;

			SGrid& grid		  = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nHotGridIndex);

			int nX = 0, nZ = 0;
			this->GetMapCoordinate(nHotGridIndex, nX, nZ);
			string strMaterialName = "";

			strMaterialName = CSceneAudioMgr::GetInst()->GetMaterialName(grid.nMaterialIndex);

			DebugText(0xff00ff00, "当前格子索引：%d, 横向：%d, 纵向：%d, mat: %s, 保存地图状态是", nHotGridIndex, nX, nZ, strMaterialName.c_str());
			DebugText(0xff00ff00, "当前格子材质名：%s, %d, 是否为强制行材质索引: %d", strMaterialName.c_str(), grid.nMaterialIndex, grid.bMatNotEffectByTileSets);
			DebugText(0xff00ff00, "当前格子顶点色是否影响物件：%d, 阻挡类型: %d", grid.bEffectByVertexColor, grid.nBlockType);

			int nSubRgnIndex = grid.m_nDivideRegionIndex;
			int nRegionIndex = grid.m_nRegionIndex;
			int nRegWidth = pTerrain->GetWidthInRegion();
			nX = nRegionIndex % nRegWidth; // 列
			nZ = nRegionIndex / nRegWidth; // 行
			nX = nX / 4;
			nZ = nZ / 4; 

			DebugText(0xff00ff00, "当前区域 %d, 细分区域 %d, lightmap: %d_%d, %d, %d", nRegionIndex, nSubRgnIndex, nX, nZ, grid.dwBufferIndex, grid.bSubRenderUnitIndex);


			CTObject *pObject = this->GetObjectHot();
  			if ( pObject )
  			{
  				vector<string> veCMapEditModelNames;
  				string strEffectName = "", strModelName = "", strEffectFileName = "";
	  
  				CEditModelGroup *pModelGroup = NULL;
  				pModelGroup = pObject->GetModelGroup();
	  
  				if( pModelGroup != NULL )
  				{
					pModelGroup->GetEffectFileName(strEffectFileName);
  					pModelGroup->GetEffectName(strEffectName);
  					veCMapEditModelNames = pModelGroup->GetModelFileNames();
					string strAniName = pModelGroup->GetAnimationGroupName();
  					strModelName = veCMapEditModelNames[0];
					int nCenterGridIndex = pModelGroup->GetCenterGridIndex();
					bool bShadow = pModelGroup->IsCastShadow();
					bool bRecieveShadow = pModelGroup->IsReceiveShadow();
					bool bPlayAni = pModelGroup->GetIsPlayAnimation();
					bool bUseCameraCollision = pModelGroup->GetIsUseCameraCollison();
					int nRenderstyle = pModelGroup->GetRenderStyle();

					float fRotateY = pModelGroup->GetRotation();
					float fRotateZ = pModelGroup->GetPitch();
					float fRotateX = pModelGroup->GetYaw();
					float fScaleX  = pModelGroup->GetScaleX();
					float fScaleY  = pModelGroup->GetScaleY();
					float fScaleZ  = pModelGroup->GetScaleZ();
					CMapEditObject *pMapEditObject = NULL;
					pMapEditObject = pModelGroup->GetSelfPresentation(0);
					CVector3f pos = pMapEditObject->sModelSavePro.vPosition;
					int   nMirrorType = pModelGroup->GetMirrorType();
					bool  bEffectByVertexColor = grid.bEffectByVertexColor;
	  
					CEditStateOpertiaonMgr::EEDIT_STATE eState = CEditStateOpertiaonMgr::GetInst()->GetEditState();

   					if(eState == CEditStateOpertiaonMgr::EES_SET_TILE_FACE)
  					{
						DebugText(0xff00ff00, "热点物件: %s 衰减值: %d, 物体是否受顶点色影响:%d, 镜像: %d, 渲染状态: %d", strModelName.c_str(), pModelGroup->GetAffectRatio(), bEffectByVertexColor, nMirrorType, nRenderstyle);		
  					}
   					if( pModelGroup->m_SL.m_bIAmbScenePointLight )
  					{
						DebugText(0xff00ff00, "点光源是否影响地表: %d", pModelGroup->m_SL.m_bEffectTerrain);	
  					}
					if( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
					{
						CVector3f target = pModelGroup->m_SpotLight.m_TargetPos;
						DebugText(0xff00ff00, "聚光灯是否影响地表: %d, 目标点:%f, %f, %f", pModelGroup->m_SpotLight.m_bEffectTerrain, target.x, target.y, target.z);	
					}

					if ( pModelGroup->m_pSceneAudio )
					{
						string strCueName = pModelGroup->GetSceneCueName();
						DebugText(0xff00ff00, "音源名字: %s ", strCueName.c_str());
					}
	  

					if( strAniName != "" )
					{
						DebugText(0xff0000ff, "动作名: %s, 动画是否播放: %d", strAniName.c_str(), bPlayAni);
					}

					if( strEffectName != "" )
					{
						DebugText(0xff00ff00, "特效名: %s, 文件: %s", strEffectName.c_str(), strEffectFileName.c_str());
					}

					DebugText(0xff00ff00, "xyz旋转: %f, %f, %f, zyz缩放：%f, %f, %f, %f, %f, %f", fRotateX, fRotateY, fRotateZ, fScaleX, fScaleY, fScaleZ, pos.x, pos.y, pos.z);	
					DebugText(0xff00ff00, "热点物件中心格子：%d, 是否阴影: %d, 是否接收阴影: %d , CameraCollision: %d", nCenterGridIndex, bShadow, bRecieveShadow, bUseCameraCollision);				
  				}
  			}
	  

			float fHeight = CEditStateOpertiaonMgr::GetInst()->GetMeshLogicHeight();	
			DebugText(0xffff00ff, "逻辑高度 : %f", fHeight);
	  
  			float fFOV = CCameraManager::GetInst()->GetActiveCamera()->GetFovY();
  			float fDis = CCameraManager::GetInst()->GetActiveCamera()->GetDistance();
  			float fPitch = CCameraManager::GetInst()->GetActiveCamera()->GetPitch();
  			float fYaw = CCameraManager::GetInst()->GetActiveCamera()->GetYaw();
			CVector3f vCamPos = CCameraManager::GetInst()->GetActiveCamera()->GetNewPosition();
			CVector3f vCamTgt = CCameraManager::GetInst()->GetActiveCamera()->GetNewTarget();
			DebugText(0xff00ff00, "摄像机位置 : x: %.2f  y: %.2f  z: %.2f", 
												   vCamPos.x, vCamPos.y, vCamPos.z);
			DebugText(0xff00ff00, "摄像机目标 : x: %.2f  y: %.2f  z: %.2f",
												   vCamTgt.x, vCamTgt.y, vCamTgt.z);
			DebugText(0xff00ff00, "摄像机FOV : %.2f Dis : %.2f Pitch : %.2f Yaw : %.2f", fFOV, fDis, fPitch, fYaw);
		}
	}

	CTObject * COperator::GetLastAmbientObject()
	{
		if( m_vecAmbientObjectsPairs.empty() )
			return NULL;
		else
		{
			m_vecAmbientObjectsPairs.back().first->AddRef(); //!!! grab in case lost
			return m_vecAmbientObjectsPairs.back().first;
		}
	}

	void COperator::ReMoveAmbientObject(CTObject * p)
	{
		pair<CTObject *,bool> temp = make_pair(p ,true);
		m_vecAmbientObjectsPairs.erase(
		remove(m_vecAmbientObjectsPairs.begin(), m_vecAmbientObjectsPairs.end(), temp), m_vecAmbientObjectsPairs.end() );
	}

	void COperator::HideObject( void * p )
	{
		for ( size_t i = 0; i < m_vecAmbientObjectsPairs.size(); ++i )
		{
			pair<CTObject *,bool> & pT = m_vecAmbientObjectsPairs[i];
			
			if ( pT.first == p ) 
			{
				pT.second = false;
				pT.first->GetModelGroup()->SetEditState(sqr::EES_DELETE);

				this->ReMoveAmbientObject(pT.first);
			}
		}
	}

	void COperator::ShowObject( void * p )
	{
		for ( size_t i = 0; i < m_vecAmbientObjectsPairs.size(); ++i )
		{
			pair<CTObject *,bool> & pT = m_vecAmbientObjectsPairs[i];
			if ( pT.first == p ) 
			{
				pT.second = true;
				pT.first->GetModelGroup()->SetVisible(true);
			}
		}
	}

	void COperator::MapBackPicMoveInfo()
	{
		::SendMessage(pOperatorPropertyCtrl->hParamsExtraView,WM_PARAMEXTRAVIEW_MOVEINFO,0,0);
	}

	void COperator::CreateTitleActiveObject(vector<string> & veCTileEditorModelFileNames, vector< vector<sqr::SBlockInfo> > & vvBlock, vector< vector<sqr::SLogicHeightInfo> > & vvHeight, bool bSubUnit, sqr::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ)
	{
		 CToolSetModel::GetInst()->CreateTitleActiveObject(veCTileEditorModelFileNames, vvBlock, vvHeight, bSubUnit, eBlockType, fLift, fShiftX, fShiftZ);
	}

	void COperator::SwitchSnapMode()
	{
		pOperatorPropertyCtrl->bSnapMode = !pOperatorPropertyCtrl->bSnapMode;
	}

	bool COperator::GetSnapMode()
	{
		return pOperatorPropertyCtrl->bSnapMode;
	}

	void COperator::OpenSaveMapMark()
	{
		CMapEditApp::GetInst()->m_bSave = true;

		MessageBox(NULL,"可以保存地图","提示",MB_OK);
	}

	void COperator::SetObjectHot( CTObject * p )
	{
		m_processParams.pObjectHot = p;

		CEditStateOpertiaonMgr::EEDIT_STATE eState = CEditStateOpertiaonMgr::GetInst()->GetEditState();

		if( eState == CEditStateOpertiaonMgr::EES_SET_TILE_LOGIN ) 
			::SendMessage(pOperatorPropertyCtrl->hLoginSetView,WM_LOGINVIEW_SELECT,0,0);
		else
		{
			if( eState == CEditStateOpertiaonMgr::EES_AUDIO ) 
				this->RefreshAmbientAudioPro();
		}
	}

	void COperator::SetIsUseSpecular( bool b )
	{
		m_bUseSpecular = b;

		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain && !pTerrain->IsWater() )
			pTerrain->ReWriteTerrainBuffer(pTerrain->m_TerrainBufferIndexs, 1);
	}
}
