#include "stdafx.h"
#include "ToolSetModel.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "UserMsg.h"
#include "Render.h"

#include "CmdPlaceObject.h"
#include "CmdMoveObject.h"
#include "CmdBornObject.h"
#include "CmdDeleteObject.h"
#include "CmdRotateObject.h"
#include "CmdLiftMoveObject.h"
#include "CmdHideObject.h"
#include "picker.h"
#include "Engine.h"
#include "CTileEditorModel.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "CGraphic.h"
#include "OffLineBaker/OfflineRender.h"
#include "BaseHelper.h"
#include "OperationLog.h"
#include "CMapEditModelObjPro.h"
#include "CEditModel.h"
#include "EditStateOpertiaonMgr.h"
#include "IoSystem.h"
#include "CMapEffect.h"
#include "CEventState.h"

int nRotation = 0; // 地表旋转角度（0－不旋转；1－顺时针旋转90度；2－顺时针旋转180度；3－顺时针旋转270度）

int nGridIndexSave = 0;
vector<int> vecGridIndicesToCopySave;

namespace sqr
{
	CToolSetModel::CToolSetModel()
	{
		m_eCopyType					= ECT_TERRAIN;
		m_eMirrorType				= EMT_XWAY;
		m_eGenType					= EGT_SINGLE;
		m_eScaleType				= EES_SCALEXYZ;
		m_vMouseStart				= CVector3f(0.0f,0.0f,0.0f);
		m_vMouseEnd					= CVector3f(0.0f,0.0f,0.0f);
		m_bPreSelecting				= false;
		m_bSelecting				= false;
		m_dwWidth					= 0;
		m_dwDepth					= 0;
		m_nAnchorPoint				= 1; //%%%%% 定位点默认在左下角
		m_pAreaLockTexture			= NULL;
		m_pRS						= NULL;
		m_pTitleModel				= NULL;
		m_bAlwaysSelectthisObject = false;
	}

	CToolSetModel::~CToolSetModel()
	{
		if(m_pAreaLockTexture)
			SafeRelease(m_pAreaLockTexture);
		if(m_pRS)
			m_pRS = NULL;
		if(m_pTitleModel)
			m_pTitleModel = NULL;
	}

	void CToolSetModel::Initialize(HWND hResSetView, HWND hLoginSetView)
	{	
		m_hResSetView	= hResSetView;
		m_hLoginSetView = hLoginSetView;

		m_SetLogicLong  = 0;
		m_SetLogicShort = 0;
		m_SetLogicType  = 0;
		m_SetShowPath   = false;

		m_vecLockedObjects.clear();
		m_vecActiveObjects.clear();

		// create shader
		CRenderDevice * pRender = CRenderDevice::GetInst();

		m_eCopyType = ECT_OBJECT;

		string config = "MEditorRes:DefaultLock.dds";
		string str = IoSystem::GetInst()->GetFullPath(config);
		m_pAreaLockTexture = pRender->GetTexture(str);
	}

	void CToolSetModel::CreateActiveObject(BYTE bResGroupMark, bool bVisibleByConfigure, bool bShadow, bool bTranslucent, string strItemName, vector<string> & veCEditModelFileNames, bool bSubUnit, sqr::ECenter_Type eCenterType, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float xScale, float yScale, float zScale, 
										   CVector3f vGroupOffset, bool bCameraOriented, bool bEffectBase, bool bAudio, bool bLogin)
	{
		CCmdBornObject * pCmd = new CCmdBornObject;
		pCmd->SetItemName(strItemName);
		pCmd->SetModelFileNames(veCEditModelFileNames);
		pCmd->SetSubUnit(bSubUnit);
		pCmd->SetCenterType(eCenterType);
		pCmd->SetUserOffset(fLift, fShiftX, fShiftZ);
		pCmd->SetUserOffsetEx(fLiftEx, fShiftXEx, fShiftZEx);
		pCmd->SetRotation(fRotation);
		pCmd->SetRotationCount(nRotationCnt);
		pCmd->SetPitch(fPitch);
		pCmd->SetPitchCount(nPitchCnt);
		pCmd->SetYaw(fYaw);
		pCmd->SetYawCount(nYawCnt);
		pCmd->SetScaleX(xScale);
		pCmd->SetScaleY(yScale);
		pCmd->SetScaleZ(zScale);
		pCmd->SetGroupOffset(vGroupOffset);
		pCmd->SetCameraOriented(bCameraOriented);
		pCmd->SetIsEffectBase(bEffectBase);
		pCmd->SetTranslucent(bTranslucent);
		pCmd->SetCastShadow(bShadow);
		pCmd->SetResGroupMark(bResGroupMark);
		pCmd->SetIsVisibleByConfigure(bVisibleByConfigure);
		pCmd->SetIsAudio(bAudio);
		pCmd->SetIsLoginModel(bLogin);
		pCmd->Execute();
	}

	void CToolSetModel::CreateTitleActiveObject(vector<string> & veCTileEditorModelFileNames, vector< vector<sqr::SBlockInfo> > & vvBlock, vector< vector<sqr::SLogicHeightInfo> > & vvHeight, bool bSubUnit, sqr::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ)
	{
		COperator * pOperator		= CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		//////////////////////////////////////////////////////////////////////////
		// delete current if any
		if ( pOperator->GetTitleActiveObject() )
		{
			pOperator->SetTitleActiveObject(NULL);
		}

		CTObjectPtr pObject = new CTObject;
		CEditModelGroup * pModelGroup = new CEditModelGroup;

		int BlockInfoNum = 0, LogicHeightInfoNum = 0;
		vector<CTileEditorModel*> vec;
		vector<string>::const_iterator iter = veCTileEditorModelFileNames.begin();
		for ( int i = 0; iter != veCTileEditorModelFileNames.end(); ++iter, ++i )
		{
			if(m_pTitleModel)
				m_pTitleModel->UnRender();
			m_pTitleModel = CSceneManager::GetInst()->AddNodeTitleModel(*iter, bSubUnit, fLift, fShiftX, fShiftZ, BlockInfoNum, LogicHeightInfoNum);
			vec.push_back(m_pTitleModel);
		}

		pModelGroup->AddTitleContainer(vec,bSubUnit,eBlockType,0,0);
		pModelGroup->SetModelFileNames(veCTileEditorModelFileNames);
		pObject->SetModelGroup(pModelGroup);
		pObject->GetModelGroup()->SetEditState(sqr::EES_PLACED);
		pOperator->SetTitleActiveObject(pObject);
	}

	void CToolSetModel::RenderPreViewModel()
	{
		if( m_pTitleModel )
			m_pTitleModel->Render();
	}

	CTObject* CToolSetModel::PlaceRebirthModel(const int nGridIndexHot, const int nRenderStyle, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ, const CVector3f& pos)
	{
		if( m_vecActiveObjects.empty() )
			return NULL;

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}

		m_vecLockedObjects = m_vecActiveObjects;

		size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(true);
		}

		// place an object
		CCmdPlaceObject * pCmd = NULL;
		pCmd = new CCmdPlaceObject;
		if( pCmd == NULL )
			return false;

		for ( int nModelIndex = 0; nModelIndex < m_vecActiveObjects.back()->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_vecActiveObjects.back()->GetModelGroup()->GetSelfPresentation(nModelIndex);

			pModel->SetRenderStyle(nRenderStyle);
			pModel->RestoreRotation( fRotation, nRotationCnt, fYaw, nYawCnt, fPitch, nPitchCnt );
			pModel->RestoreScale( scaleX, scaleY, scaleZ );
			pModel->SetWorldPosition(pos);
		}


		pCmd->SetHotGridIndex(nGridIndexHot);

		CTObject * p = NULL;
		p = m_vecActiveObjects.back();
		pCmd->SetObject(p);
		pCmd->Execute();

		return p;
	}

	bool CToolSetModel::Rebirth(int nGridIndexHot, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ, const CVector3f& pos, int nAniStart, float fPlus, sqr::EAUTO_GENERATION_TYPE eAutoGenerationType, int nRenderStyle, int nAtten, bool bVisible,bool bShadow,bool bTrans,bool bZTest,bool bSelfZTest,bool bExport)
	{
		if ( m_vecActiveObjects.empty() ) 
			return false;

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTObject * p = PlaceRebirthModel(nGridIndexHot, nRenderStyle, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ, pos);
		if( p )
		{
			///得到当前物体是否需要收到阴影
			bool bReceiveShadow = pOperator->IsReceiveShadow();

			p->GetModelGroup()->SetAtten(nAtten);
			p->GetModelGroup()->SetShadowSecondPass(bVisible);
			p->GetModelGroup()->SetCastShadow(bShadow);
			p->GetModelGroup()->SetIsTranslucent(bTrans);

			p->SetCastShadow(bShadow);

			p->GetModelGroup()->SetReceiveShadow(bReceiveShadow);

			pOperator->SetReceiveShadow(false);

			return true;
		}
		else
			return false;
	}

	bool CToolSetModel::Rebirth(const MoldeSaveProperty* pModelSavePro, int nAffectRatio, const string strCurName)
	{
		CTObject * p = NULL;
		p = PlaceRebirthModel(pModelSavePro->nCenterGridIndex,pModelSavePro->byRenderStyle,pModelSavePro->vRotate.y, pModelSavePro->nRotationCnt, 
			pModelSavePro->vRotate.z, pModelSavePro->nPitchCnt, pModelSavePro->vRotate.x, pModelSavePro->nYawCnt, pModelSavePro->vScale.x, pModelSavePro->vScale.y, pModelSavePro->vScale.z, pModelSavePro->vPosition);

		if( p )
		{
			CEditModelGroup *pModelGroup = p->GetModelGroup();
			if( pModelGroup == NULL )
				return false;

			pModelGroup->SetMoldeSaveProperty(pModelSavePro);
			pModelGroup->SetAffectRatio(nAffectRatio);
			pModelGroup->SetIsTranslucent(pModelSavePro->bTranslucent);
			pModelGroup->SetIsUseCameraCollison(pModelSavePro->bUseCameraCollision);

			if( !strCurName.empty() )
				p->GetModelGroup()->SetSceneCueName(strCurName);		
			
			return true;
		}
		else
			return false;
	}

	bool CToolSetModel::Rebirth(int nGridIndexHot,BYTE bTransState, int nMirrorType, int nAffectRatio, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ, int nAniStart, float fPlus, sqr::EAUTO_GENERATION_TYPE eAutoGenerationType, int nRenderStyle, int nAtten, bool bVisibleByConfigure, bool bShadow,bool bTrans, string strCurName)
	{
		if ( m_vecActiveObjects.empty() ) 
			return false;

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		
		///得到当前物体是否需要收到阴影
		bool bReceiveShadow = pOperator->IsReceiveShadow();
		CVector3f pos = CVector3f(0.0f, 0.0f, 0.0f);
		CTObject *p = PlaceRebirthModel(nGridIndexHot, nRenderStyle, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, scaleX, scaleY, scaleZ, pos);
		if( p )
		{
			p->GetModelGroup()->SetModelTransState(bTransState);

			p->GetModelGroup()->SetAffectRatio( nAffectRatio );
			p->GetModelGroup()->SetAtten(nAtten);
			p->GetModelGroup()->SetIsVisibleByConfigure(bVisibleByConfigure);
			p->GetModelGroup()->SetCastShadow(bShadow);
			p->GetModelGroup()->SetIsTranslucent(bTrans);

			p->SetCastShadow(bShadow);

			p->GetModelGroup()->SetReceiveShadow(bReceiveShadow);

			pOperator->SetReceiveShadow(false);

			if( strCurName != "" )
				p->GetModelGroup()->SetSceneCueName(strCurName);

			return true;
		}
		else
			return false;
	}

