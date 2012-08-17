#include "StdAfx.h"
#include "Brush/TerrainModelBrush.h"
#include "Brush/BaseBrushDraw.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "MapCommand/CmdBornObject.h"
#include "MapCommand/CmdPlaceObject.h"
#include "MapCommand/CmdDeleteObject.h"
#include "CEventState.h"
#include "CSceneBoxDeubg.h"

const float SCALE_STEP		= 0.2f;
const float ROTATION_STEP	= 3.141592654f*2.0f/256.0f;
const float MOVE_STEP		= 1.0f;
const float QUICK_MOVE_STEP	= 10.0f;

//////////////////////////////////////////////////////////////////////////
struct MultiSelectContex
{
	vector<CRenderNode*>	m_vecSelectObjList;
	float xMin;
	float zMin;
	float xMax;
	float zMax;
	MultiSelectContex(void)
	{
		xMin	= 0.0f;
		zMin	= 0.0f;
		xMax	= 0.0f;
		zMax	= 0.0f;
	}
};

void MultiSelectFun(CRenderNode* pChild, void* param)
{
	MultiSelectContex* pContex = (MultiSelectContex*)param;
	if( pChild == NULL || pContex == NULL )
		return;

	CRenderNode *pCurObj = pChild;
	CVector3f position = pCurObj->GetPosition();

	if( position.x >= pContex->xMin && position.z >= pContex->zMin && 
		position.x <= pContex->xMax && position.z <= pContex->zMax )
		pContex->m_vecSelectObjList.push_back(pCurObj);
}
//////////////////////////////////////////////////////////////////////////

CTerrainModelBrush::CTerrainModelBrush(void) : m_eActiveModelState(EES_NONE), m_pSceneBoxDebuger(NULL)
{

}

CTerrainModelBrush::~CTerrainModelBrush(void)
{
	if( m_pSceneBoxDebuger )
		SafeDelete(m_pSceneBoxDebuger);

	ClearActiveObject();
}

void CTerrainModelBrush::MouseMoveEvent(MSG_MOUSE_INF mouse)
{
	CBaseBrush::MouseMoveEvent(mouse);

	if( m_vecActiveModels.empty() )
		return;

	if( m_eActiveModelState == EES_MOVING )
	{
		size_t size = m_vecActiveModels.size();
		for ( size_t m = 0; m < size; ++m )
		{
			m_vecActiveModels[m]->SetPosition(m_vecBrushCenter);
		}
	}
}

void CTerrainModelBrush::LButtonUpEvent(MSG_MOUSE_INF mouse)
{
	if( !m_vecActiveModels.empty() )
	{
		PlaceActiveModel();
	}
	else
	{
		m_vecLockModels.clear();

		CIRect rect;
		m_pContext->GetWindowTarget()->GetRect(rect);

		int	width  = rect.right - rect.left;
		int	height = rect.bottom - rect.top;
		float	x  = float(m_mouseX)/float(width);
		float	y  = float(m_mouseY)/float(height);

		CRenderNode* pNode = m_pContext->SelectModelByCursor(x, y);
		CRenderObject *pSelectObject = NULL;
		if( pNode && pNode->GetNodeType() == CRenderObject::st_RenderObjectType )
		{
			pSelectObject = (CRenderObject*)pNode;
			CEditModel *pEditModel = (CEditModel *)pSelectObject;
			m_vecLockModels.push_back(pEditModel);
		}
	}
}

void CTerrainModelBrush::CtrlKeyDownEvent(MSG_KEY_INF key)
{
	switch( key )
	{
	case MGI_KEY_C:	
		{
			//copy model
			CopySelectModels();
		}
		break;

	case MGI_KEY_V:	
		{
			//paste model
			PasteSelectModels();
		}
		break;

	case MGI_KEY_F6:
		{
			OnSwitchUseCameraCollision();
			break;
		}

	case MGI_KEY_D1:
		{
			SelectModelsByBrush();
			break;
		}
	}
}

