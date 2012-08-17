#include "stdafx.h"
#include "Render.h"
#include "CMapEditObject.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "MEMath.h"
#include "Picker.h"
#include "WireBox.h"
#include "CEditModel.h"
#include "CMEModel.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "IEffect.h"
#include "IEffectGroup.h"
#include "CEffectGroup.h"
#include "CGraphic.h"
#include "SceneAudioMgr.h"
#include "CSkeletalsMgr.h"
#include "EditStateOpertiaonMgr.h"
#include "CEditContext.h"
#include "CTreeScene.h"
#include "CEditDataScene.h"
#include "CEditorCore.h"

const float MatRGBWeight = 128.0f / 255.0f;
float		fModelQuickLift = 0.0f;
float		fModelNormalLift = 0.0f;
const CMatrix MirrorZMatrix(  -1.0f, 0.0f, 0.0f, 0.0f, 
							  -0.0f, 1.0f, 0.0f, 0.0f,
							  -0.0f, 0.0f, 1.0f, 0.0f,
							   0.0f, 0.0f, 0.0f, 1.0f);
const CMatrix MirrorXMatrix(  1.0f, 0.0f, -0.0f, 0.0f, 
							  0.0f, 1.0f, -0.0f, 0.0f,
							  0.0f, 0.0f, -1.0f, 0.0f,
							  0.0f, 0.0f,  0.0f, 1.0f);
using namespace sqr_tools;
namespace sqr
{
	CMapEditObject::CMapEditObject()
	{
		m_strSceneCueName	= "";

		fModelQuickLift		= 10;
		fModelNormalLift	= 1;

		m_pObjectDialog		= NULL;
		m_pGroup			= NULL;

		m_IsAsynLoadPiece	= false;

		m_pWireBox			= NULL;
		m_pTempWireBox		= NULL;
	}

	CMapEditObject::~CMapEditObject()
	{
		this->Release();
	}

	void CMapEditObject::Release()
	{
		CEditModel::SetVisible(false);
		m_pAEmitter = NULL;

		if ( m_pWireBox )
		{
			m_pWireBox = NULL;
		}
		if ( m_pTempWireBox )
			m_pTempWireBox = NULL;
	}

	void CMapEditObject::SetCenterType(ECenter_Type eCenterType)
	{
		sModelCommonPro.eCenterType = eCenterType;
	}

	void CMapEditObject::SetCenterTypeOrigin(ECenter_Type eCenterType)
	{
		sModelCommonPro.eCenterType = eCenterType;
	}

	sqr::ECenter_Type CMapEditObject::GetCenterType()
	{
		return sModelCommonPro.eCenterType;
	}

	bool CMapEditObject::Create(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, 
							   float fLiftEx, float fShiftXEx, float fShiftZEx, 
							   float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, 
							   float xScale, float yScale, float zScale)
	{
		if ( strFileName == "" )
		{
			MessageBox(NULL, "模型名为空，请检查", "提示", MB_OK);
			return false;
		}

 		sModelCommonPro.bFixedHeight= false;
 		sModelCommonPro.strFileName	= strFileName;
 		sModelCommonPro.bSubUnit	= bSubUnit;
 		sModelCommonPro.fLift		= fLift;
 		sModelCommonPro.fShiftX		= fShiftX;
 		sModelCommonPro.fShiftZ		= fShiftZ;

		sModelSavePro.vUserOffsetEx = CVector3f(fShiftXEx, fLiftEx, fShiftZEx);
 
 		sModelSavePro.nRotationCnt	= nRotationCnt;
 		sModelSavePro.nPitchCnt		= nPitchCnt;
 		sModelSavePro.nYawCnt		= nYawCnt;
		sModelSavePro.vRotate = CVector3f(fYaw, fRotation, fPitch);

		sModelSavePro.vScale = CVector3f(xScale, yScale, zScale);
 
 		sModelCommonPro.matUserOffset.SetTranslate(sModelCommonPro.fShiftX, sModelCommonPro.fLift, sModelCommonPro.fShiftZ);

 		CMatrix matTemp;
 		CMatrix matTemp1;

 		//matTemp.SetRotateZ(fPitch);
 		//matTemp1.SetRotateX(fYaw);
 		//sModelCommonPro.matRotY.SetRotateY(fRotation);
 		//sModelCommonPro.matRotY = matTemp  * sModelCommonPro.matRotY;
 		//sModelCommonPro.matRotY = matTemp1 * sModelCommonPro.matRotY;
 
 		//sModelCommonPro.matScale.SetScale(xScale, yScale, zScale);
 
 		CGraphic * pCoreGraphic = CGraphic::GetInst();

		//if(strFileName.find("effectdummy") == -1)
		//{
 			//SQR_TRY
 			//{
 			//	if ( pCoreGraphic->CreatePieceGroup(strFileName.c_str(), &m_pPieceGroup) == INVALID )
 			//	{
 			//		string log = strFileName;
 			//		log = log + "模型没有找到，可能该文件不存在";
	 
 			//		MessageBox(NULL, log.c_str(), "错误", MB_OK);
	 
				//	CMapEditApp::GetInst()->savelog += strFileName + "\n";
				//	if ( !CMapEditApp::GetInst()->GetOperator()->m_bLoadModel )
				//		CMapEditApp::GetInst()->m_bSave = false;
	 		//		
				//	return false;
 			//	}
 			//}
 			//SQR_CATCH(exp)
 			//{
 			//	if ( !CMapEditApp::GetInst()->GetOperator()->m_bLoadModel )
 			//		CMapEditApp::GetInst()->m_bSave = false;
	 
 			//	CMapEditApp::GetInst()->savelog += strFileName + "\n";
	 
 			//	MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
 			//	return false;
 			//}
 			//SQR_TRY_END;


		//	for ( size_t i = 0; i < m_pPieceGroup->GetPieceClassNum(); i++)
		//	{
		//		CEditModel::AddMeshPiece(m_pPieceGroup,m_pPieceGroup->GetPieceClassName(i),0);
		//	}

		//	m_pPieceGroup->Release();
		//}
		CEditModel::AddPiece(RFT_ARP,strFileName.c_str(),NULL,NULL);
 		CEditModel::GetScaleDimension(0,sModelCommonPro.nWidthDimInGrids,sModelCommonPro.nDepthDimInGrids,GRID_SPACE);
 
 		if ( xScale > 1.0f )
 			sModelCommonPro.nWidthDimInGrids *= ( (int)( ( xScale/GRID_SPACE )/2 + 0.5f ) )*2 + 1;
 		if ( zScale > 1.0f )
 			sModelCommonPro.nDepthDimInGrids *= ( (int)( ( zScale/GRID_SPACE )/2 + 0.5f ) )*2 + 1;
 
 		sModelCommonPro.fRedOverallAmbientRatio = 128.0f/255.0f;
 		sModelCommonPro.fBlueOverallAmbientRatio = 128.0f/255.0f;
 		sModelCommonPro.fGreenOverallAmbientRatio = 128.0f/255.0f;
 
 		sModelCommonPro.nWidthRadius = sModelCommonPro.nWidthDimInGrids/2 +1;
 		sModelCommonPro.nDepthRadius = sModelCommonPro.nDepthDimInGrids/2 +1;

		m_pWireBox = CSceneManager::GetInst()->AddNodeWireBox(sModelCommonPro.fHeight, sModelCommonPro.fWidth, sModelCommonPro.fDepth, 0xff0000ff, false, NULL);

		return true;
	}