#pragma region shiftkey
	void CToolSetModel::ShiftKeyDownEvent(const MSG_KEY_INF& key)
	{
		switch(key)
		{
			//////////////////////////////////////////////////////////////////////////
			///spot light
			/// pageup 目标位置上偏
		case MGI_KEY_Prior:
			{	
				this->UpdateSpotTargetPos(0);
				return;
			}

			/// pagedown 目标位置下偏
		case MGI_KEY_Next:
			{
				this->UpdateSpotTargetPos(1);
				return;
			}

			///left 目标位置左偏
		case MGI_KEY_Left:
			{
				this->UpdateSpotTargetPos(2);
				return;
			}

			///right 目标位置右偏
		case MGI_KEY_Right:
			{
				this->UpdateSpotTargetPos(3);
				return;
			}

			///目标位置里移
		case MGI_KEY_Up:
			{
				this->UpdateSpotTargetPos(5);
				return;
			}

			///目标位置外移
		case MGI_KEY_Down:
			{
				this->UpdateSpotTargetPos(4);
				return;
			}

			//////////////////////////////////////////////////////////////////////////

		default:
			break;
		}
	}
#pragma endregion shiftkey

#pragma region controlkey
	void CToolSetModel::ControlKeyDownEvent(const MSG_KEY_INF& key)
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		switch(key)
		{
		case MGI_KEY_D0:
			{
				static bool bHide = true;
				this->HideAndShowAllObjects(bHide);
				bHide = !bHide;

				return;
			}

			//////////////////////////////////////////////////////////////////////////

		case MGI_KEY_A:
			this->SelectSceneAllTheSameObjects();	
			return;

		case MGI_KEY_R:
			this->RevertSelectObject();
			return;

		case MGI_KEY_Q:
			m_bAlwaysSelectthisObject = !m_bAlwaysSelectthisObject;
			return;

		case MGI_KEY_F7:
			this->SetIsVisibleByConfigure(true);
			return;

		case MGI_KEY_F8:
			this->SetIsVisibleByConfigure(false);
			return;

			///ctrl+delete delete scene all objct
		case MGI_KEY_Delete:
			this->DeleteSceneObjects();

			//模型烘培颜色不比阴影色更暗,周自强希望烘焙后的草跟地表的lightmap颜色一致, 就是color = max(bakedColor, globalShadowColor);
		case MGI_KEY_F9:
			this->SetBakingColorIsNotGreaterShadow();
			break;

		case MGI_KEY_F6:
			this->SwitchModelIsUseCameraCollison();
			break;

		case MGI_KEY_C:
			{
				this->CopyTerrainAndObject();
				break;
			}

		case MGI_KEY_V:
			{
				this->PastateTerrain();
				break;
			}

		case MGI_KEY_D3:
			{
				::SendMessage( m_hResSetView, WM_TEXTUREVIEW_CHIFFLAYONE, 1, 0);
				return;
			}

		default:
			return;
		}
	}
#pragma endregion controlkey

#pragma region normalkey
	void CToolSetModel::NormalKeyDownEvent(const sqr::MSG_KEY_INF &key)
	{
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		switch(key)
		{
 			///next object
 		case MGI_KEY_N:
 			//pOperator->SelectNextObject();
 			this->SelectGridNextObject();
 			break;

			///object mirror
		case MGI_KEY_M:
			this->MirrorSelectObject();
			return;

			///hide select object
		case MGI_KEY_Back:
			{
				if( CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EES_AUDIO )
					return;

				static bool bHideMode = true;
				this->HideAndShowObject(bHideMode);
				bHideMode = !bHideMode;

				return;
			}

			///隐藏物体顶点色
		case MGI_KEY_F11:
			{
				this->HideModelVertexColor();
				return;
			}

			///enlarge model
		case MGI_KEY_D7:
			this->ChangeObjectSize(0);
			break;

			///small model
		case MGI_KEY_D8:
			this->ChangeObjectSize(1);
			break;

			///revert model size
		case MGI_KEY_D6:
			this->ChangeObjectSize(2);
			break;

			///x rotate +
		case MGI_KEY_R:
			this->RotateSelectObject(0);
			break;

			///x rotate -
		case MGI_KEY_F:
			this->RotateSelectObject(1);
			break;

			///y rotate +
		case MGI_KEY_I:
			this->RotateSelectObject(2);
			break;

			///y rotate -
		case MGI_KEY_K:
			this->RotateSelectObject(3);
			break;

			///Z rotate +
		case MGI_KEY_Y:
			this->RotateSelectObject(4);
			break;

			///Z rotate -
		case MGI_KEY_H:
			this->RotateSelectObject(5);
			break;

			///垂直旋转90度
		case MGI_KEY_T:
			this->ApeakRotateObject();
			break;

			///拾取
		case MGI_KEY_C:
			this->PickObject(false);
			break;

			///音效开关
		case MGI_KEY_F8:
			this->StopModelMusic();
			break;

		case MGI_KEY_Space:
			{
				this->LockSelectObjects();
				return;
			}

			///left
		case MGI_KEY_Left:
			{
				this->MoveSelectObject(4);
				return;
			}

			///right
		case MGI_KEY_Right:
			{
				this->MoveSelectObject(5);
				return;
			}

			///in
		case MGI_KEY_Up:
			{
				this->MoveSelectObject(6);
				return;
			}

			///out
		case MGI_KEY_Down:
			{
				this->MoveSelectObject(7);
				return;
			}

			///切换单个物体接收阴影
		case MGI_KEY_F5:
			{
				this->SwitchReceiveShadow();
				return;
			}

			///设置阴影
		case MGI_KEY_F6:
			{
				this->OnSwitchShadow();
				return;
			}

			///下一个渲染状态
		case MGI_KEY_F9:
			{
				this->OnUseNextRenderStyle();
				return;
			}

			///On/Off物体骨骼
		case MGI_KEY_F12:
			{
				this->PlayAniOnOff();
				return;
			}

		default:
			break;
		}
	}