void CTerrainModelBrush::LShiftKeyDownEvent(MSG_KEY_INF key)
{
 	if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
 	{
 		//将模型黏在鼠标上移动
 		if( !m_vecLockModels.empty() )
 		{
 			m_vecActiveModels    = m_vecLockModels;
			m_eActiveModelState	 = EES_MOVING;

			size_t size = m_vecActiveModels.size();
			for ( size_t m = 0; m < size; ++m )
			{
				m_pContext->AddToRenderScene(m_vecActiveModels[m]);
			}
			
 		}
 	}
}

void CTerrainModelBrush::MoveLButtonDownEvent(MSG_MOUSE_INF mouse)
{
	return;
}

bool CTerrainModelBrush::KeyDownEvent( MSG_KEY_INF key )
{
	if ( ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN ) )
	{
		CtrlKeyDownEvent(key);
		return false;
	}

	bool bContinue = false;
	switch( key )
	{
	case MGI_KEY_D6:///revert model size
		NormalModelSize();
		break;

	case MGI_KEY_D7:///enlarge model
		ZoomInModelSize();
		break;

	case MGI_KEY_D8:///small model
		ZoomOutModelSize();
		break;

	case MGI_KEY_Delete:
		DelectLockObject();
		break;

		///x rotate +
	case MGI_KEY_R:
		RotateModelAxisX(true);
		break;

		///x rotate -
	case MGI_KEY_F:
		RotateModelAxisX(false);
		break;

		///y rotate +
	case MGI_KEY_I:
		RotateModelAxisY(true);
		break;

		///y rotate -
	case MGI_KEY_K:
		RotateModelAxisY(false);
		break;

		///Z rotate +
	case MGI_KEY_Y:
		RotateModelAxisZ(true);
		break;

		///Z rotate -
	case MGI_KEY_H:
		RotateModelAxisZ(false);
		break;

		///垂直旋转90度
	case MGI_KEY_T:
		ApeakRotateModel();
		break;

		///恢复旋转信息
	case MGI_KEY_G:
		RevertModelRotation();
		break;

		///move left
	case MGI_KEY_Left:
		MoveModelAxisX(false);
		break;

		///move right
	case MGI_KEY_Right:
		MoveModelAxisX(true);
		break;

		///move in
	case MGI_KEY_Up:
		MoveModelAxisZ(false);
		break;

		///move out
	case MGI_KEY_Down:
		MoveModelAxisZ(true);
		break;

		///move down
	case MGI_KEY_Next:
		MoveModelAxisY(false, MOVE_STEP);
		break;

		///move up
	case MGI_KEY_Prior:
		MoveModelAxisY(true, MOVE_STEP);
		break;

		///quick move down
	case MGI_KEY_End:
		MoveModelAxisY(false, QUICK_MOVE_STEP);
		break;

		///quick move up
	case MGI_KEY_Home:
		MoveModelAxisY(true, QUICK_MOVE_STEP);
		break;
		
		///下一个渲染状态
	case MGI_KEY_F9:
		OnNextRenderStyle();
		break;

		///设置阴影
	case MGI_KEY_F6:
		OnSwitchShadow();
		break;

		///切换单个物体接收阴影
	case MGI_KEY_F5:
		OnSwitchReceiveShadow();
		break;

	case MGI_KEY_F11:
		OnPlayAniOnOff();
		break;

	default:
		bContinue = CBaseBrush::KeyDownEvent(key);
		break;
	}

	return bContinue;
}

void CTerrainModelBrush::KeyUpEvent( MSG_KEY_INF key )
{
}

void CTerrainModelBrush::CreateActiveModel( const vector<string>& vecEditModelFileNames, const string& strAniName, const string& strEffectFileName, const string& strEffectName, bool bVisibleByConfigure )
{
	m_vecActiveModels.clear();
	
	CCmdBornObject *pCmd = new CCmdBornObject(m_pContext);
	
	pCmd->SetTerrainBrush(this);
	pCmd->SetEditModelFileNames(vecEditModelFileNames);
	pCmd->SetEditModelAgpName(strAniName);
	pCmd->SetEditModelEffectInfo(strEffectFileName, strEffectName);
	pCmd->SetIsVisibleByConfigure(bVisibleByConfigure);

	pCmd->Execute();
}