	bool CMapEditObject::Create(const string& strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, const CVector3f& vUserOffsetEx, const CVector3f& vRotate, 
								int nRotationCnt, int nPitchCnt, int nYawCnt, const CVector3f& vScale )
	{
		if ( strFileName == "" )
		{
			MessageBox(NULL, "模型名为空，请检查", "提示", MB_OK);
			return false;
		}

		sModelCommonPro.bFixedHeight	= false;
		sModelCommonPro.strFileName		= strFileName;
		sModelCommonPro.bSubUnit		= bSubUnit;
		sModelCommonPro.fLift			= fLift;
		sModelCommonPro.fShiftX			= fShiftX;
		sModelCommonPro.fShiftZ			= fShiftZ;

		sModelSavePro.vUserOffsetEx		= vUserOffsetEx;
		sModelSavePro.vRotate			= vRotate;
		sModelSavePro.vScale			= vScale;

		sModelSavePro.nRotationCnt		= nRotationCnt;
		sModelSavePro.nPitchCnt			= nPitchCnt;
		sModelSavePro.nYawCnt			= nYawCnt;

		sModelCommonPro.matUserOffset.SetTranslate(fShiftX, fLift, fShiftZ);

		CMatrix matTemp;
		CMatrix matTemp1;

		m_Quaternion.setIdentity();

		RotateX(vRotate.x);
		RotateZ(vRotate.z);
		RotateY(vRotate.y);

		setScale(vScale.x, vScale.y, vScale.z);

		CGraphic * pCoreGraphic = CGraphic::GetInst();


		//if(strFileName.find("effectdummy") == -1)
		//{
		//	SQR_TRY
		//	{
		//		if ( pCoreGraphic->CreatePieceGroup(strFileName.c_str(), &m_pPieceGroup) == INVALID )
		//		{
		//			string log = strFileName;
		//			log = log + "模型没有找到，可能该文件不存在";


		//			MessageBox(NULL, log.c_str(), "错误", MB_OK);

		//			CMapEditApp::GetInst()->savelog += strFileName + "\n";
		//			if ( !CMapEditApp::GetInst()->GetOperator()->m_bLoadModel )
		//				CMapEditApp::GetInst()->m_bSave = false;

		//			return false;
		//		}
		//	}
		//	SQR_CATCH(exp)
		//	{
		//		if ( !CMapEditApp::GetInst()->GetOperator()->m_bLoadModel )
		//			CMapEditApp::GetInst()->m_bSave = false;

		//		CMapEditApp::GetInst()->savelog += strFileName + "\n";

		//		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
		//		return false;
		//	}
		//	SQR_TRY_END;

		//	for ( size_t i = 0; i < m_pPieceGroup->GetPieceClassNum(); i++)
		//	{
		//		CEditModel::AddMeshPiece(m_pPieceGroup,m_pPieceGroup->GetPieceClassName(i),0);
		//	}

		//	m_pPieceGroup->Release();
		//}
		AddPiece(RFT_ARP,strFileName.c_str(),NULL,NULL);

		CEditModel::GetScaleDimension(0,sModelCommonPro.nWidthDimInGrids,sModelCommonPro.nDepthDimInGrids,GRID_SPACE);

		if ( vScale.x > 1.0f )
			sModelCommonPro.nWidthDimInGrids *= ( (int)( ( vScale.x/GRID_SPACE )/2 + 0.5f ) )*2 + 1;
		if ( vScale.z > 1.0f )
			sModelCommonPro.nDepthDimInGrids *= ( (int)( ( vScale.z/GRID_SPACE )/2 + 0.5f ) )*2 + 1;

		sModelCommonPro.fRedOverallAmbientRatio   = 128.0f/255.0f;
		sModelCommonPro.fBlueOverallAmbientRatio  = 128.0f/255.0f;
		sModelCommonPro.fGreenOverallAmbientRatio = 128.0f/255.0f;

		sModelCommonPro.nWidthRadius = sModelCommonPro.nWidthDimInGrids/2 +1;
		sModelCommonPro.nDepthRadius = sModelCommonPro.nDepthDimInGrids/2 +1;

		m_pWireBox = CSceneManager::GetInst()->AddNodeWireBox(sModelCommonPro.fHeight, sModelCommonPro.fWidth, sModelCommonPro.fDepth, 0xff0000ff, false, NULL);

		return true;
	}

	void CMapEditObject::IncAtten()
	{
		++sModelCommonPro.nWidthRadius; 
		++sModelCommonPro.nDepthRadius;
	}

	void CMapEditObject::DecAtten()
	{
		--sModelCommonPro.nWidthRadius;
		--sModelCommonPro.nDepthRadius;
	}

	void CMapEditObject::SetAttenRadius(int nWidth, int nDepth)
	{
		sModelCommonPro.nDepthRadius = nDepth;
		sModelCommonPro.nWidthRadius = nWidth;
	}

	void CMapEditObject::GetAttenRadius(int & nWidth, int & nDepth)
	{
		nDepth = sModelCommonPro.nDepthRadius;
		nWidth = sModelCommonPro.nWidthRadius;
	}

	void CMapEditObject::RecaculateSelfAmbientRatio(float & fRed, float & fGreen, float & fBlue)
	{
		if ( sModelCommonPro.nDepthRadius <= 0 || sModelCommonPro.nWidthRadius <= 0 )
		{
			fRed = fGreen = fBlue = 1.0f;
			return;
		}

		CVector3f v(sModelCommonPro.fShiftX+sModelSavePro.vUserOffsetEx.x,0.0f,sModelCommonPro.fShiftZ+sModelSavePro.vUserOffsetEx.z);
		CMatrix matRotY;
		CMatrix matTemp;
		CMatrix matTemp1;
		matTemp.SetRotateZ(sModelSavePro.vRotate.z);
		matTemp1.SetRotateX(sModelSavePro.vRotate.x);
		matRotY.SetRotateY(sModelSavePro.vRotate.y);
		matRotY = matTemp * matRotY;
		matRotY = matTemp1 * matRotY;
		v *= matRotY;

		v = v + sModelCommonPro.vBlockCenter;

		int nDeltaX = int(v.x / GRID_SPACE);
		int nDeltaZ = int(v.z / GRID_SPACE);

		int nCenterGridIndex = nDeltaZ*CMapEditApp::GetInst()->GetEditingMesh()->GetWidth() + nDeltaX;
		nCenterGridIndex = this->sModelSavePro.nCenterGridIndex;

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		vector<DWORD> vecGridIndices;

		if ( pTerrain->IsValid(nCenterGridIndex) )
		{
			int nGridIndex = -1;
			for ( int i=-1; i<=1; ++i )
			{
				for ( int j=-1; j<=1; ++j )
				{
					nGridIndex = nCenterGridIndex + i*pTerrain->GetWidth() + j;
				}

				if ( pTerrain->IsValid(nGridIndex) )
				{
					vecGridIndices.push_back(nGridIndex);
				}
			}
		}

		DWORD Red = 0;
		DWORD Green = 0;
		DWORD Blue = 0;

		size_t vectorSize = vecGridIndices.size();

		for ( size_t i = 0; i < vectorSize; ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecGridIndices[i]);

			// 场景物体是否受材质色也即（顶点色）影响
			bool bEffectGridModel = false;
#ifdef USE_OLD_LIGHT_MODEL
			bEffectGridModel = grid.bEffectByVertexColor;
#else
			bEffectGridModel = true;
#endif

			if ( bEffectGridModel )
			{
				for ( int k = 0; k < 4; ++k )
				{
					sqr::SVertex & vertex = pTerrain->GetVertex(grid.dwVertexIndex[k]);
					Red += ((BYTE*)(&vertex.oColor))[2];
					Green += ((BYTE*)(&vertex.oColor))[1];
					Blue += ((BYTE*)(&vertex.oColor))[0];
				}
			}
			else
			{
				for ( int k = 0; k < 4; ++k )
				{
					Red += 0x80;
					Green += 0x80;
					Blue += 0x80;
				}
			}
		}