#pragma endregion normalkey

	void CToolSetModel::OnEvent(const CMsg& msg)
	{
		COperator *pOperator	   = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		
		switch ( msg.msgSource )
		{		
		case MGS_KEYMSG:
			{
				switch(msg.msgType.key_type)
				{
				case MGT_KEY_UP:
					{

					}
					break;
				case MGT_KEY_DOWN:
					{
						if ( !m_vecLockedObjects.empty() || m_eCopyType == ECT_TERRAIN || m_eCopyType == ECT_GENERATE )
						{	
						}

						if ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN  )
						{
							this->ShiftKeyDownEvent(msg.msgInfo.key_info);
						}

						else if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN  )
						{
							this->ControlKeyDownEvent(msg.msgInfo.key_info);
						}

						this->NormalKeyDownEvent(msg.msgInfo.key_info);
					}
					break;
				}
			}

			break;
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_LBNUP:
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN )
						{
							CTObject * p = pOperator->GetObjectHot();
							if( p != NULL )
							{
								p->GetModelGroup()->SetLocked(true);
								m_vecLockedObjects.push_back(p);
							}
						}
						else
						{
							size_t size = m_vecLockedObjects.size();
							for ( size_t i = 0; i < size; ++i )
							{
								m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
							}
							m_vecLockedObjects.clear();
						}

						if ( m_bSelecting )
						{
							this->LockSelectObjects();

							if( !m_vecLockedObjects.empty() )
								m_vGroupCenter = (m_vMouseEnd + m_vMouseStart)/2.0f;

							m_bSelecting = false;
							m_bPreSelecting = false;

							break;
						}

						////////////////////////////////////////////////////////////////////////////

						// place all objects or lock one
						if( !m_vecActiveObjects.empty() )
						{
							//CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(true);

							SQR_TRY
							{
								PlaceActiveObject();
							}
							SQR_CATCH(exp)
							{
								//CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(false);

								string log = exp.ErrorMsg();
								log = "放置物体失败";
								MessageBox(NULL, log.c_str(), "提示", MB_OK);
								return;
							}
							SQR_TRY_END;

							//CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(false);

						}
						else
						{
							this->MoveHotObject();
						}
					}


			case MGT_MOUSE_LBNDOWN:	
				{
					if ( m_vecActiveObjects.empty() && !(CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN) && !(CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN) )
					{
						CTObject * p = pOperator->GetObjectHot();

						if ( p && (m_eCopyType == ECT_GENERATE) ) break;

						CPicker::GetInst()->GetPickPos(NULL,&m_vMouseStart);
						m_vMouseEnd = m_vMouseStart;
						m_bPreSelecting = true;
					}
				}
				break;

			case MGT_MOUSE_MOVE :
				{
					if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
					{
						if ( m_bPreSelecting && m_vecActiveObjects.empty() && !(CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN) && !(CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN) )
						{	
							CPicker::GetInst()->GetPickPos(NULL,&m_vMouseEnd);

							if ( abs((m_vMouseEnd - m_vMouseStart).x) > GRID_SPACE*2 || abs((m_vMouseEnd - m_vMouseStart).z) > GRID_SPACE*2 )
							{
								m_bSelecting = true;
							}
						}
					}
				}
				break;
				}
				break;

				//////////////////////////////////////////////////////////////////////////
			}
		}
	}

	void CToolSetModel::Render()
	{
		CRenderDevice *pRender	= CRenderDevice::GetInst();
		COperator* pOperator	= CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh* pTerrain	= CMapEditApp::GetInst()->GetEditingMesh();

		static CVector3f vertex[4];
		static CVector2f uv0[4];
		static DWORD color[4];
		static VerColorTex rvertex[6];

		// show brush
		if ( pOperator->GetGridIndexHot() != INVALID )
		{
			int nGridIndex = pOperator->GetGridIndexHot();
			//%%%%% begin

			if(!m_vecGridIndicesToCopy.empty())
			{
				nRotation = nRotation % 4;
				switch(nRotation)
				{
				case 0:
					{
						CVector3f vStartPos;
						if(pTerrain->IsValid(nGridIndex))
						{
							SGrid & s = pTerrain->GetGrid(nGridIndex);	
							vStartPos = pTerrain->GetVertex(s.dwVertexIndex[0]).vPosition;
						}

						vertex[0] = vStartPos;
						vertex[1] = vStartPos + CVector3f(0.0f, 0.0f, m_dwDepthToCopy * GRID_SPACE);
						vertex[2] = vStartPos + CVector3f(m_dwWidthToCopy * GRID_SPACE, 0.0f, 0.0f);
						vertex[3] = vStartPos + CVector3f(m_dwWidthToCopy * GRID_SPACE, 0.0f, m_dwDepthToCopy * GRID_SPACE);
					}
					break;
				}
			}
			else
			{
				//%%%%% end
				SGrid & s = pTerrain->GetGrid(nGridIndex);
				vertex[0] = pTerrain->GetVertex(s.dwVertexIndex[0]).vPosition;
				vertex[1] = pTerrain->GetVertex(s.dwVertexIndex[3]).vPosition;
				vertex[2] = pTerrain->GetVertex(s.dwVertexIndex[1]).vPosition;
				vertex[3] = pTerrain->GetVertex(s.dwVertexIndex[2]).vPosition;
			}//%%%%%

			color[0] = 0xdfff0000;
			color[1] = 0xdfff0000;
			color[2] = 0xdfff0000;
			color[3] = 0xdfff0000;

			rvertex[0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, color[0], 0.0f, 1.0f);
			rvertex[1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color[1], 0.0f, 0.0f);
			rvertex[2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color[2], 1.0f, 1.0f);

			rvertex[3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color[1], 0.0f, 0.0f);
			rvertex[4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, color[3], 1.0f, 0.0f);
			rvertex[5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color[2], 1.0f, 1.0f);

			pRender->DrawLayer3DWireQuad(&m_pRS, rvertex, 1);

			m_pRS->m_TexOP_S0 = TOP_MODULATE;
			m_pRS->m_TexArg1_S0 = TA_TEXTURE;
			m_pRS->m_TexArg2_S0 = TA_DIFFUSE;

			m_pRS->m_AlpOP_S0 = TOP_SELECTARG1;
			m_pRS->m_AlpArg1_S0 = TA_TEXTURE;

			m_pRS->m_AlphaTestEnable = true;
			m_pRS->m_Alpharef = 0x40;
			m_pRS->m_AlphaTestFun = CMP_GREATER; 
			m_pRS->m_AlphaBlendEnable = false;

			m_pRS->m_LightEnable = false;
			m_pRS->m_ZTestEnable = false;
			m_pRS->m_ZWrite = true;
			m_pRS->m_Cull = CULLT_CCW;
		}

		//////////////////////////////////////////////////////////////////////////

		vector<int> vecBrushGridIndices;
		color[0] = 0xffffffff;
		color[1] = 0xffffffff;
		color[2] = 0xffffffff;
		color[3] = 0xffffffff;

		// draw selected area 渲染鼠标选中的区域
		if ( m_bSelecting || m_eCopyType == ECT_TERRAIN || m_eCopyType == ECT_ALL || m_eCopyType == ECT_GENERATE )
		{
			vecBrushGridIndices = this->GetSelectedGridIndices();
		}

		if( vecBrushGridIndices.empty() )
			return;

		size_t cnt = vecBrushGridIndices.size();

		float fStepInX = 0.0f, fStepInZ = 0.0f;

		fStepInX = 1.0f/(m_dwWidth);
		fStepInZ = 1.0f/(m_dwDepth);

		DWORD drawnCnt = 0;
		for ( DWORD n = 0; n < cnt; ++n, ++drawnCnt )
		{	
			if ( drawnCnt && !((drawnCnt) % GRID_BATCH_SIZE) )
			{	
				pRender->DrawLayer3DSolidQuad(&m_pRS, rvertex, m_pAreaLockTexture, GRID_BATCH_SIZE);

				m_pRS->m_TexOP_S0 = TOP_MODULATE;
				m_pRS->m_TexArg1_S0 = TA_TEXTURE;
				m_pRS->m_TexArg2_S0 = TA_DIFFUSE;

				m_pRS->m_AlpOP_S0 = TOP_SELECTARG1;
				m_pRS->m_AlpArg1_S0 = TA_TEXTURE;

				m_pRS->m_AlphaTestEnable = true;
				m_pRS->m_Alpharef = 0x40;
				m_pRS->m_AlphaTestFun = CMP_GREATER; 
				m_pRS->m_AlphaBlendEnable = false;
				m_pRS->m_LightEnable = false;
				m_pRS->m_ZTestEnable = true;
				m_pRS->m_ZWrite = true;
				m_pRS->m_Cull = CULLT_CCW;
				drawnCnt = 0;
			}

			SGrid & s = pTerrain->GetGrid(vecBrushGridIndices[n]);

			int nStepsInX = 0, nStepsInZ = 0;
			if ( m_vMouseEnd.z > m_vMouseStart.z )
			{
				if ( m_vMouseEnd.x > m_vMouseStart.x )
				{
					nStepsInX = n % (m_dwWidth);
					nStepsInZ = m_dwDepth - n / (m_dwWidth);
				}
				else
				{
					nStepsInX = m_dwWidth - (n % (m_dwWidth)) - 1;
					nStepsInZ = m_dwDepth - n / (m_dwWidth);
				}
			}
			else
			{
				if ( m_vMouseEnd.x > m_vMouseStart.x )
				{
					nStepsInX = n % (m_dwWidth);
					nStepsInZ = n / (m_dwWidth) + 1;
				}
				else
				{
					nStepsInX = m_dwWidth - (n % (m_dwWidth)) - 1;
					nStepsInZ = n / (m_dwWidth) + 1;
				}
			}


			vertex[0] = pTerrain->GetVertex(s.dwVertexIndex[0]).vPosition;
			uv0[0] = CVector2f(nStepsInX*fStepInX, nStepsInZ*fStepInZ);

			vertex[1] = pTerrain->GetVertex(s.dwVertexIndex[3]).vPosition;
			uv0[1] = CVector2f(nStepsInX*fStepInX, nStepsInZ*fStepInZ - fStepInZ);

			vertex[2] = pTerrain->GetVertex(s.dwVertexIndex[1]).vPosition;
			uv0[2] = CVector2f(nStepsInX*fStepInX + fStepInX, nStepsInZ*fStepInZ);

			vertex[3] = pTerrain->GetVertex(s.dwVertexIndex[2]).vPosition;
			uv0[3] = CVector2f(nStepsInX*fStepInX + fStepInX, nStepsInZ*fStepInZ - fStepInZ);

			rvertex[6 * drawnCnt + 0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv0[0].x, uv0[0].y);
			rvertex[6 * drawnCnt + 1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv0[2].x, uv0[2].y);

			rvertex[6 * drawnCnt + 3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv0[3].x, uv0[3].y);
			rvertex[6 * drawnCnt + 5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv0[2].x, uv0[2].y);
		}
		if( drawnCnt > 0 )
		{
			pRender->DrawLayer3DSolidQuad(&m_pRS, rvertex, m_pAreaLockTexture, drawnCnt);

			m_pRS->m_TexOP_S0 = TOP_MODULATE;
			m_pRS->m_TexArg1_S0 = TA_TEXTURE;
			m_pRS->m_TexArg2_S0 = TA_DIFFUSE;

			m_pRS->m_AlpOP_S0 = TOP_SELECTARG1;
			m_pRS->m_AlpArg1_S0 = TA_TEXTURE;

			m_pRS->m_AlphaTestEnable = true;
			m_pRS->m_Alpharef = 0x40;
			m_pRS->m_AlphaTestFun = CMP_GREATER; 
			m_pRS->m_AlphaBlendEnable = false;
			m_pRS->m_LightEnable = false;
			m_pRS->m_ZTestEnable = true;
			m_pRS->m_ZWrite = true;
			m_pRS->m_Cull = CULLT_CCW;
		}

		///render 聚光灯的方向
		CEditModelGroup *pModelGroup = NULL;
		size_t lockcnt = m_vecLockedObjects.size();
		for ( size_t i = 0; i < lockcnt; ++i )
		{
			pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
			if ( pModelGroup && pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
			{
				CVector3f start = pModelGroup->m_SpotLight.m_TargetPos;
				CVector3f end   = pModelGroup->m_SpotLightPos;

				if( start != end )
					pRender->Draw3DLines(start, end, 0xff00ff00);
			}
		}
	}

	void CToolSetModel::SetMirrorType(const int nCopyType)
	{
		m_eMirrorType = EMirrorType(nCopyType);
	}

	void CToolSetModel::SetSelectSceneType(const int nType)
	{
		m_eSelectTpye = ESelectType(nType);
	}

	void CToolSetModel::SetCopyType(const int nCopyType)
	{
		m_eCopyType = ECopyType(nCopyType);
	}

	void CToolSetModel::SetGenType(const int nGenType)
	{
		m_eGenType = EGenType(nGenType);
	}

	void CToolSetModel::SetSceleType(const EEdit_Scale_Type eScaleType)
	{
		m_eScaleType = eScaleType;
	}

	vector<int> & CToolSetModel::GetSelectedGridIndices()
	{
		static vector<int> vecBrushGridIndices;

		vecBrushGridIndices.clear();

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		
		m_dwWidth = DWORD(fabs((m_vMouseStart - m_vMouseEnd).x)/(GRID_SPACE)+1);
		m_dwDepth = DWORD(fabs((m_vMouseStart - m_vMouseEnd).z)/(GRID_SPACE)+1);
		
		int nGridIndexStart = (int)(m_vMouseStart.x/GRID_SPACE) + ((int)(m_vMouseStart.z/GRID_SPACE))*pTerrain->GetWidth();
		if ( pTerrain->IsValid(nGridIndexStart) )
		{
			DWORD dwCnt = m_dwWidth * m_dwDepth;
			int nGridIndex = nGridIndexStart;
			
			for ( DWORD j = 0; j < dwCnt; j++)
			{
				if ( j )
				{
					if ( !(j % m_dwWidth) ) 
					{
						if ( m_vMouseEnd.z > m_vMouseStart.z )
						{
							if ( m_vMouseEnd.x > m_vMouseStart.x )
							{
								nGridIndex += pTerrain->GetWidth() - m_dwWidth + 1;
							}
							else
							{
								nGridIndex += pTerrain->GetWidth() + m_dwWidth - 1;
							}
						}
						else
						{
							if ( m_vMouseEnd.x > m_vMouseStart.x )
							{
								nGridIndex -= pTerrain->GetWidth() + m_dwWidth - 1;
							}
							else
							{
								nGridIndex -= pTerrain->GetWidth() - m_dwWidth + 1;
							}
						}
					}
					else
					{
						if ( m_vMouseEnd.x > m_vMouseStart.x )
						{
							++nGridIndex;
						}
						else
						{
							--nGridIndex;
						}
					}
				}

				// check for valid grid
				if ( pTerrain->IsValid(nGridIndex) )
				{
					vecBrushGridIndices.push_back(nGridIndex);
				}
			}
		}

		return vecBrushGridIndices;
	}

	void CToolSetModel::AddActiveObject(CTObject * p)
	{
		m_vecActiveObjects.push_back(p);
	}

	void CToolSetModel::RemoveActiveObject(CTObject * p)
	{
		m_vecActiveObjects.erase(
			remove(m_vecActiveObjects.begin(), m_vecActiveObjects.end(), CTObjectPtr(p)), m_vecActiveObjects.end() );
	}

	void CToolSetModel::ReMoveLastObject()
	{
		CTObject *p = NULL;

		p = this->GetLastActiveObject();

		if( p )
		{
			CEditModelGroup *pModelGroup = NULL;
			pModelGroup = p->GetModelGroup();
			if( pModelGroup )
			{
				if( pModelGroup->GetIsEffectBased() )
				{
					CMapEditApp::GetInst()->GetOperator()->RemoveIndividualEffectObject(p);
				}
				pModelGroup->ReleaseSelfModels();
				pModelGroup = NULL;
			}

			this->RemoveActiveObject(p);
		}
	}

	void CToolSetModel::SetActiveObjects(vector< CTObjectPtr> & v)
	{
		m_vecActiveObjects = v;
	}

	vector< CTObjectPtr> & CToolSetModel::GetActiveObjects()
	{
		return m_vecActiveObjects;
	}


	vector< CTObjectPtr> & CToolSetModel::GetLockObjects()
	{
		return m_vecLockedObjects;
	}

	CTObject * CToolSetModel::GetLockObjectHot()
	{
		vector< CTObjectPtr> &LockObjects = this->GetLockObjects();
		if( LockObjects.empty() )
			return NULL;
		else
			return LockObjects[0];
	}

	CTObject * CToolSetModel::GetLastActiveObject()
	{
		if ( !m_vecActiveObjects.empty() )
			return m_vecActiveObjects.back();
		else
			return NULL;
	}

	CTObject * CToolSetModel::GetLastLockedObject()
	{
		if ( !m_vecLockedObjects.empty() )
			return m_vecLockedObjects.back();
		else
			return NULL;
	}

	void CToolSetModel::SetLastLockedObject(CTObject * p)
	{
		if( m_bAlwaysSelectthisObject )
			return;

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}

		m_vecLockedObjects.clear();
		m_vecLockedObjects.push_back(p);
		p->GetModelGroup()->SetLocked(true);
	}

	int CToolSetModel::GetActiveObjectsCount()
	{
		return  m_vecActiveObjects.size();
	}

	void CToolSetModel::OnUseNextRenderStyle()
	{
		int nPresentationCount = 0;
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CEditModelGroup *pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
		
			nPresentationCount = pModelGroup->GetSelfPresentationCount();
			for ( int nModelIndex = 0; nModelIndex < nPresentationCount; ++nModelIndex )
			{
				CMapEditObject * pModel = NULL;
				pModel = pModelGroup->GetSelfPresentation(nModelIndex);

				if( pModel )
				{
					pModel->UseNextRenderStyle();
					BYTE byRenderStyle = pModelGroup->GetRenderStyle();
					pModel->sModelSavePro.byRenderStyle = byRenderStyle;
				}
			}
		}
	}

	void CToolSetModel::OnDelete()
	{
		if(m_vecLockedObjects.empty()) 
			return;

		size_t size = m_vecLockedObjects.size();
		bool   bDelete = true;
		if( size > 50 )
		{
			if ( IDNO == MessageBox(NULL, "当前准备删除的物体个数大于50个，确定是否删除", "YES删除", MB_YESNO) )
				bDelete = false;
		}

		if( bDelete )
		{
			OnDeleteSelectModels();
			m_vecLockedObjects.clear();
		}
	}

	void CToolSetModel::OnDeleteSelectModels()
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			OnDeleteModelByIndex(i);
		}
	}

	void CToolSetModel::OnDeleteModelByIndex(const size_t index)
	{
		CCmdDeleteObject * pCmd = NULL;
		pCmd = new CCmdDeleteObject;
		if( pCmd == NULL )
			return;

		int nGridIndex = m_vecLockedObjects[index]->GetCenterGridIndex();

		pCmd->SetCenterGridIndex(nGridIndex);
		pCmd->SetObject(m_vecLockedObjects[index]);

		////////////////////////////////////////////////////////////////////////////
		int nModelPresentationCount = m_vecLockedObjects[index]->GetModelGroup()->GetSelfPresentationCount();
		for ( int nModelIndex = 0; nModelIndex < nModelPresentationCount; ++nModelIndex )
		{
			CMapEditObject * pModel = NULL;
			pModel = m_vecLockedObjects[index]->GetModelGroup()->GetSelfPresentation(nModelIndex);
			if( pModel == NULL )
				return;

			pCmd->AddBlockGridIndex(INVALID);
			pCmd->AddVertexIndex(INVALID);
		}

		////////////////////////////////////////////////////////////////////////////

		pCmd->Execute();
		m_vecLockedObjects[index]->GetModelGroup()->SetLocked(false);
	}

	void CToolSetModel::ReMoveObjects(int nWay)
	{
		if(!m_vecActiveObjects.empty()) 
			return;

		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		float liftx = 0.0f, liftz = 0.0f; 
		int nGridIndexBlockCenter = 0;

		CEditModelGroup *pModelGroup = NULL;
		vector<int> gridIndexs;
		
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			bool bBreak = false;

			pModelGroup =  m_vecLockedObjects[i]->GetModelGroup();
			liftx = pModelGroup->GetShiftXEx();
			liftz = pModelGroup->GetShiftZEx();
			gridIndexs.push_back(pModelGroup->GetCenterGridIndex());

			bool bEffectBase = pModelGroup->GetIsEffectBased();
			string strEffectName = "";
			pModelGroup->GetEffectName(strEffectName);
			IEffectGroup *pGroup = pModelGroup->GetEffectGroup();
			IEffect * pEffect = pModelGroup->GetEffect();

			CCmdLiftMoveObject * pLiftMoveObjectCmd = new CCmdLiftMoveObject;

			pLiftMoveObjectCmd->SetObject(m_vecLockedObjects[i]);

			switch(nWay)
			{
			case 0:
				{
					//判断是否在移动范围内，是的话，就直接移动
					if( liftx > -GRID_SPACE * 0.5f + 1.1f )
					{
						pModelGroup->UpdateShiftXEx(nWay);
						bBreak = true;
					}
				}
				break;
			case 1:
				{
					if( liftx < GRID_SPACE * 0.5f - 1.1f )
					{
						pModelGroup->UpdateShiftXEx(nWay);
						bBreak = true;
					}
				}
				break;
			case 2:
				{
					if( liftz > -GRID_SPACE/2 + 1.1f )
					{
						pModelGroup->UpdateShiftZEx(nWay);
						bBreak = true;
					}
				}
				break;
			case 3:
				{
					if( liftz < GRID_SPACE/2 - 1.1f )
					{
						pModelGroup->UpdateShiftZEx(nWay);
						bBreak = true;
					}
				}
				break;
			default:
				break;
			}

			nGridIndexBlockCenter = pModelGroup->GetPreCenterGridIndex();
			pLiftMoveObjectCmd->SetCenterGridIndex(nGridIndexBlockCenter);
			pLiftMoveObjectCmd->SetShiftXEx(liftx);
			pLiftMoveObjectCmd->SetShiftZEx(liftz);

			if( !bBreak )
			{
				//首先delete
				//不需要清空m_vecLockedObjects
				OnDeleteModelByIndex(i);

				//然后再放置

				CVector3f blockCenter = pTerrainMesh->GetGrid(nGridIndexBlockCenter).vCenter;

				switch(nWay)
				{
				case 0:
					{
						--nGridIndexBlockCenter;

						blockCenter.x -= GRID_SPACE;

						pModelGroup->SetPlacedEditState(sqr::EESP_SHIFTX);
						break;
					}
					
				case 1:
					{
						++nGridIndexBlockCenter;
						blockCenter.x += GRID_SPACE;
						pModelGroup->SetPlacedEditState(sqr::EESP_SHIFTX);
						break;
					}

				case 2:
					{
						nGridIndexBlockCenter -= pTerrainMesh->GetWidth();
						blockCenter.z -= GRID_SPACE;
						pModelGroup->SetPlacedEditState(sqr::EESP_SHIFTZ);
						break;
					}

				case 3:
					{
						nGridIndexBlockCenter += pTerrainMesh->GetWidth();
						blockCenter.z += GRID_SPACE;
						pModelGroup->SetPlacedEditState(sqr::EESP_SHIFTZ);
						break;
					}
					
				default:
					break;
				}

				pModelGroup->SetBlockCenter(blockCenter);
				pModelGroup->m_bMoveDelete = true;

				if( bEffectBase )
				{
					pModelGroup->SetIsEffectBased(bEffectBase);
					pModelGroup->LinkEffect(pEffect, pGroup);
				}

				// VALIDATION //
				if ( nGridIndexBlockCenter == INVALID ) 
					return; 

				CCmdPlaceObject * pCmd = new CCmdPlaceObject;

				pCmd->SetGroupOffset(pModelGroup->GetSelfPresentation(0)->GetGroupOffset());

				int nAniStart = 0;
				float fPlus = 1.0f;
				int nMaxFrameCnt = pModelGroup->GetAniFrameCount();

				for ( int nModelIndex = 0; nModelIndex < pModelGroup->GetSelfPresentationCount(); nModelIndex++ )
				{
					CMapEditObject * pModel = pModelGroup->GetSelfPresentation(nModelIndex);

					switch(nWay)
					{
					case 0:
						pModel->SetShiftXEx(GRID_SPACE * 0.5f);
						break;
					case 1:
						pModel->SetShiftXEx(-GRID_SPACE * 0.5f);
						break;
					case 2:
						pModel->SetShiftZEx(GRID_SPACE * 0.5f);
						break;
					case 3:
						pModel->SetShiftZEx(-GRID_SPACE * 0.5f);
						break;
					default:
						break;
					}
					pModel->SetAnimationStart(/*nAniStart*/0,fPlus);
				}

				pCmd->SetHotGridIndex(nGridIndexBlockCenter);

				pCmd->SetObject(m_vecLockedObjects[i]);

				pCmd->Execute();

				m_vecLockedObjects[i]->GetModelGroup()->SetLocked(true);
			}

			pLiftMoveObjectCmd->Execute();
		}
	}

	void CToolSetModel::OnSwitchShadow()
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->SwitchShadow();

			bool bShadow = m_vecLockedObjects[i]->GetModelGroup()->IsCastShadow();
			bShadow = !bShadow;
			m_vecLockedObjects[i]->GetModelGroup()->SetCastShadow(bShadow);
		}
	}

	void CToolSetModel::OnObjectsShadow(const bool bOn)
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->SwitchShadow();

			m_vecLockedObjects[i]->GetModelGroup()->SetCastShadow(bOn);
		}
	}

	void CToolSetModel::CalculateGroupCenter()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		int nXMin = 0, nXMax = 0;
		int nYMin = 0, nYMax = 0;
		nXMin = nYMin = MAXLONG;
		nXMax = nYMax = -1;

		size_t size = m_vecLockedObjects.size();
		if(size > 1)
		{
			for ( size_t i = 0; i < size; ++i )
			{
				int nCenterGridIndex = m_vecLockedObjects[i]->GetModelGroup()->GetCenterGridIndex();
				int nX = nCenterGridIndex % pTerrain->GetWidth();
				int nY = nCenterGridIndex / pTerrain->GetWidth();
			
				if(nX <= nXMin) nXMin = nX;
				if(nX >= nXMax) nXMax = nX;

				if(nY <= nYMin) nYMin = nY;
				if(nY >= nYMax) nYMax = nY;	
			}

			int nLeftBottomGridIndex = nYMin * pTerrain->GetWidth() + nXMin;
			SGrid & gridLeftBottom = pTerrain->GetGrid(nLeftBottomGridIndex);

			int nRightTopGridIndex = nYMax * pTerrain->GetWidth() + nXMax;
			SGrid & gridRightTop = pTerrain->GetGrid(nRightTopGridIndex);

			m_vGroupCenter = 0.5f * (gridLeftBottom.vCenter + gridRightTop.vCenter);
		}
	}

	void CToolSetModel::PlaceActiveObject()
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		// VALIDATION //
		if ( pOperator->GetGridIndexHot() == INVALID ) 
			return; 

		//////////////////////////////////////////////////////////////////////////
		vector< CTObjectPtr> vecActiveObjects = m_vecActiveObjects;
		size_t size = vecActiveObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CEditModelGroup *pModelGroup = vecActiveObjects[i]->GetModelGroup();
			int nGridIndexBlockCenter = pModelGroup->GetAdjustedGridIndexBlockCenter(pOperator->GetGridIndexHot());

			// VALIDATION //
			if ( nGridIndexBlockCenter == INVALID ) return; 

			CCmdPlaceObject * pCmd = NULL;
			pCmd = new CCmdPlaceObject;
			if( pCmd == NULL )
				return;

			pCmd->SetGroupOffset(pModelGroup->GetSelfPresentation(0)->GetGroupOffset());
			pCmd->SetHotGridIndex(nGridIndexBlockCenter);
			pCmd->SetObject(vecActiveObjects[i]);

			pCmd->Execute();		
		}

		if(!m_vecLockedObjects.empty())
		{
			for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
			}

			m_vecLockedObjects.clear();
		}

		m_vecLockedObjects  =  vecActiveObjects;

		size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(true);
		}
	}

	void CToolSetModel::SwitchReceiveShadow()
	{
		bool   bReceiveShadow = false;
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			bReceiveShadow = m_vecLockedObjects[i]->GetModelGroup()->IsReceiveShadow();
			bReceiveShadow = !bReceiveShadow;
			m_vecLockedObjects[i]->GetModelGroup()->SetReceiveShadow(bReceiveShadow);
		}
	}

	void CToolSetModel::SetAllObjectReceiveShadow(const bool bReceiveShadow)
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		if( pOperator )
		{
			CTerrainMesh *pTerrain = NULL;
			pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
			if ( pTerrain )
			{
				int nGridCount = pTerrain->GetGridCount();
				vector< CTObjectPtr >::iterator iter;
				CEditModelGroup *pModelGroup = NULL;

				for ( int i = 0; i < nGridCount; ++i )
				{
					SGrid * pGrid = &pTerrain->GetGrid(i);

					if ( !pGrid->vecCenterObjects.empty() )
					{
						iter = pGrid->vecCenterObjects.begin();
						for ( ; iter != pGrid->vecCenterObjects.end(); ++iter)
						{
							CTObjectPtr p = (*iter);

							pModelGroup = p->GetModelGroup();

							pModelGroup->SetReceiveShadow(bReceiveShadow);
						}
					}
				}
			}

			MessageBox(0, "设置完毕", "提示", 0);
		}
	}

	void CToolSetModel::CopyTerrain()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		pTerrainMesh->InitHeightDeltaAll();
		m_vecGridIndicesToCopy = this->GetSelectedGridIndices();

		m_dwWidthToCopy = m_dwWidth;
		m_dwDepthToCopy = m_dwDepth;
		sort(m_vecGridIndicesToCopy.begin(),m_vecGridIndicesToCopy.end());
		nRotation = 0; //%%%%%
		m_nAnchorPoint = 1;
	}

	void CToolSetModel::CopySceneObject()
	{
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		if ( !m_vecActiveObjects.empty() )
			return;

		vector< CTObjectPtr> vecLockedObjects = m_vecLockedObjects;

		for ( size_t i = 0; i < vecLockedObjects.size(); i++ )
		{
			int nGridIndexBlockCenter = vecLockedObjects[i]->GetCenterGridIndex();

			SGrid & grid = pTerrainMesh->GetGrid(nGridIndexBlockCenter);

			CVector3f vGroupOffset;

			// more than one
			if ( vecLockedObjects.size() > 1 )
			{
				vGroupOffset = grid.vCenter - m_vGroupCenter; 
			}
			else
			{
				vGroupOffset = CVector3f(0.0f,0.0f,0.0f);
			}									

			CVector3f vv(vecLockedObjects[i]->GetModelGroup()->GetUserOffsetEx().y,
				vecLockedObjects[i]->GetModelGroup()->GetUserOffsetEx().x,
				vecLockedObjects[i]->GetModelGroup()->GetUserOffsetEx().z);

			CEditModelGroup *pModelGroup = NULL;
			pModelGroup = vecLockedObjects[i]->GetModelGroup();
			if( pModelGroup == NULL )
				return;

			pOperator->CopyRebirth(nGridIndexBlockCenter, pModelGroup, vecLockedObjects[i]->GetAutoGenerationType(), vGroupOffset);

			//copy new born
			CEditModelGroup *pNewModelGroup = m_vecActiveObjects.back()->GetModelGroup();

			if ( pModelGroup->m_SL.m_bIAmbScenePointLight )
			{
				string strEffectName;
				pModelGroup->SetIsEffectBased(true);
				pModelGroup->GetEffectName(strEffectName);
				CMapEffect::GetInst()->ToolSetModel_LinkEffect(strEffectName,vecLockedObjects[i]->GetModelGroup()->GetEffectGroup(), NULL);

				pNewModelGroup->m_SL = pModelGroup->m_SL;
			}
			else if( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
			{
				string strEffectName;
				pModelGroup->SetIsEffectBased(true);
				pModelGroup->GetEffectName(strEffectName);
				CMapEffect::GetInst()->ToolSetModel_LinkEffect(strEffectName,vecLockedObjects[i]->GetModelGroup()->GetEffectGroup(), NULL);
				pNewModelGroup->m_SpotLight = pModelGroup->m_SpotLight;
			}

			pNewModelGroup->SetAffectRatio(pModelGroup->GetAffectRatio());
			pNewModelGroup->UpdateScale(3, EES_SCALEXYZ);

			pNewModelGroup->SetGroupOffeset(vGroupOffset);
		}

		for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}

		m_vecLockedObjects.clear();
		m_vecLockedObjects = vecLockedObjects;
	}

	void CToolSetModel::CopyTerrainAndObject()
	{
		switch (m_eCopyType)
		{
		case ECT_OBJECT:
			this->CopySceneObject();
			break;

		case ECT_TERRAIN:
			this->CopyTerrain();
			break;

		case ECT_ALL:
			{
				//this->LockSelectObjects();
				this->CopySceneObject();
				this->CopyTerrain();
				break;
			}

		default:
			break;

		}
	}

	void CToolSetModel::PastateTerrain()
	{
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		if ( m_eCopyType == ECT_TERRAIN || m_eCopyType== ECT_ALL && !CMapEditApp::GetInst()->GetIsEditingWater() )
		{
			if ( pOperator->GetGridIndexHot() != INVALID && m_vecGridIndicesToCopy.size() > 0)
			{
				int nGridIndex = pOperator->GetGridIndexHot();

				nGridIndexSave = nGridIndex;
				vecGridIndicesToCopySave = m_vecGridIndicesToCopy;

				pTerrainMesh->RotateGrids(nGridIndex, m_dwWidthToCopy, m_dwDepthToCopy, m_vecGridIndicesToCopy, nRotation, m_nAnchorPoint);
				m_vecGridIndicesToCopy.clear();
			}
		}
	}

	void CToolSetModel::SelectSceneAllTheSameObjects()
	{
		set<string> selectarplist;
		string strArpName = "", comparearpname = "";

		// remove all original
		for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
		{
			CEditModelGroup *pModelGroup = NULL;
			pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
			if( pModelGroup == NULL )
				continue;

			strArpName = pModelGroup->GetModelFileNameByIndex(0);
			selectarplist.insert(strArpName);

			pModelGroup->SetLocked(false);
		}


		m_vecLockedObjects.clear();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		set<string>::iterator iter = selectarplist.begin();
		set<string>::iterator eiter = selectarplist.end();
		size_t npos = 0;
		DWORD gridCnt = pTerrainMesh->GetGridCount();

		for ( iter; iter != eiter; ++iter )
		{
			comparearpname = *iter;

			for ( DWORD i = 0; i < gridCnt; ++i )
			{
				SGrid & grid = pTerrainMesh->GetGrid(i);

				if ( !grid.vecCenterObjects.empty() )
				{
					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
					{
						CEditModelGroup *pModelGroup = NULL;
						pModelGroup = grid.vecCenterObjects[j]->GetModelGroup();

						strArpName = pModelGroup->GetModelFileNameByIndex(0);

						if( strArpName.compare(comparearpname) == 0 )
							m_vecLockedObjects.push_back(grid.vecCenterObjects[j]);
					}
				} 
			}

		}

		for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(true);
		}
	}

	void CToolSetModel::RevertSelectObject()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		DWORD gridCnt = pTerrainMesh->GetGridCount();

		static bool bRevert = true;

		if( bRevert )
		{
			for ( DWORD i = 0; i < gridCnt; ++i )
			{
				SGrid & grid = pTerrainMesh->GetGrid(i);

				if ( !grid.vecCenterObjects.empty() )
				{
					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
						grid.vecCenterObjects[j]->GetModelGroup()->SetEditState(sqr::EES_DELETE);
				} 
			}

			size_t size = m_vecLockedObjects.size();
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->SetEditState(sqr::EES_PLACED);
			}
		}
		else
		{
			for ( DWORD i = 0; i < gridCnt; ++i )
			{
				SGrid & grid = pTerrainMesh->GetGrid(i);

				if ( !grid.vecCenterObjects.empty() )
				{
					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
						grid.vecCenterObjects[j]->GetModelGroup()->SetEditState(sqr::EES_PLACED);
				} 
			}
		}

		bRevert = !bRevert;
	}

	void CToolSetModel::PlayAniOnOff()
	{
		static bool bStop = true;

		if(!m_vecActiveObjects.empty()) 
			return;

		if( bStop )
		{
			size_t size = m_vecLockedObjects.size();
			for ( size_t i = 0; i < size; i++ )
			{
				m_vecLockedObjects[i]->GetModelGroup()->StopModelAnimation();
			}
		}
		else
		{
			size_t size = m_vecLockedObjects.size();
			for ( size_t i = 0; i < size; i++ )
			{
				string agpname = m_vecLockedObjects[i]->GetModelGroup()->GetAnimationGroupName();
				m_vecLockedObjects[i]->GetModelGroup()->SetAnimationByAgpFileName(agpname);
			}
		}

		bStop = !bStop;
	}

	void CToolSetModel::SelectGridNextObject()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		vector< CTObjectPtr> vecActiveObjects = m_vecLockedObjects;

		COpearationLog::GetInst()->WriteOperationLog("选中下一个物体");

		size_t size = vecActiveObjects.size();
		for ( size_t m = 0; m < size; ++m )
		{
			int nGridCenter = vecActiveObjects[m]->GetModelGroup()->GetSelfPresentation(0)->GetCenterGridIndex();

			if( pTerrainMesh->IsValid(nGridCenter) )
			{
				SGrid & grid = pTerrainMesh->GetGrid(nGridCenter);

				size_t i = 0, j = 0;

				CEditModelGroup *pModelGroup = NULL;
				for( ; i < grid.vecCenterObjects.size(); ++i )
				{
					CTObjectPtr p = grid.vecCenterObjects[i];

					pModelGroup = p->GetModelGroup();
					if( !pModelGroup->GetLocked() && !pModelGroup->m_bNext )
					{
						pModelGroup->SetLocked(true);
						pModelGroup->m_bNext = true;
					}
					else
						pModelGroup->SetLocked(false);

					++j;
				}
				if( j == grid.vecCenterObjects.size() )
				{
					for( size_t n = 0; n < grid.vecCenterObjects.size(); ++n )
					{
						if( n == 0 )
						{
							grid.vecCenterObjects[n]->GetModelGroup()->SetLocked(true);
							grid.vecCenterObjects[n]->GetModelGroup()->m_bNext = true;
						}
						else
						{
							grid.vecCenterObjects[n]->GetModelGroup()->SetLocked(false);
							grid.vecCenterObjects[n]->GetModelGroup()->m_bNext = false;
						}
					}
				}
			}
		}
	}

	void CToolSetModel::ChangeObjectSize( const int index )
	{
		char szObjectBuf[128];
		sprintf(szObjectBuf, "改变物体大小,type = %d", index);
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

		if(m_vecLockedObjects.empty())
			return;

		CEditModelGroup *pModelGroup = NULL;

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
			pModelGroup->UpdateScale(index, m_eScaleType);
			pModelGroup->SetPlacedEditState(sqr::EESP_SCALE);
		}
	}

	void CToolSetModel::MoveSelectObject( const int index )
	{
		if(m_vecLockedObjects.empty())
			return;

		CEditModelGroup *pModelGroup = NULL;
		size_t			size		 = m_vecLockedObjects.size();

		switch (index)
		{
			//上移
		case 0:
			for ( size_t i = 0; i < size; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
				pModelGroup->UpdateLiftEx(1);
				pModelGroup->SetPlacedEditState(sqr::EESP_LIFT);
			}
			break;

			//下移
		case 1:
			for ( size_t i = 0; i < size; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
				m_vecLockedObjects[i]->GetModelGroup()->UpdateLiftEx(0);
				m_vecLockedObjects[i]->GetModelGroup()->SetPlacedEditState(sqr::EESP_LIFT);
			}
			break;

			//快速上移
		case 2:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateSuperLiftEx(1);
					pModelGroup->SetPlacedEditState(sqr::EESP_LIFT);
				}
			}
			break;

			//快速下移
		case 3:
			for ( size_t i = 0; i < size; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
				pModelGroup->UpdateSuperLiftEx(0);
				pModelGroup->SetPlacedEditState(sqr::EESP_LIFT);
			}
			break;

			//左移
		case 4:
			ReMoveObjects(0);
			break;

			//右移
		case 5:
			ReMoveObjects(1);
			break;

			//前移
		case 6:
			ReMoveObjects(3);
			break;

			//后移
		case 7:
			ReMoveObjects(2);
			break;

		default:
			break;
		}

	}

	void CToolSetModel::RotateSelectObject( const int index )
	{
		if(m_vecLockedObjects.empty()) 
			return;

		char szObjectBuf[128];
		sprintf(szObjectBuf, "旋转物体,type = %d", index);
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

		CEditModelGroup *pModelGroup = NULL;
		size_t			size		 = m_vecLockedObjects.size();

		switch (index)
		{
			//x轴正向旋转
		case 0:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(4);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}
			}
			break;

			//x轴负向旋转
		case 1:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(5);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}

			}
			break;

			//y轴正向旋转
		case 2:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(1);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}
			}
			break;

			//y轴负向旋转
		case 3:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(0);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}
			}
			break;

			//z轴正向旋转
		case 4:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(2);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}
			}
			break;

			//z轴负向旋转
		case 5:
			{
				for ( size_t i = 0; i < size; ++i )
				{
					pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					pModelGroup->UpdateRotation(3);
					pModelGroup->SetPlacedEditState(sqr::EESP_ROTATE);
				}
			}
			break;

		default:
			break;
		}
	}

	void CToolSetModel::ApeakRotateObject()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		if(m_vecLockedObjects.empty() || !m_vecActiveObjects.empty()) 
			return;

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; i++ )
		{
			CCmdRotateObject * pCmd = NULL;
			pCmd = new CCmdRotateObject;
			if( pCmd == NULL )
				return;

			pCmd->SetObject(m_vecLockedObjects[i]);
			pCmd->Execute();
			m_vecLockedObjects[i]->GetModelGroup()->SetPlacedEditState(sqr::EESP_ROTATE);
		}
	}

	void CToolSetModel::PickObject(bool bRotate)
	{
		if( m_vecLockedObjects.empty() )
			return;

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		if(m_vecActiveObjects.empty())
		{
			vector< CTObjectPtr> vecLockedObjects = m_vecLockedObjects;

			this->CalculateGroupCenter();

			size_t size = vecLockedObjects.size();
			if( bRotate && size > 1 )
			{
				for ( size_t i = 0; i < size; ++i )
				{
					// Calculate the result grid index this obj is in after rotating
					CEditModelGroup *pModelGroup = vecLockedObjects[i]->GetModelGroup();
					pModelGroup->SetGroupCenter(m_vGroupCenter);
					pModelGroup->SetNotGroup(FALSE);

					int nCenterGridIndexCur = vecLockedObjects[i]->GetModelGroup()->GetCenterGridIndexCurrent();

					// Move
					CCmdMoveObject * pCmdMove = NULL;
					pCmdMove = new CCmdMoveObject;
					if( pCmdMove == NULL )
						return;

					int nCenterGridIndex = vecLockedObjects[i]->GetModelGroup()->GetCenterGridIndex();

					if ( pModelGroup->m_SL.m_bIAmbScenePointLight )
					{
						set<int>::iterator iter = pTerrainMesh->m_DynamicLightGrids.find(nCenterGridIndex);
						if ( iter != pTerrainMesh->m_DynamicLightGrids.end() )
							pTerrainMesh->m_DynamicLightGrids.erase(iter);

						iter = pTerrainMesh->m_VisibleDynamicLightGrids.find(nCenterGridIndex);
						if ( iter != pTerrainMesh->m_VisibleDynamicLightGrids.end() )
							pTerrainMesh->m_VisibleDynamicLightGrids.erase(iter);
					}

					pCmdMove->SetCenterGridIndex(nCenterGridIndex);
					pCmdMove->SetObject(vecLockedObjects[i]);
					CVector3f vGroupOffset = CVector3f(0.0f,0.0f,0.0f);
					pCmdMove->SetGroupOffset(vGroupOffset);

					pCmdMove->Execute();

					// Place
					//

					//CTObject * p = m_vecActiveObjects[0];
					CTObject * p = vecLockedObjects[i];

					CCmdPlaceObject * pCmdPlace = NULL;
					pCmdPlace = new CCmdPlaceObject;
					if( pCmdPlace == NULL )
						return;

					pCmdPlace->SetGroupOffset(p->GetModelGroup()->GetSelfPresentation(0)->GetGroupOffset());

					int nAniStart = 0;
					float fPlus = 1.0f;
					int nMaxFrameCnt =  p->GetModelGroup()->GetAniFrameCount();

					for ( int nModelIndex = 0; nModelIndex < p->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
					{
						CMapEditObject * pModel = p->GetModelGroup()->GetSelfPresentation(nModelIndex);
						pModel->SetAnimationStart(/*nAniStart*/0,fPlus);
					}

					pCmdPlace->SetHotGridIndex(nCenterGridIndexCur);
					pCmdPlace->SetObject(p);

					pCmdPlace->Execute();

					// notify the client
					::SendMessage(m_hResSetView, WM_RESSETVIEW_NEWONEBORN,0,0);

					//
					// Rotate
					CCmdRotateObject * pCmdRotate = NULL;
					pCmdRotate = new CCmdRotateObject;
					if( pCmdRotate == NULL )
						return;

					pCmdRotate->SetObject(vecLockedObjects[i]);
					pCmdRotate->Execute();
					vecLockedObjects[i]->GetModelGroup()->SetPlacedEditState(sqr::EESP_ROTATE);

				}
			}
			else
			{
				// Move to the cursor
				//
				size = m_vecLockedObjects.size();
				for ( size_t i = 0; i < size; ++i )
				{
					int nGridIndexBlockCenter = m_vecLockedObjects[i]->GetCenterGridIndex();

					SGrid & grid = pTerrainMesh->GetGrid(nGridIndexBlockCenter);

					CEditModelGroup *pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
					if ( pModelGroup->m_SL.m_bIAmbScenePointLight )
					{
						set<int>::iterator iter = pTerrainMesh->m_DynamicLightGrids.find(nGridIndexBlockCenter);
						if ( iter != pTerrainMesh->m_DynamicLightGrids.end() )
							pTerrainMesh->m_DynamicLightGrids.erase(iter);

						iter = pTerrainMesh->m_VisibleDynamicLightGrids.find(nGridIndexBlockCenter);
						if ( iter != pTerrainMesh->m_VisibleDynamicLightGrids.end() )
							pTerrainMesh->m_VisibleDynamicLightGrids.erase(iter);
					}

					CVector3f vGroupOffset;

					// more than one
					if ( size > 1 )
					{
						vGroupOffset = grid.vCenter - m_vGroupCenter; 
					}
					else
					{
						vGroupOffset = CVector3f(0.0f,0.0f,0.0f);
					}

					CCmdMoveObject * pCmd = NULL;
					pCmd = new CCmdMoveObject;
					if( pCmd == NULL )
						return;

					int nCenterGridIndex = m_vecLockedObjects[i]->GetCenterGridIndex();
					pCmd->SetCenterGridIndex(nCenterGridIndex);
					pCmd->SetObject(m_vecLockedObjects[i]);
					pCmd->SetGroupOffset(vGroupOffset);

					pCmd->Execute();
				}	

			}
		}
	}

	void CToolSetModel::ChangeObjectAttention( const int index )
	{
		size_t size = m_vecLockedObjects.size();
		if( index == 0 )
		{
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->IncAffectRatio();
			}
		}
		else if( index == 1 )
		{
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->DecAffectRatio();
			}
		}
	}

	void CToolSetModel::MirrorSelectObject()
	{
		COpearationLog::GetInst()->WriteOperationLog("设置物体镜像属性");

		for ( size_t i = 0; i < m_vecLockedObjects.size(); i++ )
			m_vecLockedObjects[i]->GetModelGroup()->SetMirrorType(m_eMirrorType);
	}

	void CToolSetModel::HideAndShowObject( bool bHide )
	{
		COpearationLog::GetInst()->WriteOperationLog("显示/隐藏物体");
		size_t size = m_vecLockedObjects.size();
		
		//隐藏当前选中模型 后退键
		if( bHide )
		{
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->HideSelf();
				m_vecShowHideObjects.push_back(m_vecLockedObjects[i]);
			}
		}
		else
		{
			//显示消隐的模型 | 

			size = m_vecShowHideObjects.size();
			for ( size_t i = 0; i < size; ++i )
				m_vecShowHideObjects[i]->GetModelGroup()->ShowSelf();

			m_vecShowHideObjects.clear();
		}
	}

	void CToolSetModel::HideAndShowAllObjects(bool bHide)
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		DWORD dwGridCnt = pTerrainMesh->GetGridCount();

		for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			SGrid * pGrid = &pTerrainMesh->GetGrid(i);
			if ( !pGrid->vecCenterObjects.empty() )
			{
				for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);

					if( bHide )
						p->GetModelGroup()->HideSelf();
					else
						p->GetModelGroup()->ShowSelf();
				}
			}
		}
	}

	void CToolSetModel::HideModelVertexColor()
	{
		//隐藏顶点对模型的影响

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			for ( int nModelIndex = 0; nModelIndex < m_vecLockedObjects[i]->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
			{
				static bool bHideVertex = false;
				bHideVertex = !bHideVertex;
				m_vecLockedObjects[i]->GetModelGroup()->SetHideModelVertexColor(bHideVertex);
			}
		}
	}

	void CToolSetModel::UpdateSpotTargetPos( const int index )
	{
		if ( !m_vecLockedObjects.empty() || m_eCopyType == ECT_TERRAIN || m_eCopyType == ECT_GENERATE )
		{
			CEditModelGroup *pModelGroup = NULL;
			size_t lockcnt = m_vecLockedObjects.size();
			for ( size_t i = 0; i < lockcnt; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
				if ( pModelGroup && pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
				{
					pModelGroup->UpdateSpotTargetPos(index);
				}
			}
		}
	}

	void CToolSetModel::SetLightEffectterrain()
	{
		if ( !m_vecLockedObjects.empty() )
		{
			CEditModelGroup *pModelGroup = NULL;
			size_t lockcnt = m_vecLockedObjects.size();
			for ( size_t i = 0; i < lockcnt; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();
				int nGridIndex = pModelGroup->GetCenterGridIndex();

				if( pModelGroup )
				{
					if ( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
						pModelGroup->m_SpotLight.m_bEffectTerrain = true;
					else if ( pModelGroup->m_SL.m_bIAmbScenePointLight )
						pModelGroup->m_SL.m_bEffectTerrain = true;

					OfflineRender::GetInst()->UpdateGridLights(nGridIndex);

					pModelGroup = NULL;
				}

			}
		}
	}

	void CToolSetModel::SetLightUnEffectterrain()
	{
		if ( !m_vecLockedObjects.empty() )
		{
			CEditModelGroup *pModelGroup = NULL;
			size_t lockcnt = m_vecLockedObjects.size();
			for ( size_t i = 0; i < lockcnt; ++i )
			{
				pModelGroup = m_vecLockedObjects[i]->GetModelGroup();

				if( pModelGroup )
				{
					if ( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
						pModelGroup->m_SpotLight.m_bEffectTerrain = false;
					else if ( pModelGroup->m_SL.m_bIAmbScenePointLight )
						pModelGroup->m_SL.m_bEffectTerrain = false;

					pModelGroup = NULL;
				}

			}
		}
	}

	void CToolSetModel::StopModelMusic()
	{
		if(m_vecLockedObjects.empty()) 
			return;

		bool bStop = m_vecLockedObjects[0]->GetModelGroup()->GetIsStopSound();
		bStop = !bStop;

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->StopSceneCue(bStop);

	}

	void CToolSetModel::SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed )
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->SetNextAnimation(szAniFileName, true, DelayTime, AniSpeed);
	}

	void CToolSetModel::SetAnimationFrame( const string& str, const int8 nFrame )
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->SetAnimationFrame(str, nFrame);
	}

	void CToolSetModel::CleanAniFrames(const string& str)
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->CleanAniFrames(str);
	}

	void CToolSetModel::DeleteAniFrame(const string& str, const int nCurrKey)
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->DeleteAniFrame(str, nCurrKey);
	}

	int16 CToolSetModel::GetAnimationFrame( const string& str ) const
	{
		if( m_vecLockedObjects.empty() )
			return -1;
		else
			return m_vecLockedObjects[0]->GetModelGroup()->GetAnimationFrame(str);
	}

	vector<int16> CToolSetModel::GetAnimationFrames( const string& str ) const
	{
		if( !m_vecLockedObjects.empty() )
			return m_vecLockedObjects[0]->GetModelGroup()->GetAnimationFrames(str);
		else
		{
			vector<int16> frameList; 
			return frameList;
		}
	}

	void CToolSetModel::SetIsVisibleByConfigure( const bool b )
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
			m_vecLockedObjects[i]->GetModelGroup()->SetIsVisibleByConfigure(b);
	}

	void CToolSetModel::SelectAllSceneObject()
	{
		CTerrainMesh * pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrainMesh == NULL )
			return;

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}
		m_vecLockedObjects.clear();

		int nTerrainGrids = pTerrainMesh->GetGridCount();
		for ( int i = 0; i < nTerrainGrids; ++i )
		{
			SGrid * pGrid = &pTerrainMesh->GetGrid(i);

			if ( !pGrid->vecCenterObjects.empty())
			{
				size_t objectCnt = pGrid->vecCenterObjects.size();
				for ( size_t j = 0; j < objectCnt; ++j)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[j];

					m_vecLockedObjects.push_back(p);
					p->GetModelGroup()->SetLocked(true);
				}
			}
		}
	}

	void CToolSetModel::SetWhichLoginObjectIsLock(const string& str)
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}
		m_vecLockedObjects.clear();

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		map<int, string>::iterator beg;
		map<int, string>::iterator end = pOperator->m_LoginModelGridIndexs.end();

		for ( beg = pOperator->m_LoginModelGridIndexs.begin(); beg != end; ++beg )
		{
			int nGridIndex = beg->first;
			SGrid * pGrid = &pTerrainMesh->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty() )
			{
				for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);
					CEditModelGroup *pModelGroup = p->GetModelGroup();

					///login model 跳过
					if( pModelGroup->m_bLoginModel )
					{
						string strItemName = pModelGroup->GetItemName();
						if( strItemName == str )
						{
							m_vecLockedObjects.push_back(p);
							pModelGroup->SetLocked(true);
							this->m_bAlwaysSelectthisObject = true;
							return;
						}
					}
				}
			}
		}
	}

	vector<int> CToolSetModel::GetSelectSceneObject( const string& str )
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}
		m_vecLockedObjects.clear();

		vector<int> gridIndexList;
		
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		vector<DWORD> calRangeVisibleGridIndexs;
		
		DWORD dwGridCnt = 0;
 		dwGridCnt = pTerrainMesh->GetGridCount();

		int nGridIndex = 0;
		for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			nGridIndex = i;

			SGrid * pGrid = &pTerrainMesh->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty() )
			{
				for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);
					CEditModelGroup *pModelGroup = p->GetModelGroup();
					string strEffectName;
					pModelGroup->GetEffectFileName(strEffectName);

					switch (m_eSelectTpye)
					{
					case SCL_OBJECT:
						{
							string strItemName = pModelGroup->GetItemName();
							if( strItemName == str && strEffectName.empty() && pModelGroup->m_pSceneAudio == NULL )
								gridIndexList.push_back(nGridIndex);
						}
						break;

					case SCL_EFFECT:
						{
							if( strEffectName.find(str) != -1 )
								gridIndexList.push_back(nGridIndex);
						}
						break;

					case SCL_AUDIO:
						{
							if( pModelGroup->m_pSceneAudio )
							{
								string strCueName = pModelGroup->GetSceneCueName();
								if( strCueName.find(str) != -1 )
									gridIndexList.push_back(nGridIndex);
							}
						}
						break;

					case SCL_LIGHT:
						{
							if( pModelGroup->m_SL.m_bIAmbScenePointLight || pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
								gridIndexList.push_back(nGridIndex);
						}
						break;

					default:
						break;
					}
				}
			}
		}

		return gridIndexList;
	}

	void CToolSetModel::LockSceneObject( const string& str, const int& nGridIndex )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
		}
		m_vecLockedObjects.clear();

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if( !pTerrainMesh->IsValid(nGridIndex) )
			return;

		//DWORD dwGridCnt = pTerrainMesh->GetGridCount();
		//for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			SGrid * pGrid = &pTerrainMesh->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty() )
			{
				for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);
					CEditModelGroup *pModelGroup = p->GetModelGroup();

					switch (m_eSelectTpye)
					{
					case SCL_OBJECT:
						{
							string strItemName = pModelGroup->GetItemName();
							if( strItemName == str )
							{
								m_vecLockedObjects.push_back(p);
								pModelGroup->SetLocked(true);
								return;
							}
						}
						break;

					case SCL_EFFECT:
						{
							string strEffectName;
							pModelGroup->GetEffectFileName(strEffectName);
							if( strEffectName.find(str) != -1 )
							{
								m_vecLockedObjects.push_back(p);
								pModelGroup->SetLocked(true);
								return;
							}
						}
						break;

					case SCL_AUDIO:
						{
							if( pModelGroup->m_pSceneAudio )
							{
								string strCueName = pModelGroup->GetSceneCueName();
								if( strCueName.find(str) != -1 )
								{
									m_vecLockedObjects.push_back(p);
									pOperator->RefreshAmbientAudioPro();
									pModelGroup->SetLocked(true);
									return;
								}
							}
						}
						break;

					case SCL_LIGHT:
						{
							if( pModelGroup->m_SL.m_bIAmbScenePointLight || pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
							{
								//if( i == nGridIndex )
								{
									m_vecLockedObjects.push_back(p);
									if( pModelGroup->m_SL.m_bIAmbScenePointLight)
									{
										pOperator->RefreshEffectViewSceneLightSliderState(
											pModelGroup->m_SL.m_Rang,
											pModelGroup->m_SL.m_Attenuation0,
											pModelGroup->m_SL.m_Attenuation1,
											pModelGroup->m_SL.m_Attenuation2,
											pModelGroup->m_SL.m_Dyn_BrightTime ,
											pModelGroup->m_SL.m_Dyn_DarkTime,
											pModelGroup->m_SL.m_Dyn_ChangeTime,
											pModelGroup->m_SL.m_Dyn_ChangeProbability,
											pModelGroup->m_SL.m_bDynamic );
									}
									else if( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
									{
										int nTheta = (int)(pModelGroup->m_SpotLight.m_fTheta * 180.0f / CMath::pi + 0.5f);
										int nPhi = (int)(pModelGroup->m_SpotLight.m_fPhi * 180.0f / CMath::pi + 0.5f);

										pOperator->RefreshEffectViewSpotLightSliderState(
											pModelGroup->m_SpotLight.m_fRang,
											pModelGroup->m_SpotLight.m_fAttenuation0,
											pModelGroup->m_SpotLight.m_fAttenuation1,
											pModelGroup->m_SpotLight.m_fAttenuation2,
											pModelGroup->m_SpotLight.m_fFallOff,
											float(nTheta), float(nPhi));
									}
									pModelGroup->SetLocked(true);
									return;
								}
							}
						}
						break;

					default:
						break;
					}
				}
			}
		}
	}

	void CToolSetModel::LockObjectLinkEffect(const string& effectname)
	{
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CTObject *pModel = NULL;
			pModel = m_vecLockedObjects[i];

			if( pModel )
				CMapEffect::GetInst()->ToolSetModel_LinkEffect(effectname, pModel);
		}
	}

	void CToolSetModel::LockObjectLinkAnimation()
	{
		//if( m_vecLockedObjects.empty() )
		//	return;

		//string aniname("");
		//static TCHAR Filter[] = " Animation Files (*.agp)|*.agp|";

		//CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT, Filter );
		//if ( Dlg.DoModal() == IDOK )
		//{
		//	POSITION Pos = Dlg.GetStartPosition();

		//	CString str = Dlg.GetNextPathName( Pos );

		//	aniname = str.GetBuffer();

		//	locale loc;
		//	use_facet< ctype<TCHAR> >( loc ).tolower
		//		( &aniname[0], &aniname[aniname.length()] );

		//	int nPos = aniname.find("tile\\");
		//	if ( nPos != -1 )
		//	{
		//		aniname = aniname.substr( nPos, aniname.length() );
		//	}
		//}

		//size_t size = m_vecLockedObjects.size();
		//for ( size_t i = 0; i < size; i++ )
		//{
		//	m_vecLockedObjects[i]->GetModelGroup()->StopModelAnimation();
		//	m_vecLockedObjects[i]->GetModelGroup()->SetAnimationByAgpFileName(aniname);
		//}
	}

	void CToolSetModel::SetBakingColorIsNotGreaterShadow()
	{
		if( CEditStateOpertiaonMgr::GetInst()->GetEditState() != CEditStateOpertiaonMgr::EEDIT_STATE::EES_SET_TILE_FACE )
			return;

		bool   bBakingColorMark = false;
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			bBakingColorMark = m_vecLockedObjects[i]->GetModelGroup()->GetBakingColorIsNotGreaterShadow();
			m_vecLockedObjects[i]->GetModelGroup()->SetBakingColorIsNotGreaterShadow(!bBakingColorMark);
		}
	}

	void CToolSetModel::SwitchModelIsUseCameraCollison()
	{
		if( CEditStateOpertiaonMgr::GetInst()->GetEditState() != CEditStateOpertiaonMgr::EEDIT_STATE::EES_SET_TILE_FACE )
			return;

		bool bUseCameraCollsion = false;
		size_t size = m_vecLockedObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			bUseCameraCollsion = m_vecLockedObjects[i]->GetModelGroup()->GetIsUseCameraCollison();
			m_vecLockedObjects[i]->GetModelGroup()->SetIsUseCameraCollison(!bUseCameraCollsion);
		}
	}

	void CToolSetModel::DeleteSceneObjects()
	{
		if ( IDYES == MessageBox(NULL, "是否要删除场景中所有物体", "YES删除", MB_YESNO) )
		{
			CTerrainMesh * pTerrainMesh = NULL;
			pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
			if( pTerrainMesh == NULL )
				return;

 			int nTerrainGrids = pTerrainMesh->GetGridCount();
 			for ( int i = 0; i < nTerrainGrids; ++i )
 			{
 				SGrid * pGrid = &pTerrainMesh->GetGrid(i);

				if ( !pGrid->vecCenterObjects.empty())
				{
					size_t objectCnt = pGrid->vecCenterObjects.size();
					for ( size_t j = 0; j < objectCnt; ++j)
					{
						CTObjectPtr p = pGrid->vecCenterObjects[j];

						if ( p->GetModelGroup()->GetIsEffectBased() )
							p->GetModelGroup()->DeleteLinkEffect();
						
						p->GetModelGroup()->SetEditState(sqr::EES_DELETE);
					}

					pGrid->vecCenterObjects.clear();
				}
 			}

			MessageBox(0, "删除所有物体完毕", "提示", 0);
		}

	}

	void CToolSetModel::LockSelectObjects()
	{
		if ( m_eCopyType == ECT_OBJECT || m_eCopyType == ECT_ALL )
		{
			vector<int> vecBrushGridIndices = vecBrushGridIndices = this->GetSelectedGridIndices();
			size_t cnt = vecBrushGridIndices.size();
			vector< CTObjectPtr> vecLockedObjects;
			CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

			for ( size_t n = 0; n < cnt; ++n )
			{
				SGrid & grid = pTerrainMesh->GetGrid(vecBrushGridIndices[n]);

				if ( !grid.vecCenterObjects.empty() )
				{
					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
					{
						vecLockedObjects.push_back(grid.vecCenterObjects[j]);
					}
				} 
			}

			if( vecLockedObjects.empty() )
				return;

			size_t size = m_vecLockedObjects.size();
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
			}

			m_vecLockedObjects = vecLockedObjects;

			size = m_vecLockedObjects.size();
			for ( size_t i = 0; i < size; ++i )
			{
				m_vecLockedObjects[i]->GetModelGroup()->SetLocked(true);
			}
		}
	}

	void CToolSetModel::GetObjectIntersectedPosHeight()
	{
		if( m_vecLockedObjects.empty() )
			return;

		POINT pt;
		MSG_MOUSE_INF mouse_pos = CEventState::GetInst()->GetMousePosition();
		pt.x = mouse_pos.X;
		pt.y = mouse_pos.Y;
	
		float fHeight = m_vecLockedObjects[0]->GetModelGroup()->GetSelfPresentation(0)->GetIntersectedPosHeight(pt);	
		CEditStateOpertiaonMgr::GetInst()->SetMeshLogicHeight(fHeight);
	}

	void CToolSetModel::GetHotModelInfo( string& name )
	{
		CEditModelGroup *pModelGroup = NULL;
		CTObject *pObject = CMapEditApp::GetInst()->GetOperator()->GetObjectHot();
		if( pObject )
		{
			pModelGroup = pObject->GetModelGroup();

			if( pModelGroup != NULL )
				name = pModelGroup->GetItemName();
		}
	}

	void CToolSetModel::SwitchLinkModelEffect()
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		int nGridCnt = pTerrain->GetGridCount();
		static bool bHideSceneEffect = false;

		bHideSceneEffect = !bHideSceneEffect;

		if( bHideSceneEffect )
		{
			CMapEditApp::GetInst()->GetOperator()->HideAll();

			for (int i = 0; i < nGridCnt; ++i )
			{
				const SGrid &grid = pTerrain->GetGrid(i);
				if ( !grid.vecCenterObjects.empty() )
				{
					int ngridIndex = grid.dwGridIndex;
					int ngridsetIndex = 0;
					for ( vector< CTObjectPtr >::const_iterator iter = grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter )
					{
						CTObject * p = (*iter);
						CEditModelGroup *pModelGroup = p->GetModelGroup();

						string strEffectName = "";
						pModelGroup->GetEffectName(strEffectName);
						if( !strEffectName.empty() )
							pModelGroup->DeleteLinkEffect();
					}
				}
			}
		}
		else
		{
			CMapEditApp::GetInst()->GetOperator()->ShowAll();

			//ReLinkEffect
			for (int i = 0; i < nGridCnt; ++i )
			{
				const SGrid &grid = pTerrain->GetGrid(i);
				if ( !grid.vecCenterObjects.empty() )
				{
					int ngridIndex = grid.dwGridIndex;
					int ngridsetIndex = 0;
					for ( vector< CTObjectPtr >::const_iterator iter = grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter )
					{
						CTObject * p = (*iter);
						CEditModelGroup *pModelGroup = p->GetModelGroup();

						string strEffectName = "";
						pModelGroup->GetEffectName(strEffectName);
						if( !strEffectName.empty() )
							pModelGroup->ReLinkEffect();
					}
				}
			}
		}
	}

	void CToolSetModel::MoveHotObject()
	{
		CTObject * p = CMapEditApp::GetInst()->GetOperator()->GetObjectHot();
		if ( p )
		{
			if ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN )
			{
				// move it if left shift is down
				CCmdMoveObject * pCmd = NULL;
				pCmd = new CCmdMoveObject;
				if( pCmd == NULL )
					return;

				int nCenterGridIndex = p->GetCenterGridIndex();
				pCmd->SetCenterGridIndex(nCenterGridIndex);
				pCmd->SetObject(p);

				CVector3f vGroupOffset = CVector3f(0.0f,0.0f,0.0f);

				pCmd->SetGroupOffset(vGroupOffset);

				pCmd->Execute();

				for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
				{
					m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
				}

				m_vecLockedObjects.clear();
			}
			else
			{
				if( m_bAlwaysSelectthisObject )
					return;

				for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
				{
					m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
				}

				m_vecLockedObjects.clear();
				m_vecLockedObjects.push_back(p);
				p->GetModelGroup()->SetLocked(true);
			}
		}
		else // unhot
		{
			if( m_bAlwaysSelectthisObject )
				return;

			if ( !m_vecLockedObjects.empty() )
			{
				for ( size_t i = 0; i < m_vecLockedObjects.size(); ++i )
				{
					m_vecLockedObjects[i]->GetModelGroup()->SetLocked(false);
				}

				m_vecLockedObjects.clear();
			}
		}
	}
}