void CTerrainModelBrush::SetActiveModel( CEditModel* pActiveModel )
{
	m_vecActiveModels.push_back(pActiveModel);
}

void CTerrainModelBrush::SetLockModel(CEditModel* pLockModel)
{
	m_vecLockModels.push_back(pLockModel);
}

void CTerrainModelBrush::ShowLockModeWireBox()
{
	m_pSceneBoxDebuger = new CSceneBoxDebugNode(m_pContext);
	m_pContext->RegistDebugNode(m_pSceneBoxDebuger);
}

void CTerrainModelBrush::PlaceActiveModel()
{
	size_t size = m_vecActiveModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CCmdPlaceObject *pCmd = new CCmdPlaceObject(m_pContext);

		pCmd->SetTerrainBrush(this);
		pCmd->SetActiveModel(m_vecActiveModels[m]);
		pCmd->SetActiveModelPosition(m_vecBrushCenter);

		pCmd->Execute();
	}
	m_vecActiveModels.clear();
}

void CTerrainModelBrush::ZoomInModelSize()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		float fScale = pEditModel->GetScale();
		fScale += SCALE_STEP;

		pEditModel->SetScale(fScale);
	}
}

void CTerrainModelBrush::ZoomOutModelSize()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		float fScale = pEditModel->GetScale();
		float newScale = fScale - SCALE_STEP;
		if( newScale > 0.0f )
			fScale = newScale;

		pEditModel->SetScale(fScale);
	}
}

void CTerrainModelBrush::NormalModelSize()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		m_vecLockModels[m]->SetScale(1.0f);
	}
}

void CTerrainModelBrush::RotateModelAxisX(const bool bPositive)
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f rotate = pEditModel->GetModelRotate();
		rotate.x = bPositive ? rotate.x + ROTATION_STEP : rotate.x - ROTATION_STEP;

		pEditModel->RotateX(rotate.x);
		pEditModel->RotateZ(rotate.y);
		pEditModel->RotateY(rotate.z);
	}
}

void CTerrainModelBrush::RotateModelAxisY(const bool bPositive)
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f rotate = pEditModel->GetModelRotate();
		rotate.y = bPositive ? rotate.y + ROTATION_STEP : rotate.y - ROTATION_STEP;

		pEditModel->RotateX(rotate.x);
		pEditModel->RotateZ(rotate.y);
		pEditModel->RotateY(rotate.z);
	}
}

void CTerrainModelBrush::RotateModelAxisZ(const bool bPositive)
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f rotate = pEditModel->GetModelRotate();
		rotate.z = bPositive ? rotate.z + ROTATION_STEP : rotate.z - ROTATION_STEP;

		pEditModel->RotateX(rotate.x);
		pEditModel->RotateZ(rotate.y);
		pEditModel->RotateY(rotate.z);
	}
}

void CTerrainModelBrush::RevertModelRotation()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->RotateX(0.0f);
		pEditModel->RotateZ(0.0f);
		pEditModel->RotateY(0.0f);
	}
}

void CTerrainModelBrush::ApeakRotateModel()
{
	float fRotate = CMath::ToRadian(90);
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->RotateX(0.0f);
		pEditModel->RotateZ(0.0f);
		pEditModel->RotateY(fRotate);
	}
}

void CTerrainModelBrush::MoveModelAxisX(const bool bPositive)
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f pos = pEditModel->GetPosition();
		pos.x = bPositive ? pos.x + MOVE_STEP : pos.x - MOVE_STEP;

		pEditModel->SetPosition(pos);
	}
}

void CTerrainModelBrush::MoveModelAxisY(const bool bPositive, const float fStep )
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f pos = pEditModel->GetPosition();
		pos.y = bPositive ? pos.y + fStep : pos.y - fStep;

		pEditModel->SetPosition(pos);
	}
}