		fRed = Red/(4*vectorSize*255.0f);
		fGreen = Green/(4*vectorSize*255.0f);
		fBlue = Blue/(4*vectorSize*255.0f);
	}

	void CMapEditObject::GetOverallAmbientRatio(byte & uR, byte & uG, byte & uB)
	{
		uR = uint8(sModelCommonPro.fRedOverallAmbientRatio  * 0xff);
		uG = uint8(sModelCommonPro.fGreenOverallAmbientRatio* 0xff);
		uB = uint8(sModelCommonPro.fBlueOverallAmbientRatio * 0xff);
	}

	void CMapEditObject::GetOverallAmbientRatio(float &fR, float &fG, float &fB)
	{
		fR = sModelCommonPro.fRedOverallAmbientRatio;
		fG = sModelCommonPro.fGreenOverallAmbientRatio;
		fB = sModelCommonPro.fBlueOverallAmbientRatio;
	}

	void CMapEditObject::UpdateOverallAmbientRatio(float fRed, float fGreen, float fBlue)
	{
		sModelCommonPro.fRedOverallAmbientRatio		= fRed;
		sModelCommonPro.fGreenOverallAmbientRatio	= fGreen;
		sModelCommonPro.fBlueOverallAmbientRatio	= fBlue;
	}

	void CMapEditObject::UpdateOverallAmbientRatio(const float fRatio)
	{
 		//利用aratio顶点对模型的影响，如果aratio值越小，模型的表面材质mat的个颜色受顶点影响就越小
		sModelCommonPro.fRedOverallAmbientRatio   = MatRGBWeight * fRatio + (1 - fRatio);
		sModelCommonPro.fGreenOverallAmbientRatio = MatRGBWeight * fRatio + (1 - fRatio);
		sModelCommonPro.fBlueOverallAmbientRatio  = MatRGBWeight * fRatio + (1 - fRatio);
	}

	float CMapEditObject::GetIntersectedPosHeight(const POINT& point) 
	{
		float fHeight = 0.0f;

		CCamera *pCamera = CMainWindowTarget::GetInst()->GetCamera();
		CMatrix matProj, matView;
		matProj = pCamera->getProjectionMatrix();
		matView = pCamera->getViewMatrix();

		RECT rc;
		HWND hWnd;
		hWnd = CEngine::GetInst()->GetD3DWindow();
		GetClientRect(hWnd,&rc);

		// Compute the vector of the pick ray in screen space
		CVector3f v;
		v.x =  ( ( ( 2.0f * point.x ) / rc.right  ) - 1 ) / matProj._11;
		v.y = -( ( ( 2.0f * point.y ) / rc.bottom ) - 1 ) / matProj._22;
		v.z =  1.0f;

		// Get the inverse view matrix
		CMatrix m = matView;
		m.Invert();

		// Transform the screen space pick ray into 3D space
		CVector3f vPickRayDir;
		CVector3f vPickRayOrig;
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		vPickRayOrig.x = m._41;
		vPickRayOrig.y = m._42;
		vPickRayOrig.z = m._43;

		for ( GMap< GString, CRenderPiece >::iterator it = CEditModel::GetRenderPieces().begin(); it != CEditModel::GetRenderPieces().end(); it++ )
		{
			uint32 uMaxRSNum = 0;

			CRenderPiece & RenderPiece = it->second;
			GString str = it->first;
			size_t nPieceCnt = RenderPiece.m_Pieces.size();

			for ( size_t i = 0; i < nPieceCnt; ++i )
			{
				CRenderPiece::PieceData * pPieceData = &RenderPiece.m_Pieces[i];
				CPiece * pPiece = pPieceData->m_pPiece;
				CMesh *pMesh = NULL;
				pMesh = pPiece->GetMesh();

				if ( pMesh )
				{
					int nFaceNum  = pMesh->GetFaceNum( 0 );
					VertexData* pVBData = pMesh->GetVertexData();
					IndexData* pVIData = pMesh->GetIndexBuf(0);

					WORD* indexs = (WORD*)pVIData->indexBuffer->lock(HBL_READ_ONLY);

					CVector3f Vec0, Vec1, Vec2, VecDir, intersectVec;
					CVector3f planeP0, planeP1, planeP2;
					
					int indexTemp  = 0, in = -1;

					for ( int num = 0; num < nFaceNum; ++num )
					{
						indexTemp = indexs[++in];
						Vec0 = pMesh->GetVertex(indexTemp);
						planeP0 = Vec0.FastMultiply( sModelCommonPro.mat );
						Vec0 = CVector3f(planeP0.x, planeP0.y, planeP0.z);

						indexTemp = indexs[++in];
						Vec1 = pMesh->GetVertex(indexTemp);
						planeP1 = Vec1.FastMultiply( sModelCommonPro.mat );
						Vec1 = CVector3f(planeP1.x, planeP1.y, planeP1.z);

						indexTemp = indexs[++in];
						Vec2 = pMesh->GetVertex(indexTemp);
						planeP2 = Vec2.FastMultiply( sModelCommonPro.mat );
						Vec2 = CVector3f(planeP2.x, planeP2.y, planeP2.z);
		
						float fPu = 0.0f, fPv = 0.0f, fDis = 0.0f;
						if ( CMath::IntersectTri( Vec0, Vec1, Vec2, vPickRayOrig, vPickRayDir, fPu, fPv, fDis ) )
						{
							intersectVec = Vec0 + fPu*(Vec1 - Vec0) + fPv*(Vec2-Vec0);

							pVIData->indexBuffer->unlock();
							return intersectVec.y;
						}
					}

					pVIData->indexBuffer->unlock();
				}
			}
		}

		return fHeight;
	}

	int CMapEditObject::GetRenderStyle()
	{
		return CMEModel::GetRenderStyle( CEditModel::GetRenderPieces());
	}

	void CMapEditObject::SetHideModelVertexColor(bool b)
	{
		sModelCommonPro.bHideModelVertexColor = b;
	}

	void CMapEditObject::SetRenderStyle(int nRenderStyle)
	{
		CMEModel::SetRenderStyle( CEditModel::GetRenderPieces(), nRenderStyle);
	}

	//bool CMapEditObject::Intersected(CVector3f vDir,CVector3f vPos)
	//{
	//	float u,v,d;

	//	if(		CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[1],sModelCommonPro.vCorners[2],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[2],sModelCommonPro.vCorners[3],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[4],sModelCommonPro.vCorners[5],sModelCommonPro.vCorners[6],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[4],sModelCommonPro.vCorners[6],sModelCommonPro.vCorners[7],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[4],sModelCommonPro.vCorners[7],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[7],sModelCommonPro.vCorners[3],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[1],sModelCommonPro.vCorners[5],sModelCommonPro.vCorners[6],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[1],sModelCommonPro.vCorners[2],sModelCommonPro.vCorners[6],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[1],sModelCommonPro.vCorners[5],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[0],sModelCommonPro.vCorners[5],sModelCommonPro.vCorners[4],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[3],sModelCommonPro.vCorners[7],sModelCommonPro.vCorners[6],vPos,vDir, u,v,d)
	//		||	CMath::IntersectTri(sModelCommonPro.vCorners[3],sModelCommonPro.vCorners[2],sModelCommonPro.vCorners[6],vPos,vDir, u,v,d) )
	//	{
	//		return true;
	//	}

	//	return false;
	//}

	void CMapEditObject::InitRotation()
	{
		//sModelCommonPro.matRotY.Identity();
		m_Quaternion.setIdentity();
		m_bNeedUpdate = true;
		sModelSavePro.vRotate.y = 0.0f;
		sModelCommonPro.fRotationCurrent = 0.0f;
	}

	void CMapEditObject::InitScale()
	{
		sModelSavePro.vScale = CVector3f(1.0f, 1.0f, 1.0f);
	}

	void CMapEditObject::UpdateRotation(int n)
	{
		//y
		if ( n == 0 )
		{
			sModelSavePro.vRotate.y -= ROTATION_STEP;
		}
		else if ( n==1 )
		{
			sModelSavePro.vRotate.y += ROTATION_STEP;
		}
		//z
		else if ( n==2 )
		{
			sModelSavePro.vRotate.z += ROTATION_STEP;
		}
		else if ( n==3 )
		{
			sModelSavePro.vRotate.z -= ROTATION_STEP;
		}
		//x
		else if ( n==4 )
		{
			sModelSavePro.vRotate.x += ROTATION_STEP;
		}
		else if ( n==5 )
		{
			sModelSavePro.vRotate.x -= ROTATION_STEP;
		}

		this->SetIsChanged(true);
	}

	void CMapEditObject::UpdateScale(int n, const EEdit_Scale_Type eScaleType)
	{
		//所有轴进行物体放大
		if ( n == 0 )
		{
			switch (eScaleType)
			{
			case EES_SCALEXYZ:
				{
					sModelSavePro.vScale.x += SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( sModelSavePro.vScale.x > 6.0f )
					{
						sModelSavePro.vScale.x = sModelSavePro.vScale.y;
						return;
					}

					sModelSavePro.vScale.y += SCALE_STEP;
					sModelSavePro.vScale.z += SCALE_STEP;
				}
				break;

			case EES_SCALEX:
				{
					float temp = sModelSavePro.vScale.x + SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp > 6.0f )
						return;

					sModelSavePro.vScale.x = temp;
				}
				break;

			case EES_SCALEY:
				{
					float temp = sModelSavePro.vScale.y + SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp > 6.0f )
						return;

					sModelSavePro.vScale.y = temp;
				}
				break;

			case EES_SCALEZ:
				{
					float temp = sModelSavePro.vScale.z + SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp > 6.0f )
						return;

					sModelSavePro.vScale.z = temp;
				}
				break;

			default:
				break;
			}
		}
		//所有轴进行物体
		else if ( n==1 )
		{
			switch (eScaleType)
			{
			case EES_SCALEXYZ:
				{
					sModelSavePro.vScale.x -= SCALE_STEP;
					if ( sModelSavePro.vScale.x < 0.0f )
					{
						sModelSavePro.vScale.x = sModelSavePro.vScale.y;
						return;
					}

					sModelSavePro.vScale.y -= SCALE_STEP;
					sModelSavePro.vScale.z -= SCALE_STEP;
				}
				break;

			case EES_SCALEX:
				{
					float temp = sModelSavePro.vScale.x - SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp < 0.0f )
						return;

					sModelSavePro.vScale.x = temp;
				}
				break;

			case EES_SCALEY:
				{
					float temp = sModelSavePro.vScale.y - SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp < 0.0f )
						return;

					sModelSavePro.vScale.x = temp;
				}
				break;

			case EES_SCALEZ:
				{
					float temp = sModelSavePro.vScale.z - SCALE_STEP;
					//如果超过一个扩大范围，就停止扩大
					if ( temp < 0.0f )
						return;

					sModelSavePro.vScale.z = temp;
				}
				break;

			default:
				break;
			}
		}
		//还原物体大小
		else if ( n==2 )
		{
			switch (eScaleType)
			{
			case EES_SCALEXYZ:
				{
					sModelSavePro.vScale.x = 1.0f;
					sModelSavePro.vScale.y = 1.0f;
					sModelSavePro.vScale.z = 1.0f;
				}
				break;

			case EES_SCALEX:
				sModelSavePro.vScale.x = 1.0f;
				break;

			case EES_SCALEY:
				sModelSavePro.vScale.y = 1.0f;
				break;

			case EES_SCALEZ:
				sModelSavePro.vScale.z = 1.0f;
				break;

			default:
				break;
			}
		}
		else if ( n==3 )
		{
			CEditModel::SetScale(sModelSavePro.vScale.x);
		}

		this->SetIsChanged(true);
	}

	void CMapEditObject::UpdateLiftEx(int nWay)
	{
		if ( nWay == 0 )
		{
			sModelSavePro.vUserOffsetEx.y -= fModelNormalLift;
			sModelCommonPro.vUserOffset.y -= fModelNormalLift;
		}
		if ( nWay == 1 )
		{
			sModelSavePro.vUserOffsetEx.y += fModelNormalLift;
			sModelCommonPro.vUserOffset.y += fModelNormalLift;
		}

		this->SetIsChanged(true);
	}

	void CMapEditObject::UpdateSuperLiftEx(int nWay)
	{
		if ( nWay == 0 )
		{
			sModelSavePro.vUserOffsetEx.y -= fModelQuickLift;
			sModelCommonPro.vUserOffset.y -= fModelQuickLift;
		}
		if ( nWay == 1 )
		{
			sModelSavePro.vUserOffsetEx.y += fModelQuickLift;
			sModelCommonPro.vUserOffset.y += fModelQuickLift;
		}

		this->SetIsChanged(true);
	}

	//////////////////////////////////////////////////////////////////////////

	float CMapEditObject::GetShiftXEx()
	{
		return sModelSavePro.vUserOffsetEx.x;
	}

	void CMapEditObject::SetShiftXEx(float x)
	{
		sModelSavePro.vUserOffsetEx.x = x;
		sModelCommonPro.vUserOffset.x = x;
	}

	void CMapEditObject::UpdateShiftXEx(int nWay)
	{
		if (  nWay == 1 )
		{
			sModelSavePro.vUserOffsetEx.x += 1.0f;
			sModelCommonPro.vUserOffset.x += 1.0f;
			this->SetIsChanged(true);
		}
		if ( nWay == 0 )
		{
			sModelSavePro.vUserOffsetEx.x -= 1.0f;
			sModelCommonPro.vUserOffset.x -= 1.0f;
			this->SetIsChanged(true);
		}
	}

	void CMapEditObject::SetShiftZEx(float z)
	{
		sModelSavePro.vUserOffsetEx.z = z;
		sModelCommonPro.vUserOffset.z = z;
	}

	float CMapEditObject::GetShiftZEx()
	{
		return sModelSavePro.vUserOffsetEx.z;
	}

	void CMapEditObject::UpdateShiftZEx(int nWay)
	{
		if ( nWay == 3 )
		{
			sModelSavePro.vUserOffsetEx.z += 1.0f;
			sModelCommonPro.vUserOffset.z += 1.0f;
			this->SetIsChanged(true);
		}

		if ( nWay == 2 )
		{
			sModelSavePro.vUserOffsetEx.z -= 1.0f;
			sModelCommonPro.vUserOffset.z -= 1.0f;
			this->SetIsChanged(true);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	float CMapEditObject::GetLiftEx()
	{
		return sModelSavePro.vUserOffsetEx.y;
	}

	//y
	float CMapEditObject::GetRotation()
	{
		return sModelSavePro.vRotate.y;
	}

	//z
	float CMapEditObject::GetPitch()
	{
		return sModelSavePro.vRotate.z;
	}

	//x
	float CMapEditObject::GetYaw()
	{
		return sModelSavePro.vRotate.x;
	}

	float CMapEditObject::GetScaleX()
	{
		return sModelSavePro.vScale.x;
	}

	float CMapEditObject::GetScaleY()
	{
		return sModelSavePro.vScale.y;
	}

	float CMapEditObject::GetScaleZ()
	{
		return sModelSavePro.vScale.z;
	}

	float CMapEditObject::GetAdjustedRotation()
	{
		return acos(sModelCommonPro.mat._11);
	}

	void CMapEditObject::RestoreLiftEx(float f)
	{
		sModelSavePro.vUserOffsetEx.y = f;
		sModelCommonPro.vUserOffset.y = f;
	}

	void CMapEditObject::RestoreRotation(float f, int n)
	{
		sModelSavePro.vRotate.y = f;
		sModelSavePro.nRotationCnt = n;
	}

	void CMapEditObject::RestoreRotation(float yf, int yn, float xf, int xn, float zf, int zn)
	{
		sModelSavePro.vRotate.y = yf;
		sModelSavePro.nRotationCnt = yn;
		sModelSavePro.vRotate.x = xf;
		sModelSavePro.nYawCnt = xn;
		sModelSavePro.vRotate.z = zf;
		sModelSavePro.nPitchCnt = zn;
	}

	void CMapEditObject::RestoreScale(float scalex, float scaley, float scalez)
	{
		sModelSavePro.vScale = CVector3f(scalex, scaley, scalez);
	}

	// for picking
//	void CMapEditObject::UpdateBoundingBox(bool bAudioUpdate)
//	{
//		CVector3f mMin, mMax;
//		CMatrix mat;
//		mat = sModelCommonPro.matPieceOff*sModelCommonPro.matUserOffset*sModelCommonPro.matWorldOff;
//
//		if( !bAudioUpdate )
//		{
//			sModelCommonPro.vCorners[0] = sModelCommonPro.m_vCorners[0] * mat;
//			sModelCommonPro.vCorners[1] = sModelCommonPro.m_vCorners[1] * mat;
//			sModelCommonPro.vCorners[2] = sModelCommonPro.m_vCorners[2] * mat;
//			sModelCommonPro.vCorners[3] = sModelCommonPro.m_vCorners[3] * mat;
//			sModelCommonPro.vCorners[4] = sModelCommonPro.m_vCorners[4] * mat;
//			sModelCommonPro.vCorners[5] = sModelCommonPro.m_vCorners[5] * mat;
//			sModelCommonPro.vCorners[6] = sModelCommonPro.m_vCorners[6] * mat;
//			sModelCommonPro.vCorners[7] = sModelCommonPro.m_vCorners[7] * mat;
//
//			mMin = sModelCommonPro.vCorners[0];
//			mMax = sModelCommonPro.vCorners[0];
//
//			for ( int32 i = 1; i < 8; ++i )
//			{
//				if ( sModelCommonPro.vCorners[i].x < mMin.x )
//				{
//					mMin.x = sModelCommonPro.vCorners[i].x;
//				}
//				else if ( sModelCommonPro.vCorners[i].x > mMax.x )
//				{
//					mMax.x = sModelCommonPro.vCorners[i].x;
//				}
//
//				if ( sModelCommonPro.vCorners[i].y < mMin.y )
//				{
//					mMin.y = sModelCommonPro.vCorners[i].y;
//				}
//				else if ( sModelCommonPro.vCorners[i].y > mMax.y )
//				{
//					mMax.y = sModelCommonPro.vCorners[i].y;
//				}
//
//				if ( sModelCommonPro.vCorners[i].z < mMin.z )
//				{
//					mMin.z = sModelCommonPro.vCorners[i].z;
//				}
//				else if ( sModelCommonPro.vCorners[i].z > mMax.z )
//				{
//					mMax.z = sModelCommonPro.vCorners[i].z;
//				}
//			}
//		}
//
//		if( m_strSceneCueName != "" || sModelCommonPro.bEffectBase )
//		{ 			
//			int nRange = CSceneAudioMgr::GetInst()->GetSceneCueBoxRange();
//			float fRange = GRID_SPACE * nRange;
//
// 			mMin = CVector3f(-30.0f, 0.0f,  -30.0f);
// 			mMax = CVector3f( 30.0f, 60.0f,  30.0f);
// 			mMin = mMin * mat ;
// 			mMax = mMax * mat ;
//
//			mMin -= CVector3f(fRange, fRange, fRange);
//			mMax += CVector3f(fRange, fRange, fRange);
//
//// 			if( m_strSceneCueName.find("blacksmith01") != -1/* || m_strSceneCueName.find("forest02") != -1*/ )
//// 			{
////   				DebugOut("%s, min %f, %f, %f\n", m_strSceneCueName.c_str(), sModelCommonPro.mMin.x, sModelCommonPro.mMin.y, sModelCommonPro.mMin.z);
////   				DebugOut("max %f, %f, %f\n", sModelCommonPro.mMax.x, sModelCommonPro.mMax.y, sModelCommonPro.mMax.z);
//// 			}
//		}
//		
//		m_OrgAABB.setExtents(mMin, mMax);
//	}

	void CMapEditObject::UpdateRotationDiscrete()
	{
		sModelSavePro.vRotate.y += CMath::ToRadian(90);
		++sModelSavePro.nRotationCnt;
		
		CMatrix matRotY;
		matRotY.SetRotateY(CMath::ToRadian(90));

		// update block center
		if ( this->sModelCommonPro.bSubUnit )
		{
			CVector3f vOldCenter = sModelCommonPro.vBlockCenter + CVector3f(GRID_SPACE,0,GRID_SPACE);

			switch ( sModelCommonPro.eCenterType )
			{
				case ECT_CENTER:
					break;

				case ECT_CORNER:
					break;

				case ECT_XMIDWAY:
					sModelCommonPro.eCenterType = ECT_ZMIDWAY;
					sModelCommonPro.vBlockCenter.z += GRID_SPACE/2.0f;
					sModelCommonPro.vBlockCenter.x -= GRID_SPACE/2.0f;
					break;

				case ECT_ZMIDWAY:
					sModelCommonPro.eCenterType = ECT_XMIDWAY;
					sModelCommonPro.vBlockCenter.x += GRID_SPACE/2.0f;
					sModelCommonPro.vBlockCenter.z -= GRID_SPACE/2.0f;
					break;
			}
		}

		// update image center
		int nImageCenterGridIndex = this->GetUserOffsetAdjustedGridIndex(this->GetCenterGridIndex());
		SGrid &gridImageCenter = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nImageCenterGridIndex);
		sModelCommonPro.vImageCenter = gridImageCenter.vCenter;

		this->SetIsChanged(true);
	}

	int CMapEditObject::GetCenterGridIndex()
	{
		return sModelSavePro.nCenterGridIndex;
	}

	CVector3f CMapEditObject::GetGridCenter() const
	{
		SGrid &grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(sModelSavePro.nCenterGridIndex);
		return grid.vCenter;
	}

	void CMapEditObject::SetOffsetOverall(CVector3f vec)
	{
		sModelCommonPro.vOffsetOverall = vec;
		this->CalOffsetOverall();
	}

	void CMapEditObject::CalOffsetOverall()
	{
		CVector3f vModelScale  = CEditModel::GetBoxScale();
		CVector3f vModelOffset = CEditModel::GetBoxOff();

		sModelCommonPro.matSelfOff.SetTranslate(-sModelCommonPro.vOffsetOverall.x, -sModelCommonPro.vOffsetOverall.y, -sModelCommonPro.vOffsetOverall.z);

		CVector3f v = vModelOffset - sModelCommonPro.vOffsetOverall;
		sModelCommonPro.matPieceOff.SetTranslate(v.x, vModelOffset.y - vModelScale.y*0.5f - sModelCommonPro.vOffsetOverall.y, v.z);

// 		DebugOut("%s, %f, %f, %f, vec = %f, %f, %f\n", sModelCommonPro.strFileName.c_str(), sModelCommonPro.matSelfOff._41, sModelCommonPro.matSelfOff._42, sModelCommonPro.matSelfOff._43, sModelCommonPro.vOffsetOverall.x, sModelCommonPro.vOffsetOverall.y, sModelCommonPro.vOffsetOverall.z);
// 		DebugOut("scale %f, %f, %f, offsert %f, %f, %f\n", vModelScale.x, vModelScale.y, vModelScale.z, 
// 			vModelOffset.x, vModelOffset.y, vModelOffset.z);

		/*
		ZMirror matrix is
		-1 0 0 0 
		-0 1 0 0 
		-0 0 1 0
		0 0 0 1
		*/
		sModelCommonPro.matSelfOffZMirror = sModelCommonPro.matSelfOff;
		sModelCommonPro.matSelfOffZMirror._11 *= -1.0f;
		sModelCommonPro.matSelfOffZMirror._21 *= -1.0f;
		sModelCommonPro.matSelfOffZMirror._31 *= -1.0f;
		sModelCommonPro.matSelfOffZMirror._41 *= -1.0f;

		/*
		XMirror matrix is
		1 0 -0 0 
		0 1 -0 0 
		0 0 -1 0
		0 0  0 1
		*/
		sModelCommonPro.matSelfOffXMirror = sModelCommonPro.matSelfOff;
		sModelCommonPro.matSelfOffXMirror._13 *= -1.0f;
		sModelCommonPro.matSelfOffXMirror._23 *= -1.0f;
		sModelCommonPro.matSelfOffXMirror._33 *= -1.0f;
		sModelCommonPro.matSelfOffXMirror._43 *= -1.0f;

		// wirebox box
		sModelCommonPro.fHeight = vModelScale.y * sModelSavePro.vScale.y;
		sModelCommonPro.fWidth  = vModelScale.x * sModelSavePro.vScale.x;
		sModelCommonPro.fDepth  = vModelScale.z * sModelSavePro.vScale.z;

		if( m_pWireBox )
			m_pWireBox->Create(sModelCommonPro.fHeight, sModelCommonPro.fWidth, sModelCommonPro.fDepth, 0xff0000ff);

		sModelCommonPro.m_vCorners[0] = CVector3f(-sModelCommonPro.fWidth/2, 0.0f, -sModelCommonPro.fDepth/2);
		sModelCommonPro.m_vCorners[1] = sModelCommonPro.m_vCorners[0] + CVector3f(0.0f,0.0f,sModelCommonPro.fDepth);
		sModelCommonPro.m_vCorners[2] = sModelCommonPro.m_vCorners[1] + CVector3f(sModelCommonPro.fWidth,0.0f,0.0f);
		sModelCommonPro.m_vCorners[3] = sModelCommonPro.m_vCorners[0] + CVector3f(sModelCommonPro.fWidth,0.0f,0.0f);
		sModelCommonPro.m_vCorners[4] = sModelCommonPro.m_vCorners[0] + CVector3f(0.0f,sModelCommonPro.fHeight,0.0f);
		sModelCommonPro.m_vCorners[5] = sModelCommonPro.m_vCorners[1] + CVector3f(0.0f,sModelCommonPro.fHeight,0.0f);
		sModelCommonPro.m_vCorners[6] = sModelCommonPro.m_vCorners[2] + CVector3f(0.0f,sModelCommonPro.fHeight,0.0f);
		sModelCommonPro.m_vCorners[7] = sModelCommonPro.m_vCorners[3] + CVector3f(0.0f,sModelCommonPro.fHeight,0.0f);
	}

	void CMapEditObject::SetCenterGridIndex(int n)
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if ( n != INVALID )
		{
			SGrid & grid = pTerrain->GetGrid(n);
			sModelCommonPro.vBlockCenter = grid.vCenter;

			int		nImageCenterGridIndex	= this->GetUserOffsetAdjustedGridIndex(n);
			SGrid   &gridImageCenter		= pTerrain->GetGrid(nImageCenterGridIndex);
			sModelCommonPro.vImageCenter    = gridImageCenter.vCenter;

			if ( this->IsSubUnit() )
			{
				switch ( sModelCommonPro.eCenterType )
				{
					case ECT_CENTER:
						break;

					case ECT_CORNER:
						sModelCommonPro.vBlockCenter.x -= GRID_SPACE/2.0f;
						sModelCommonPro.vBlockCenter.z -= GRID_SPACE/2.0f;
						break;

					case ECT_XMIDWAY:
						sModelCommonPro.vBlockCenter.z -= GRID_SPACE/2.0f;
						break;

					case ECT_ZMIDWAY:
						sModelCommonPro.vBlockCenter.x -= GRID_SPACE/2.0f;
						break;
				}
			}

			sModelSavePro.nCenterGridIndex = n;
			sModelCommonPro.nCenterGridIndexCurrent = n;
		}
		else
		{
			sModelCommonPro.vBlockCenter = CVector3f(0.0f,0.0f,0.0f);
			sModelCommonPro.matWorldOff.Identity();
			sModelSavePro.nCenterGridIndex = INVALID;
			sModelCommonPro.nCenterGridIndexCurrent = INVALID;
		}
	}

	int CMapEditObject::GetOriginAdjustedCenterGridIndex(int n)
	{
		SGrid & grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(n);
		int nDeltaX = 0;
		int nDeltaZ = 0;

		CVector3f vModelOffset = CEditModel::GetBoxOff();
		float Offsetx = vModelOffset.x - sModelCommonPro.vOffsetOverall.x;
		float Offsetz = vModelOffset.z - sModelCommonPro.vOffsetOverall.z;

		switch ( sModelCommonPro.eCenterType )
		{
			case ECT_CENTER:
			{
				if ( Offsetx != 0.0f )
					nDeltaX = (int)( Offsetx / GRID_SPACE + 0.5f * Offsetx /abs(Offsetx) );

				if ( Offsetz != 0.0f )
					nDeltaZ = (int)( Offsetz / GRID_SPACE + 0.5f * Offsetz /abs(Offsetz) );

				if ( Offsetx == 0.0f && Offsetz != 0.0f)
					nDeltaX = nDeltaZ;
				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
			}
			break;

			case ECT_CORNER:
			{
				if ( Offsetx != 0.0f )
					nDeltaX = (int)( Offsetx / GRID_SPACE + 1.0f * Offsetx /abs(Offsetx) );

				if ( nDeltaX > 0 )
					--nDeltaX;

				if ( Offsetz != 0.0f )
					nDeltaZ = (int)( Offsetz / GRID_SPACE + 1.0f * Offsetz /abs(Offsetz) );

				if ( nDeltaZ > 0 )
					--nDeltaZ;

				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
			}
			break;

			case ECT_XMIDWAY:
			{
				if ( Offsetx != 0.0f )
					nDeltaX = (int)( Offsetx / GRID_SPACE + 0.5f* Offsetx /abs(Offsetx) );

				if ( Offsetz != 0.0f )
					nDeltaZ = (int)( Offsetz / GRID_SPACE + 1.0f* Offsetz /abs(Offsetz) );

				if ( nDeltaZ > 0 )
					--nDeltaZ;

				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
			}
			break;

			case ECT_ZMIDWAY:
			{
				if ( Offsetx != 0.0f )
					nDeltaX = (int)( Offsetx / GRID_SPACE + 1.0f * Offsetx /abs(Offsetx) );

				if ( nDeltaX > 0 )
					--nDeltaX;

				if ( Offsetz != 0.0f )
					nDeltaZ = (int)( Offsetz / GRID_SPACE + 0.5f * Offsetz /abs(Offsetz) );

				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
				if ( Offsetx != 0.0f && Offsetz == 0.0f)
					nDeltaZ = nDeltaX;
			}
			break;
		}

		return n + nDeltaX +  nDeltaZ* CMapEditApp::GetInst()->GetEditingMesh()->GetWidth();
	}

	void CMapEditObject::SetAnimationByAgpFileName(string strAgpFileName)
	{
		if( strAgpFileName == "" )
			return;

		//CAnimationsRes * pAnimationGroup;
		CSkeletalFrame* pSke;
		CAnimationGroup* pAnimationGroup;
		string sAgpFile("");

		if( strAgpFileName.rfind(".agp") != -1 )
			sAgpFile = strAgpFileName;
		else
			sAgpFile = strAgpFileName.substr(0,strAgpFileName.size()-4)+".agp";

		SQR_TRY
		{
			if (CGraphic::GetInst()->CreateAnimationGroup(sAgpFile.c_str(),&pAnimationGroup)==INVALID||CSkeletalsMgr::GetInst()->GetSkeInPool(pAnimationGroup->GetSkeletalFrameName(),&pSke) == INVALID )
			{
				string log = strAgpFileName;
				log = log + "骨架或者动画列表文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);
			}
			else
			{
				SQR_TRY
				{
					CEditModel::SetSkeletalFrame(pSke);
					CEditModel::SetAnimateGroup(pAnimationGroup);
					string strAniName = CEditModel::GetAnimateGroup()->GetAnimationName(0);
					CEditModel::SetNextAnimation(strAniName.c_str(),FramePair(0,-1),true, 1);

					sModelCommonPro.strModelAgpName = sAgpFile;
					sModelCommonPro.strModelSkeName = pAnimationGroup->GetSkeletalFrameName();

					sModelSavePro.bPlayAnimate = true;
				}
				SQR_CATCH(exp)
				{
					CEditModel::DelFramework();
					CEditModel::ClearAnimateGroup();
					CEditModel::SetSkeletalFrame(NULL);
					CEditModel::SetAnimateGroup(NULL);
					MessageBox(NULL, exp.ErrorMsg(), "提示：该模型请重新绑定骨骼", MB_OK);
				}
				SQR_TRY_END;
				pAnimationGroup->Release();
			}
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
		}
		SQR_TRY_END;

	}

	void CMapEditObject::StopModelAnimation()
	{
// 		string strAniName = CEditModel::GetAnimateGroup()->GetAnimationName(0);
// 		CEditModel::SetNextAnimation(strAniName.c_str(),FramePair(0,0),false, 1);
// 
 		sModelSavePro.bPlayAnimate = false;

		CEditModel::DelFramework();
		CEditModel::ClearAnimateGroup();
		CEditModel::SetSkeletalFrame(NULL);
		CEditModel::SetAnimateGroup(NULL);
	}

	int CMapEditObject::GetUserOffsetAdjustedGridIndex(int nGridIndex)
	{
		SGrid &grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nGridIndex);

		CVector3f vBlockCenter = grid.vCenter;

		CVector3f v(sModelCommonPro.fShiftX , 0.0f, sModelCommonPro.fShiftZ);

		v = v + vBlockCenter;

		int nDeltaX = int(v.x / GRID_SPACE);
		int nDeltaZ = int(v.z / GRID_SPACE);

		return nDeltaZ * CMapEditApp::GetInst()->GetEditingMesh()->GetWidth() + nDeltaX;
	}

	void CMapEditObject::GetImageOffsetToAdjustedCenterGrid(float &x, float &y, float &z, float &fRotationX, float &fRotationY, float &fRotationZ, float &fScaleX, float &fScaleY, float &fScaleZ )
	{
		x = sModelCommonPro.mat._41;
		y = sModelCommonPro.mat._42;
		z = sModelCommonPro.mat._43;

		fScaleX = sModelSavePro.vScale.x;
		fScaleY = sModelSavePro.vScale.y;
		fScaleZ = sModelSavePro.vScale.z;

		fRotationX = sModelSavePro.vRotate.x;
		fRotationY = sModelSavePro.vRotate.y;
		fRotationZ = sModelSavePro.vRotate.z;
	}

	void CMapEditObject::GetImageOffsetToAdjustedCenterGrid(CVector3f& offset)
	{
		offset.x = sModelCommonPro.mat._41;
		offset.y = sModelCommonPro.mat._42;
		offset.z = sModelCommonPro.mat._43;
	}

	bool CMapEditObject::GetIsChanged()
	{
		return sModelCommonPro.bIsChanged;
	}

	void CMapEditObject::SetIsChanged(bool b)
	{
		sModelCommonPro.bIsChanged = b;
		bool bCalSelfOff = CEditStateOpertiaonMgr::GetInst()->GetIsCalObjectWorOffset();

		if ( EES_PLACED == sModelCommonPro.eEditState && b )
		{
			CMatrix matTemp;
			CMatrix matTemp1;
			CMatrix matChange;
			matTemp.SetRotateZ(sModelSavePro.vRotate.z);
			matTemp1.SetRotateX(sModelSavePro.vRotate.x);
			sModelCommonPro.matRotY.SetRotateY(sModelSavePro.vRotate.y);
			sModelCommonPro.matRotY = matTemp * sModelCommonPro.matRotY;
			sModelCommonPro.matRotY = matTemp1 * sModelCommonPro.matRotY;

			sModelCommonPro.matScale.SetScale(sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z);
			matChange = sModelCommonPro.matRotY * sModelCommonPro.matScale;

			m_Quaternion.setIdentity();

			RotateX(sModelSavePro.vRotate.x);
			RotateZ(sModelSavePro.vRotate.z);
			RotateY(sModelSavePro.vRotate.y);

			CEditModel::SetScale(sModelSavePro.vScale.x);
						
			sModelCommonPro.matUserOffset.SetTranslate(sModelCommonPro.fShiftX + sModelSavePro.vUserOffsetEx.x, sModelCommonPro.fLift + sModelSavePro.vUserOffsetEx.y, sModelCommonPro.fShiftZ+sModelSavePro.vUserOffsetEx.z);

			if ( sModelCommonPro.bFixedHeight )
			{
				sModelCommonPro.matWorldOff.SetTranslate(sModelCommonPro.vBlockCenter.x, 0.0f, sModelCommonPro.vBlockCenter.z);
			}
			else
			{
				sModelCommonPro.matWorldOff.SetTranslate(sModelCommonPro.vBlockCenter.x, sModelCommonPro.vImageCenter.y, sModelCommonPro.vBlockCenter.z);
			}

			if(  bCalSelfOff )
			{
				if ( sModelSavePro.nMirrorType == 0 ) // no mirror
				{
					sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*matChange*sModelCommonPro.matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 1 ) // x
				{
					sModelCommonPro.mat = sModelCommonPro.matSelfOffXMirror*sModelCommonPro.matUserOffset*matChange*sModelCommonPro.matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 2 ) // z
				{
					sModelCommonPro.mat = sModelCommonPro.matSelfOffZMirror*sModelCommonPro.matUserOffset*matChange*sModelCommonPro.matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 3 ) // u never know
				{
					CMatrix other = sModelCommonPro.matWorldOff;
					other._42 += 128.0f;
					sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*matChange*other;
				}
			}
			else
			{	
				CMatrix matWorldOff, matUserOffset;
				matUserOffset.SetTranslate(sModelCommonPro.vUserOffset.x, sModelCommonPro.vUserOffset.y, sModelCommonPro.vUserOffset.z);

				matWorldOff.SetTranslate(sModelSavePro.vPosition.x, sModelSavePro.vPosition.y, sModelSavePro.vPosition.z);
				
				if ( sModelSavePro.nMirrorType == 0 ) // no mirror
				{
					sModelCommonPro.mat = matUserOffset*matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 1 ) // x
				{
					sModelCommonPro.mat = MirrorXMatrix*matUserOffset*matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 2 ) // z
				{
					sModelCommonPro.mat = MirrorZMatrix*matUserOffset*matWorldOff;
				}
				else if ( sModelSavePro.nMirrorType == 3 ) // u never know
				{
 					CMatrix other = sModelCommonPro.matWorldOff;
 					other._42 += 128.0f;
 					sModelCommonPro.mat = sModelCommonPro.matSelfOff*matUserOffset*other;
				}
				
				//DebugOut("grid %d, %f, %f, %f\n", sModelSavePro.nCenterGridIndex, sModelCommonPro.vUserOffset.x, sModelCommonPro.vUserOffset.y, sModelCommonPro.vUserOffset.z);
				
				sModelCommonPro.vUserOffset = CVector3f(0.0f, 0.0f, 0.0f);
			}

			this->sModelSavePro.vPosition.x = sModelCommonPro.mat._41;
			this->sModelSavePro.vPosition.y = sModelCommonPro.mat._42;
			this->sModelSavePro.vPosition.z = sModelCommonPro.mat._43;

			SetPosition(sModelCommonPro.mat.GetLoc());

			sModelCommonPro.WireBoxMat = sModelCommonPro.matPieceOff*sModelCommonPro.matUserOffset*matChange*sModelCommonPro.matWorldOff;	
			this->UpdateBoundingBox();
			
			sModelCommonPro.bIsChanged = false;

			float fHeight = sModelCommonPro.mat._42 + this->sModelCommonPro.fHeight;
			if( fHeight < MixObjHeight )
				this->sModelSavePro.bTranslucent = false;
			else
				this->sModelSavePro.bTranslucent = true;
		}
	}

	// for picking
	void CMapEditObject::UpdateBoundingBox(bool bAudioUpdate)
	{
		CVector3f mMin, mMax;
		CMatrix mat;
		mat = sModelCommonPro.matPieceOff*sModelCommonPro.matUserOffset*sModelCommonPro.matRotY*sModelCommonPro.matScale*sModelCommonPro.matWorldOff;

		if( !bAudioUpdate )
		{
			sModelCommonPro.vCorners[0] = sModelCommonPro.m_vCorners[0] * mat;
			sModelCommonPro.vCorners[1] = sModelCommonPro.m_vCorners[1] * mat;
			sModelCommonPro.vCorners[2] = sModelCommonPro.m_vCorners[2] * mat;
			sModelCommonPro.vCorners[3] = sModelCommonPro.m_vCorners[3] * mat;
			sModelCommonPro.vCorners[4] = sModelCommonPro.m_vCorners[4] * mat;
			sModelCommonPro.vCorners[5] = sModelCommonPro.m_vCorners[5] * mat;
			sModelCommonPro.vCorners[6] = sModelCommonPro.m_vCorners[6] * mat;
			sModelCommonPro.vCorners[7] = sModelCommonPro.m_vCorners[7] * mat;

			mMin = sModelCommonPro.vCorners[0];
			mMax = sModelCommonPro.vCorners[0];

			for ( int32 i = 1; i < 8; ++i )
			{
				if ( sModelCommonPro.vCorners[i].x < mMin.x )
				{
					mMin.x = sModelCommonPro.vCorners[i].x;
				}
				else if ( sModelCommonPro.vCorners[i].x > mMax.x )
				{
					mMax.x = sModelCommonPro.vCorners[i].x;
				}

				if ( sModelCommonPro.vCorners[i].y < mMin.y )
				{
					mMin.y = sModelCommonPro.vCorners[i].y;
				}
				else if ( sModelCommonPro.vCorners[i].y > mMax.y )
				{
					mMax.y = sModelCommonPro.vCorners[i].y;
				}

				if ( sModelCommonPro.vCorners[i].z < mMin.z )
				{
					mMin.z = sModelCommonPro.vCorners[i].z;
				}
				else if ( sModelCommonPro.vCorners[i].z > mMax.z )
				{
					mMax.z = sModelCommonPro.vCorners[i].z;
				}
			}
		}

		if( m_strSceneCueName != "" || sModelCommonPro.bEffectBase )
		{ 			
			int nRange = CSceneAudioMgr::GetInst()->GetSceneCueBoxRange();
			float fRange = GRID_SPACE * nRange;

			mMin = CVector3f(-30.0f, 0.0f,  -30.0f);
			mMax = CVector3f( 30.0f, 60.0f,  30.0f);
			mMin = mMin * mat ;
			mMax = mMax * mat ;

			mMin -= CVector3f(fRange, fRange, fRange);
			mMax += CVector3f(fRange, fRange, fRange);
		}

		//m_AABB.setExtents(mMin, mMax);
	}

	void CMapEditObject::SetLocked(bool b)
	{
		if ( m_pWireBox )
		{
			if (b)
			{
				m_pWireBox->SetColor(0xffff0000);
			}
			else
			{
				m_pWireBox->SetColor(0xff0000ff);
			}
		}
			
		sModelCommonPro.bLocked = b;
	
// 		if(b)
// 			SetForceRenderStyle(&CEditorCore::TR_EMRS_HOLD);
// 		else
// 			SetForceRenderStyle(NULL);
	}

	bool CMapEditObject::GetLocked()
	{
		return sModelCommonPro.bLocked;
	}

	sqr::EEdit_State_Placed CMapEditObject::GetPlacedEditState()
	{
		return sModelCommonPro.eEditStatePlaced;
	}

	void CMapEditObject::SetPlacedEditState(sqr::EEdit_State_Placed e)
	{
		sModelCommonPro.eEditStatePlaced = e;
	}

	void CMapEditObject::SetPlaceEditState()
	{
		sModelCommonPro.eEditState = sqr::EES_PLACED;
	}

	void CMapEditObject::SetEditState(sqr::EEdit_State state)
	{
		if (sModelCommonPro.eEditState==state)
			return;

		sqr::EEdit_State lastState = sModelCommonPro.eEditState;
		sModelCommonPro.eEditState = state;

		switch (state)
		{
			case EES_PLACED:
			{
				SetIsChanged(true);
				//CSceneManager::GetInst()->InsertObject(this);
				CMapEditApp::GetInst()->GetRenderScene()->AddToTreeScene(this);
				//if(!this->m_strSceneCueName.empty())
				
				//////////////////////////////////////////////////////////////////////////
				//这个地方比较龊，原因是将带特效delete后在ctrl+z还原后，因为之前调用了下onPreRender，将visible标志设成true，而特效里面没有处理
				//所以只能先强行设成false，再设成true
				if( m_pGroup && lastState == EES_DELETE )
					this->SetVisible(false);
				//////////////////////////////////////////////////////////////////////////

				this->SetVisible(true);

				break;
			}
			case EES_MOVING:
			{
				//if(m_pParentSpace)
				//	m_pParentSpace->DelChild(this);
				CMapEditApp::GetInst()->GetRenderScene()->AddToRenderScene(this);
				//CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->GetTreeNode(GetRegionID(this->GetPosition()))->DelChild(this);
				break;
			}
			case EES_DANGLING:
				SetVisible(false);
				if(m_pParentSpace)
					m_pParentSpace->DelChild(this);
				break;
			case EES_DELETE:
			{
				SetVisible(false);
				if(m_pParentSpace)
					m_pParentSpace->DelChild(this);
				//CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->GetTreeNode(GetRegionID(this->GetPosition()))->DelChild(this);
				break;
			}
			default:
				break;
		}
	}

	CVector3f CMapEditObject::GetUserOffset()
	{
		return CVector3f(sModelCommonPro.fShiftX, sModelCommonPro.fLift, sModelCommonPro.fShiftZ);
	}

	void CMapEditObject::SetUserOffset(CVector3f vec)
	{
		sModelCommonPro.fShiftX = vec.x;
		sModelCommonPro.fLift   = vec.y;
		sModelCommonPro.fShiftZ = vec.z;
	}

	void CMapEditObject::SetUserOffsetEx(CVector3f vec)
	{
		sModelSavePro.vUserOffsetEx = vec;
	}

	CVector3f CMapEditObject::GetUserOffsetEx()
	{
		return sModelSavePro.vUserOffsetEx;
	}

	void CMapEditObject::SetMirrorType(int n)
	{
		sModelSavePro.nMirrorType = n;
		this->SetIsChanged(true);
	}

	int CMapEditObject::GetMirrorType() const
	{
		return sModelSavePro.nMirrorType;
	}

	int CMapEditObject::GetRotationCount()
	{
		return sModelSavePro.nRotationCnt;
	}

	int CMapEditObject::GetPitchCount()
	{
		return sModelSavePro.nPitchCnt;
	}

	int CMapEditObject::GetYawCount()
	{
		return sModelSavePro.nYawCnt;
	}

	CVector3f CMapEditObject::GetGroupOffset()
	{
		return sModelCommonPro.vGroupOffset;
	}

	//多个物体选中后按c挪到后物体间会有偏差，原因是各个物体又相对拾取中心向二边移动
	void CMapEditObject::SetGroupOffset(CVector3f v)
	{
		sModelCommonPro.vGroupOffset = v;
		if (v.x == 0.0f)
			sModelCommonPro.nXGroupOffset = 0;
		else
			sModelCommonPro.nXGroupOffset = (int)((v.x/GRID_SPACE) + 0.5f/**abs(v.x)/v.x*/);

		if (v.z == 0.0f)
			sModelCommonPro.nZGroupOffset = 0;
		else
			sModelCommonPro.nZGroupOffset = (int)((v.z/GRID_SPACE) + 0.5f/**abs(v.z)/v.z*/);
	}

	int CMapEditObject::GetAdjustedGridIndexBlockCenter(int nGridIndex)
	{
		return nGridIndex + sModelCommonPro.nXGroupOffset + sModelCommonPro.nZGroupOffset * CMapEditApp::GetInst()->GetEditingMesh()->GetWidth();
	}

	void CMapEditObject::SetAnimationStart(int n, float fPlus)
	{
		if ( CEditModel::GetAnimateGroup() )
		{
			CEditModel::SetNextAnimation("stand01",FramePair(0,-1),true,n);
		}
		else
		{
			CEditModel::SetNextAnimation("Default",FramePair(0,-1),true,n);
		}
	}

	void CMapEditObject::SetIsTranslucent(bool b)
	{
		sModelSavePro.bTranslucent = b;
	}

	bool CMapEditObject::GetIsTranslucent()
	{
		return sModelSavePro.bTranslucent;
	}

	void CMapEditObject::SetIsUseCameraCollison(bool b)
	{
		sModelSavePro.bUseCameraCollision = b;
	}

	bool CMapEditObject::GetIsUseCameraCollison()
	{
		return sModelSavePro.bUseCameraCollision;
	}

	void CMapEditObject::SetResGroupMark(BYTE bResGroupMark)
	{
		sModelCommonPro.bResGroupMark = bResGroupMark;
	}

	BYTE CMapEditObject::GetResGroupMark()
	{
		return sModelCommonPro.bResGroupMark;
	}

	int CMapEditObject::GetAniFrameCount()
	{
		if ( CEditModel::GetAnimateGroup() )
		{
			return CEditModel::GetAnimateGroup()->GetAnimationMaxFrame(0);
		}

		return 1;
	}

	void CMapEditObject::LinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
	{
		m_pGroup = pGroup;
		CEditModel::AddEffectPtr( pEffect, CEffectPlayer::PLAY_LOOP );
	}

	void CMapEditObject::ReLinkEffect()
	{

	}

	void CMapEditObject::DeleteLinkEffect()
	{
		CEditModel::ClearAllEfx();
		sModelCommonPro.bEffectBase = false;
	}

	void CMapEditObject::SetIsCameraOriented(bool b)
	{
		sModelCommonPro.bCameraOriented = b;
	}

	bool CMapEditObject::GetIsCameraOriented()
	{
		return sModelCommonPro.bCameraOriented;
	}

	void CMapEditObject::SetIsEffectBased(bool b)
	{
		sModelCommonPro.bEffectBase = b;
	}

	bool CMapEditObject::GetIsEffectBased()
	{
		return sModelCommonPro.bEffectBase;
	}

	void CMapEditObject::HideSelf()
	{
		this->m_pTempWireBox = this->m_pWireBox;
		this->m_pWireBox = NULL;

		CEditModel::ShowPiece(NULL,false);
	}

	void CMapEditObject::ShowSelf()
	{
		if ( this->m_pTempWireBox )
		{
			this->m_pWireBox = this->m_pTempWireBox;
			this->m_pTempWireBox = NULL;
		}

		CEditModel::ShowPiece(NULL,true);
	}

	bool CMapEditObject::GetEffectName(string & strEffectName)
	{
		IEffect* pEfx = GetEffect(0);
		if(pEfx==NULL)
			return false;
		strEffectName = pEfx->GetFullEffectName();

		return true;
	}

	float CMapEditObject::GetHeight()
	{
		return sModelCommonPro.fHeight;
	}

	void CMapEditObject::SetModelTransState( const byte transState )
	{
		sModelSavePro.bTransState = transState;
	}

	byte CMapEditObject::GetModelTransState() const
	{
		return sModelSavePro.bTransState;
	}

	bool CMapEditObject::GetEffectFileName(string & strEffectFileName)
	{
		if ( m_pGroup == NULL )
		{
			strEffectFileName = "";
			return false;
		}

		strEffectFileName = m_pGroup->GetName();

		return true;
	}

	IEffectGroup * CMapEditObject::GetEffectGroup()
	{
		return m_pGroup;
	}

	void CMapEditObject::GetFinalMatrix(CMatrix & mat)
	{
		mat = sModelCommonPro.mat;
	}

	CMatrix CMapEditObject::GetMatrixWorld()
	{
		return sModelCommonPro.mat;
	}

	void CMapEditObject::SetWorldPosition(const CVector3f pos)
	{
		sModelSavePro.vPosition = pos;
	}

	int CMapEditObject::GetCenterGridIndexCurrent()
	{
		sModelCommonPro.fRotationCurrent += CMath::ToRadian(90);
		CMatrix matRotY;
		if (sModelCommonPro.bNotGroup)
		{
			matRotY.SetRotateY(sModelCommonPro.fRotationCurrent);
		}
		else
		{
			CVector3f vecOffset;
			vecOffset = sModelCommonPro.vGroupCenter - sModelCommonPro.vBlockCenter;
			CMatrix matOffset;
			CMatrix matOffsetBack;

			matOffset.SetTranslate(vecOffset.x, 0.0f, vecOffset.z);
			matOffsetBack.SetTranslate(-vecOffset.x, 0.0f, -vecOffset.z);

			matRotY.SetRotateY(sModelCommonPro.fRotationCurrent);

			matRotY = matOffsetBack * matRotY * matOffset;

		}

		CVector3f ObjectPos = CVector3f(0.0, 0.0, 0.0);
		CMatrix mat;
		mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*matRotY*sModelCommonPro.matWorldOff;
		ObjectPos *= mat;
		int x, z;
		x = int(ObjectPos.x/GRID_SPACE);
		z = int(ObjectPos.z/GRID_SPACE);

		sModelCommonPro.nCenterGridIndexCurrent = z * CMapEditApp::GetInst()->GetEditingMesh()->GetWidth() + x;

		sModelCommonPro.fRotationCurrent -= CMath::ToRadian(90);
		return sModelCommonPro.nCenterGridIndexCurrent;
	}

	CVector3f CMapEditObject::GetBlockCenter()
	{
		return sModelCommonPro.vBlockCenter;
	}

	void CMapEditObject::SetBlockCenter(CVector3f center)
	{
		sModelCommonPro.vBlockCenter = center;
	}

	void CMapEditObject::SetPreCenterGridIndex(int i)
	{
		sModelCommonPro.nPreCenterGridIndex = i;
	}

	float CMapEditObject::GetShiftX()
	{
		return sModelCommonPro.fShiftX;
	}

	float CMapEditObject::GetShiftZ()
	{
		return sModelCommonPro.fShiftZ;
	}

	float CMapEditObject::GetLiftx()
	{
		return sModelCommonPro.fLift;
	}

	int CMapEditObject::GetPreCenterGridIndex()
	{
		return sModelCommonPro.nPreCenterGridIndex;
	}

	void CMapEditObject::SetCastShadow( bool b )
	{
		sModelSavePro.bShadow = b;
	}

	bool CMapEditObject::IsCastShadow()
	{
		return sModelSavePro.bShadow;
	}

	void CMapEditObject::SetGroupCenter( CVector3f vGC )
	{
		sModelCommonPro.vGroupCenter = vGC;
	}

	CVector3f CMapEditObject::GetGroupCenter() const
	{
		return sModelCommonPro.vGroupCenter;
	}

	void CMapEditObject::SetVisible(bool b)
	{
		CEditModel::SetVisible(b);

		if( m_strSceneCueName != "" )
			this->StopSceneCue(!CEditModel::IsVisible());
	}

	void CMapEditObject::SetNotGroup( BOOL bNotGroup )
	{
		sModelCommonPro.bNotGroup = bNotGroup;
	}

	CVector3f CMapEditObject::GetModelOffset()
	{
		CVector3f offset = CEditModel::GetBoxOff();
		return offset;
	}

	CVector3f CMapEditObject::GetModelOffsetOverall() const
	{
		return sModelCommonPro.vOffsetOverall;
	}

	CVector3f CMapEditObject::GetModelBlockCenter() const
	{
		return sModelCommonPro.vBlockCenter;
	}

	void CMapEditObject::SetReceiveShadow( const bool bReceiveShadow )
	{
		sModelSavePro.bReceiveShadow = bReceiveShadow;
	}

	bool CMapEditObject::IsReceiveShadow() const
	{
		return sModelSavePro.bReceiveShadow;
	}

	void CMapEditObject::SetIsPlayAnimation( const bool b )
	{
		sModelSavePro.bPlayAnimate = b;
	}

	bool CMapEditObject::GetIsPlayAnimation() const
	{
		return sModelSavePro.bPlayAnimate;
	}

	string CMapEditObject::GetAnimationGroupName() const
	{
		return sModelCommonPro.strModelAgpName;
	}

	string CMapEditObject::GetAnimationSkeName() const
	{
		return sModelCommonPro.strModelSkeName;
	}

	bool CMapEditObject::IsSubUnit()
	{
		return sModelCommonPro.bSubUnit;
	}

	bool CMapEditObject::GetHideModelVertexColor()
	{
		return sModelCommonPro.bHideModelVertexColor;
	}

	bool CMapEditObject::GetSelected()
	{
		return sModelCommonPro.bSelected;
	}

	void CMapEditObject::SetSelected( bool b )
	{
		sModelCommonPro.bSelected = b;
		if(sModelCommonPro.bLocked)
			return;
		if(b)
			SetForceRenderStyle(&CEditorCore::TR_EMRS_SELECT);
		else
			SetForceRenderStyle(NULL);
	}

	bool CMapEditObject::IsActive()
	{
		return sModelCommonPro.eEditState == EES_MOVING;
	}

	//物体max缩放信息+地编编辑缩放信息
	sqr::CVector3f CMapEditObject::GetEditModelScale()
	{
		CVector3f vModelScale = CEditModel::GetBoxScale();
		sModelCommonPro.fHeight = vModelScale.y * sModelSavePro.vScale.y;
		sModelCommonPro.fWidth  = vModelScale.x * sModelSavePro.vScale.x;
		sModelCommonPro.fDepth  = vModelScale.z * sModelSavePro.vScale.z;

		return CVector3f(sModelCommonPro.fWidth,sModelCommonPro.fHeight,sModelCommonPro.fDepth);
	}

	string & CMapEditObject::GetName()
	{
		return sModelCommonPro.strFileName;
	}

	int CMapEditObject::GetDepthInGrids()
	{
		return sModelCommonPro.nDepthDimInGrids;
	}

	int CMapEditObject::GetWidthInGrids()
	{
		return sModelCommonPro.nWidthDimInGrids;
	}

	void CMapEditObject::SetScaleZ( float z )
	{
		sModelSavePro.vScale.z = z;
	}

	void CMapEditObject::SetScaleY( float y )
	{
		sModelSavePro.vScale.y = y;
	}

	void CMapEditObject::SetScaleX( float x )
	{
		sModelSavePro.vScale.x = x;
	}

	//z
	void CMapEditObject::SetPitch( float fR )
	{
		sModelSavePro.vRotate.z = fR;
	}

	//x
	void CMapEditObject::SetYaw( float fR )
	{
		sModelSavePro.vRotate.x = fR;
	}

	//y
	void CMapEditObject::SetRotation( float fR )
	{
		sModelSavePro.vRotate.y = fR;
	}

	vector<string> CMapEditObject::GetAniNameList() const
	{
		vector<string> vecEmpty;
		return vecEmpty;
	}

	void CMapEditObject::SetIsVisibleByConfigure( const bool b )
	{
		sModelSavePro.bVisibleByConfigure = b;
	}

	bool CMapEditObject::GetIsVisibleByConfigure() const
	{
		return sModelSavePro.bVisibleByConfigure;
	}

	void CMapEditObject::SetIsLightEffect( const bool b )
	{
		sModelCommonPro.bEffectLight = b;
	}

	bool CMapEditObject::GetIsLightEffect() const
	{
		return sModelCommonPro.bEffectLight;
	}

	void CMapEditObject::SetBakingColorIsNotGreaterShadow( const bool b )
	{
		sModelSavePro.bBakingColorMark = b;
	}

	bool CMapEditObject::GetBakingColorIsNotGreaterShadow()
	{
		return sModelSavePro.bBakingColorMark;
	}
}