void CTerrainModelBrush::MoveModelAxisZ(const bool bPositive)
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		CVector3f pos = pEditModel->GetPosition();
		pos.z = bPositive ? pos.z + MOVE_STEP : pos.z - MOVE_STEP;

		pEditModel->SetPosition(pos);
	}
}


void CTerrainModelBrush::OnNextRenderStyle()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];

		for ( GMap< GString, CRenderPiece >::iterator it = pEditModel->GetRenderPieces().begin(); it != pEditModel->GetRenderPieces().end(); ++it )
		{
			uint32 uMaxRSNum = 0;

			CRenderPiece & RenderPiece = it->second;
			GString strPieceName = it->first;

			for ( size_t i = 0; i < RenderPiece.m_Pieces.size(); ++i )
			{
				::CRenderPiece::PieceData * pPieceData = &RenderPiece.m_Pieces[i];
				CPiece * pPiece = pPieceData->m_pPiece;
				uMaxRSNum = max(uMaxRSNum, pPiece->GetRSNum());
			}

			RenderPiece.m_RenderStyle = (RenderPiece.m_RenderStyle+1)%uMaxRSNum;

			pEditModel->SetPieceRSIndex(strPieceName.c_str(), RenderPiece.m_RenderStyle);
		}
	}
}

void CTerrainModelBrush::OnSwitchShadow()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->SetIsCastShadow(pEditModel->GetIsCastShadow());
	}
}

void CTerrainModelBrush::OnSwitchReceiveShadow()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->SetIsReceiveShadow(pEditModel->GetIsReceiveShadow());
	}
}

void CTerrainModelBrush::OnPlayAniOnOff()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->SetIsPlayAnimation(pEditModel->GetIsPlayAnimation());
	}
}

void CTerrainModelBrush::OnSwitchUseCameraCollision()
{
	if( m_vecLockModels.empty() )
		return;

	size_t size = m_vecLockModels.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pEditModel = m_vecLockModels[m];
		pEditModel->SetIsUseCameraCollision(pEditModel->GetIsUseCameraCollision());
	}
}

void CTerrainModelBrush::SelectModelsByBrush()
{
	CBaseBrushDraw::brushVertexsPool vertexIndexs;
	m_pBaseBrushDraw->GetBrushVertexIndexs(vertexIndexs);
	if( vertexIndexs.empty() )
		return;

	size_t size = vertexIndexs.size();
	CVector3f leftbottom = vertexIndexs[0];
	CVector3f righttop   = vertexIndexs[size-1];
	MultiSelectContex Contex;
	if( leftbottom.x < righttop.x )
	{
		Contex.xMin = leftbottom.x;
		Contex.xMax = righttop.x;
	}
	else
	{
		Contex.xMin = righttop.x;
		Contex.xMax = leftbottom.x;
	}
	if( leftbottom.z < righttop.z )
	{
		Contex.zMin = leftbottom.z;
		Contex.zMax = righttop.z;
	}
	else
	{
		Contex.zMin = righttop.z;
		Contex.zMax = leftbottom.z;
	}

	URegionID id = GetRegionID(m_vecBrushCenter);
	m_vecLockModels.clear();

	m_pContext->GetEditDataScene()->GetTreeScene()->DoCtrl(MultiSelectFun, &Contex);
	
	size = Contex.m_vecSelectObjList.size();
	for ( size_t m = 0; m < size; ++m )
	{
		CRenderNode* pNode = Contex.m_vecSelectObjList[m];
		CRenderObject *pSelectObject = NULL;
		if( pNode && pNode->GetNodeType() == CRenderObject::st_RenderObjectType )
		{
			pSelectObject = (CRenderObject*)pNode;
			CEditModel *pEditModel = (CEditModel *)pSelectObject;
			m_vecLockModels.push_back(pEditModel);
		}
	}
}

void CTerrainModelBrush::CopySelectModels()
{

}

void CTerrainModelBrush::PasteSelectModels()
{

}

void CTerrainModelBrush::ClearActiveObject()
{
	m_vecActiveModels.clear();
	m_vecLockModels.clear();
	m_eActiveModelState  = EES_NONE;
}

void CTerrainModelBrush::DelectLockObject()
{

}



