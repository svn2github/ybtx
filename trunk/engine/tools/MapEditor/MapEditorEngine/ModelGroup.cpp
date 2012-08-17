#include "stdafx.h"
#include "ModelGroup.h"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "CTileEditorModel.h"
#include "IEffectGroup.h"
#include "IEffect.h"
#include "SceneAudio.h"
#include "CMapEditModelObjPro.h"
#include "TerrainMeshObject.h"
#include "Float.h"
#include "CEditDataScene.h"
#include "CTreeScene.h"

namespace sqr
{
	CEditModelGroup::CEditModelGroup()
	{
		m_nGridIndexImageCenter = INVALID;
		m_nAtten		= 0;
		m_nAffectRatio	= 100;
		m_bShadowWithZ	= false;
		m_bNext			= false;

		m_bFirstSee		= 0;
		m_bMoveDelete	= false;
		m_bHide			= false;
		m_bLoginModel	= false;
		m_pSceneAudio	= NULL;

		m_bReginEffect  = false;
		m_bModelInfoIsValid = false;
	}

	void CEditModelGroup::SwitchShadowSecondPass()
	{
		m_bShadowWithZ = !m_bShadowWithZ;
	}

	bool CEditModelGroup::GetShadowSecondPass()
	{
		return m_bShadowWithZ;
	}

	void CEditModelGroup::SetShadowSecondPass(bool b)
	{
		m_bShadowWithZ = b;
	}

	CEditModelGroup::~CEditModelGroup()
	{
		if( m_pSceneAudio )
		{
			delete m_pSceneAudio;
			m_pSceneAudio = NULL;
		}

		MapEditObjPool::iterator it,eit = m_vecSelfPresentation.end();
		for(it = m_vecSelfPresentation.begin(); it!=eit; ++it)
		{
			(*it)->RemoveFrmParent();
			//CMapEditApp::GetInst()->GetDataScene()->GetTreeScene()->GetTreeNode(GetRegionID((*it)->GetPosition()))->DelChild(*it);
		}
		//for_each(,,mem_fun(&CMapEditObject::Remove));
	}

	void CEditModelGroup::ReleaseSelfModels()
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CMapEditObject::Release));
	}

	void CEditModelGroup::AddContainer(vector<CMapEditObject*> &vec, bool bSubUnit, sqr::ECenter_Type eCenterType, int nXDelta, int nZDelta)
	{
		m_vecSelfPresentation = vec;
		m_bSubUnit		= bSubUnit;
		m_eCenterType	= eCenterType;

		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetCenterTypeOrigin),eCenterType) );
	}

	void CEditModelGroup::AddTitleContainer(vector<CTileEditorModel *> & vec, bool bSubUnit, sqr::ECenter_Type eBlockType, int nXDelta, int nZDelta)
	{
		m_vecTileSelfPresentation = vec;
		m_bSubUnit = bSubUnit;
	}

	void CEditModelGroup::SetCenterType(sqr::ECenter_Type e)
	{
		sqr::ECenter_Type te = static_cast<sqr::ECenter_Type>(e);
		for_each(m_vecTileSelfPresentation.begin(),m_vecTileSelfPresentation.end(),
				 bind2nd(mem_fun(&CTileEditorModel::SetCenterTypeOrigin),te) );
	}

	CMapEditObject * CEditModelGroup::GetSelfPresentation(int n)
	{
		if( n < (int)m_vecSelfPresentation.size() )
			return m_vecSelfPresentation[n];
		else
			return NULL;
	}

	int CEditModelGroup::GetSelfPresentationCount()
	{
		return m_vecSelfPresentation.size();
	}

	CTileEditorModel* CEditModelGroup::GetTitleSelfPresentation(int n)
	{
		return m_vecTileSelfPresentation[n];
	}

	int CEditModelGroup::GetTitleSelfPresentationCount()
	{
		return m_vecTileSelfPresentation.size();
	}

	void CEditModelGroup::SetModelFileNames(vector<string> & vec)
	{
		m_vecCMapEditModelFileNames = vec;
	}

	vector<string> & CEditModelGroup::GetModelFileNames()
	{
		return m_vecCMapEditModelFileNames;
	}

	size_t CEditModelGroup::GetModelFileNamesSize()
	{
		size_t asize = 0;
		vector<string>::iterator it = m_vecCMapEditModelFileNames.begin();
		vector<string>::iterator ite = m_vecCMapEditModelFileNames.end();

		for (;it!=ite;++it)
			asize += it->length();
		return asize;
	}

	string & CEditModelGroup::GetModelFileNameByIndex(int n)
	{
		return m_vecCMapEditModelFileNames[n];
	}

	void CEditModelGroup::SetEditState(EEdit_State e)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun(&CMapEditObject::SetEditState),e) );

		if( e == sqr::EES_DELETE )
			return;

		//if( /*e == sqr::EES_PLACED && */!m_bModelInfoIsValid )
		{
			CVector3f vecOffsetOverall(0.0f,0.0f,0.0f);

			float fXMin = FLT_MAX;
			float fXMax = -FLT_MAX;
			float fYMin = FLT_MAX;
			float fYMax = -FLT_MAX;
			float fZMin = FLT_MAX;
			float fZMax = -FLT_MAX;

// 			for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
// 			{
// 				CMapEditObject * p = (*iter);
// 				CVector3f offset = p->GetModelOffset();
// 				CVector3f scale  = p->GetEditModelScale();
// 
// 				float fXMinLocal = offset.x - scale.x * 0.5f;
// 				float fXMaxLocal = offset.x + scale.x * 0.5f;
// 
// 				float fZMinLocal = offset.z - scale.z * 0.5f;
// 				float fZMaxLocal = offset.z + scale.z * 0.5f;
// 
// 				float fYMinLocal = offset.y - scale.y * 0.5f;
// 				float fYMaxLocal = offset.y + scale.y * 0.5f;
// 
// 				if ( fXMinLocal < fXMin )
// 				{
// 					fXMin = fXMinLocal;
// 				}
// 
// 				if ( fXMaxLocal > fXMax )
// 				{
// 					fXMax = fXMaxLocal;
// 				}
// 
// 				if ( fZMinLocal < fZMin )
// 				{
// 					fZMin = fZMinLocal;
// 				}
// 
// 				if ( fZMaxLocal > fZMax )
// 				{
// 					fZMax = fZMaxLocal;
// 				}
// 
// 				if ( fYMinLocal < fYMin )
// 				{
// 					fYMin = fYMinLocal;
// 				}
// 
// 				if ( fYMaxLocal > fYMax )
// 				{
// 					fYMax = fYMaxLocal;
// 				}
// 			}
// 
// 			float fCenterX = ( fXMin + fXMax ) / 2.0f;
// 			float fCenterZ = ( fZMin + fZMax ) / 2.0f;
// 			float fToeY = fYMin;
// 
// 			vecOffsetOverall.x = fCenterX;
// 			vecOffsetOverall.z = fCenterZ;
// 			vecOffsetOverall.y = fToeY;


			for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				bind2nd(mem_fun(&CMapEditObject::SetOffsetOverall),vecOffsetOverall) );

			m_bModelInfoIsValid = true;
		}
	}

	void CEditModelGroup::SetPlacedEditState(sqr::EEdit_State_Placed e)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetPlacedEditState),e) );
	}

	void CEditModelGroup::SetLocked(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetLocked),b) );
	}

	void CEditModelGroup::SetCenterGridIndex(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetCenterGridIndex),n) );
	}

	void CEditModelGroup::UpdateLiftEx(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::UpdateLiftEx),n) );

		this->UpdateSpotLightDir();
	}

	void CEditModelGroup::UpdateSuperLiftEx(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::UpdateSuperLiftEx),n) );

		this->UpdateSpotLightDir();
	}

	void CEditModelGroup::UpdateSpotLightDir()
	{
		if ( !this->m_SpotLight.m_bIAmbSceneSpotLight)
			return;

		CVector3f targetpos = this->m_SpotLight.m_TargetPos;
		size_t modelSize = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < modelSize; ++i )
		{
			this->m_SpotLightPos = m_vecSelfPresentation[i]->GetGridCenter();

			///0 上下移
			this->m_SpotLightPos.y += m_vecSelfPresentation[i]->GetLiftEx();

			///1 左右移
			this->m_SpotLightPos.x += m_vecSelfPresentation[i]->GetShiftXEx();

			///2 前后移
			this->m_SpotLightPos.z += m_vecSelfPresentation[i]->GetShiftZEx();

			CVector3f dir = targetpos - this->m_SpotLightPos;
			dir.normalize();
			this->m_SpotLight.m_Direction = dir;
		}
	}

	void CEditModelGroup::UpdateSpotTargetPos(const int nType)
	{
		switch ( nType )
		{
			case 0:
			{
				this->m_SpotLight.m_TargetPos.y += 10.0f;
				break;
			}
			case 1:
			{
				this->m_SpotLight.m_TargetPos.y -= 10.0f;
				break;
			}
			case 2:
			{
				this->m_SpotLight.m_TargetPos.x -= 10.0f;
				break;
			}
			case 3:
			{
				this->m_SpotLight.m_TargetPos.x += 10.0f;
				break;
			}
			case 4:
			{
				this->m_SpotLight.m_TargetPos.z -= 10.0f;
				break;
			}
			case 5:
			{
				this->m_SpotLight.m_TargetPos.z += 10.0f;
				break;
			}
		}

		CVector3f dir = this->m_SpotLight.m_TargetPos - this->m_SpotLightPos;
		dir.normalize();
		this->m_SpotLight.m_Direction = dir;
	}

	void CEditModelGroup::UpdateOverallAmbientRatio()
	{
		float fRedOverallAmbientRatio = 0.0f;
		float fGreenOverallAmbientRatio = 0.0f;
		float fBlueOverallAmbientRatio = 0.0f;

		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			float fRed,fGreen,fBlue;
			(*iter)->RecaculateSelfAmbientRatio(fRed,fGreen,fBlue);
			fRedOverallAmbientRatio += fRed;
			fGreenOverallAmbientRatio += fGreen;
			fBlueOverallAmbientRatio += fBlue;
		}

		fRedOverallAmbientRatio /= m_vecSelfPresentation.size();
		fGreenOverallAmbientRatio /= m_vecSelfPresentation.size();
		fBlueOverallAmbientRatio /= m_vecSelfPresentation.size();

		float ARatio = m_nAffectRatio / 100.0f;

		//利用aratio顶点对模型的影响，如果aratio值越小，模型的表面材质mat的个颜色受顶点影响就越小
		fRedOverallAmbientRatio     = fRedOverallAmbientRatio * ARatio + (1 - ARatio);
		fGreenOverallAmbientRatio   = fGreenOverallAmbientRatio * ARatio + (1 - ARatio);
		fBlueOverallAmbientRatio    = fBlueOverallAmbientRatio * ARatio + (1 - ARatio);

		if ( fRedOverallAmbientRatio < 0.0f )
			fRedOverallAmbientRatio = 0.0f;
		if ( fGreenOverallAmbientRatio < 0.0f )
			fGreenOverallAmbientRatio = 0.0f;
		if ( fBlueOverallAmbientRatio < 0.0f )
			fBlueOverallAmbientRatio = 0.0f;

		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			(*iter)->UpdateOverallAmbientRatio(fRedOverallAmbientRatio,fGreenOverallAmbientRatio,fBlueOverallAmbientRatio);
		}
	}

	void CEditModelGroup::SetOverallAmbientRatio(float fRed, float fGreen, float fBlue)
	{
		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			(*iter)->UpdateOverallAmbientRatio(fRed,fGreen,fBlue);
		}
	}

	void CEditModelGroup::ResetOverallAmbientRatio()
	{
		float color = 128.0f / 255.0f;
		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			(*iter)->UpdateOverallAmbientRatio(color,color,color);
		}
		m_nAffectRatio = 100;
	}

	void CEditModelGroup::GetOverallAmbientRatio(byte & uR, byte & uG, byte & uB)
	{
		m_vecSelfPresentation[0]->GetOverallAmbientRatio(uR,uG,uB);
	}

	void CEditModelGroup::GetOverallAmbientRatio(float &fR, float &fG, float &fB)
	{
		m_vecSelfPresentation[0]->GetOverallAmbientRatio(fR,fG,fB);
	}

	void CEditModelGroup::UpdateShiftXEx(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::UpdateShiftXEx),n) );

		this->UpdateSpotLightDir();
	}

	void CEditModelGroup::SetShiftXEx(float x)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetShiftXEx), x) );
	}

	float CEditModelGroup::GetShiftXEx()
	{
		return m_vecSelfPresentation[0]->GetShiftXEx();
	}

	float CEditModelGroup::GetShiftZEx()
	{
		return m_vecSelfPresentation[0]->GetShiftZEx();

	}

	float CEditModelGroup::GetLiftEx()
	{
		return m_vecSelfPresentation[0]->GetLiftEx();
	}

	CVector3f CEditModelGroup::GetGroupOffset()
	{
		return m_vecSelfPresentation[0]->GetGroupOffset();
	}

	void CEditModelGroup::UpdateShiftZEx(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::UpdateShiftZEx),n) );

		this->UpdateSpotLightDir();
	}

	void CEditModelGroup::SetShiftZEx(float z)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetShiftZEx), z) );

	}
	void CEditModelGroup::UpdateRotation(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::UpdateRotation),n) );
	}

	void CEditModelGroup::UpdateScale(int n, const EEdit_Scale_Type eScaleType)
	{
		if( m_vecSelfPresentation.empty() )
			return;

		size_t size = m_vecSelfPresentation.size();
		for (size_t i = 0; i < size; ++i)
			m_vecSelfPresentation[i]->UpdateScale(n, eScaleType);
	}

	void CEditModelGroup::SetHideModelVertexColor(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetHideModelVertexColor),b) );
	}

	void CEditModelGroup::SetBlockCenter(CVector3f center)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetBlockCenter),center) );
	}

	CVector3f CEditModelGroup::GetBlockCenter()
	{
		return m_vecSelfPresentation[0]->GetBlockCenter();
	}

	void CEditModelGroup::SetPreCenterGridIndex(int i)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetPreCenterGridIndex),i) );
	}

	int CEditModelGroup::GetPreCenterGridIndex()
	{
		return m_vecSelfPresentation[0]->GetPreCenterGridIndex();
	}

	void CEditModelGroup::SetHide(bool b)
	{
		m_bHide = b;
	}

	void CEditModelGroup::SetGroupCenter(CVector3f m_vGC)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetGroupCenter),m_vGC) );
	}

	void CEditModelGroup::SetNotGroup(BOOL bNotGroup)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetNotGroup),bNotGroup) );
	}

	int CEditModelGroup::GetCenterGridIndexCurrent()
	{
		return m_vecSelfPresentation[0]->GetCenterGridIndexCurrent();
	}

	int CEditModelGroup::GetCenterGridIndex()
	{
		return m_vecSelfPresentation[0]->GetCenterGridIndex();
	}

	void CEditModelGroup::SetRotation(float fR)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetRotation),fR) );
	}

	void CEditModelGroup::SetYaw(float fR)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetYaw),fR) );
	}

	void CEditModelGroup::SetPitch(float fR)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetPitch),fR) );
	}

	void CEditModelGroup::SetScaleX(float x)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetScaleX),x) );
	}

	void CEditModelGroup::SetScaleY(float y)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetScaleY),y) );
	}

	void CEditModelGroup::SetScaleZ(float z)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetScaleZ),z) );
	}

	float CEditModelGroup::GetRotation()
	{
		return m_vecSelfPresentation[0]->GetRotation();
	}

	float CEditModelGroup::GetYaw()
	{
		return m_vecSelfPresentation[0]->GetYaw();
	}

	float CEditModelGroup::GetPitch()
	{
		return m_vecSelfPresentation[0]->GetPitch();
	}

	float CEditModelGroup::GetScaleX()
	{
		return m_vecSelfPresentation[0]->GetScaleX();
	}

	float CEditModelGroup::GetScaleY()
	{
		return m_vecSelfPresentation[0]->GetScaleY();
	}

	float CEditModelGroup::GetScaleZ()
	{
		return m_vecSelfPresentation[0]->GetScaleZ();
	}

	float CEditModelGroup::GetAdjustedRotation()
	{
		return m_vecSelfPresentation[0]->GetAdjustedRotation();
	}

	int CEditModelGroup::GetRotationCount()
	{
		return m_vecSelfPresentation[0]->GetRotationCount();
	}

	int CEditModelGroup::GetPitchCount()
	{
		return m_vecSelfPresentation[0]->GetPitchCount();
	}

	int CEditModelGroup::GetYawCount()
	{
		return m_vecSelfPresentation[0]->GetYawCount();
	}

	void CEditModelGroup::GetImageOffsetToAdjustedCenterGrid(float & x, float & y, float & z, float &fRotationX, float &fRotationY, float &fRotationZ, float& fScaleX, float& fScaleY, float& fScaleZ )
	{
		m_vecSelfPresentation[0]->GetImageOffsetToAdjustedCenterGrid(x, y, z, fRotationX, fRotationY, fRotationZ, fScaleX, fScaleY, fScaleZ);
	}

	void CEditModelGroup::SetUserOffsetEx(CVector3f vec)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun(&CMapEditObject::SetUserOffsetEx),vec) );
	}

	CVector3f CEditModelGroup::GetUserOffsetEx()
	{
		return m_vecSelfPresentation[0]->GetUserOffsetEx();
	}

	CVector3f CEditModelGroup::GetModelOffsetOverall()
	{
		return m_vecSelfPresentation[0]->GetModelOffsetOverall();
	}

	void CEditModelGroup::Render()
	{
		if ( m_vecSelfPresentation.empty() )
			return;

		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			if ( (*iter) )
			{
				(*iter)->Render();
			}
		}
	}

	void CEditModelGroup::SetGroupOffeset(CVector3f & v)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun<void,CMapEditObject,CVector3f>(&CMapEditObject::SetGroupOffset),v) );
	}

	void CEditModelGroup::SetSelected(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetSelected),b) );
	}

	bool CEditModelGroup::Intersected(CVector3f vDir,CVector3f vPos)
	{
		bool b = false;

		for ( vector<CMapEditObject*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			b = b||(*iter)->IntersectRay(vPos,vDir);
		}

		return b;
	}

	bool CEditModelGroup::GetIsSubUnit()
	{
		return m_bSubUnit;
	}

	sqr::ECenter_Type CEditModelGroup::GetCenterType()
	{
		return m_eCenterType;
	}

	void CEditModelGroup::SetAnimationByAgpFileName(string strAgpFileName)
	{
		m_strAgpFileName = strAgpFileName;

		if( this->m_bLoginModel )
			for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				bind2nd(mem_fun(&CMapEditObject::SetLoginAniByAgpFileName),strAgpFileName) );
		else
			for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetAnimationByAgpFileName),strAgpFileName) );
	}

	void CEditModelGroup::SetLoginAniByAgpFileName(string strAgpFileName)
	{	
		m_strAgpFileName = strAgpFileName;

		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun(&CMapEditObject::SetLoginAniByAgpFileName),strAgpFileName) );
	}

	bool CEditModelGroup::SetTitleAnimationByAgpFileName(string strAgpFileName)
	{
		if( m_vecTileSelfPresentation.empty() )
			return false;

		return m_vecTileSelfPresentation[0]->SetAnimationByAgpFileName(strAgpFileName);
	}

	void CEditModelGroup::SetMirrorType(int n)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetMirrorType),n) );
	}

	int CEditModelGroup::GetMirrorType() const
	{
		return m_vecSelfPresentation[0]->GetMirrorType();
	}

	string CEditModelGroup::GetAgpFileName()
	{
		return m_strAgpFileName;
	}

	void CEditModelGroup::SetIsChanged(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetIsChanged),b) );
	}

	CVector3f CEditModelGroup::GetUserOffset()
	{
		return m_vecSelfPresentation[0]->GetUserOffset();
	}

	string & CEditModelGroup::GetItemName()
	{
		return m_strItemName;
	}

	void CEditModelGroup::SetItemName(string & str)
	{
		m_strItemName = str;
	}

	void CEditModelGroup::AddGridIndexImageStandsOn(int n)
	{
		m_vecGridIndicesImageStandsOn.push_back(n);
	}

	vector<int> & CEditModelGroup::GetGridIndicesImageStandsOn()
	{
		return m_vecGridIndicesImageStandsOn;
	}

	void CEditModelGroup::ClearImages()
	{
		m_vecGridIndicesImageStandsOn.clear();
	}

	void CEditModelGroup::SetGridIndexImageCenter(int n)
	{
		m_nGridIndexImageCenter = n;
	}

	int CEditModelGroup::GetGridIndexImageCenter()
	{
		return m_nGridIndexImageCenter;
	}

	int CEditModelGroup::GetAdjustedGridIndexBlockCenter(int nGridIndex)
	{
		return m_vecSelfPresentation[0]->GetAdjustedGridIndexBlockCenter(nGridIndex);
	}

	BYTE CEditModelGroup::GetDir()
	{
		float fT = this->GetRotation()/ROTATION_STEP;

		int nT = (int)fT;

		return nT % 256;
	}

	void CEditModelGroup::SetIsTranslucent(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetIsTranslucent),b) );
	}

	void CEditModelGroup::SetIsUseCameraCollison(bool b)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun(&CMapEditObject::SetIsUseCameraCollison),b) );
	}

	bool CEditModelGroup::GetIsUseCameraCollison()
	{
		if( !m_vecSelfPresentation.empty() )
			return m_vecSelfPresentation[0]->GetIsUseCameraCollison();
		else
			return false;
	}

	int CEditModelGroup::GetAniFrameCount()
	{
		return m_vecSelfPresentation[0]->GetAniFrameCount();
	}

	void CEditModelGroup::LinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->LinkEffect(pEffect,pGroup);
	}

	void CEditModelGroup::ReLinkEffect()
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->ReLinkEffect();
	}

	void CEditModelGroup::DeleteLinkEffect()
	{
		if( m_vecSelfPresentation.empty() )
			return;
		
		m_vecSelfPresentation[0]->DeleteLinkEffect();
	}

	void CEditModelGroup::StopModelAnimation()
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->StopModelAnimation();
	}

	void CEditModelGroup::StopSceneCue( const bool bStop )
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->StopSceneCue(bStop,false);
	}

	bool CEditModelGroup::GetIsStopSound()
	{
		if( m_vecSelfPresentation.empty() )
			return false;
		else
			return m_vecSelfPresentation[0]->GetIsStopSound();
	}

	string CEditModelGroup::GetAnimationGroupName() const
	{
		static string nullname("");
		if( m_vecSelfPresentation.empty() )
			return nullname;

		return m_vecSelfPresentation[0]->GetAnimationGroupName();
	}

	string  CEditModelGroup::GetAnimationSkeName() const
	{
		static string nullname("");
		if( m_vecSelfPresentation.empty() )
			return nullname;

		return m_vecSelfPresentation[0]->GetAnimationSkeName();
	}

	void CEditModelGroup::SetIsPlayAnimation(const bool b)
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->SetIsPlayAnimation(b);
	}

	bool CEditModelGroup::GetIsPlayAnimation() const
	{
		if( m_vecSelfPresentation.empty() )
			return false;
		else
			return m_vecSelfPresentation[0]->GetIsPlayAnimation();
	}

	void CEditModelGroup::TitleLinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
	{
		if( m_vecTileSelfPresentation.empty() )
			return;

		m_vecTileSelfPresentation[0]->LinkEffect(pEffect,pGroup);
	}

	void CEditModelGroup::TitleUnlinkEffect()
	{
		if( m_vecTileSelfPresentation.empty() )
			return;

		m_vecTileSelfPresentation[0]->UnlinkEffect();
	}

	void CEditModelGroup::HideSelf()
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 mem_fun(&CMapEditObject::HideSelf));
	}

	void CEditModelGroup::ShowSelf()
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 mem_fun(&CMapEditObject::ShowSelf));
	}

	bool CEditModelGroup::GetIsEffectBased()
	{
		if( m_vecSelfPresentation.empty() )
			return false;

		return m_vecSelfPresentation[0]->GetIsEffectBased();
	}

	string CEditModelGroup::GetSceneCueName() const
	{
		if( m_vecSelfPresentation.empty() )
			return "";

		return m_vecSelfPresentation[0]->GetSceneCueName();
	}

	bool CEditModelGroup::GetSelected()
	{
		return m_vecSelfPresentation[0]->GetSelected();
	}

	bool CEditModelGroup::GetLocked()
	{
		return m_vecSelfPresentation[0]->GetLocked();
	}

	void CEditModelGroup::SetIsEffectBased(bool b)
	{
		m_vecSelfPresentation[0]->SetIsEffectBased(b);
	}

	void CEditModelGroup::SetSceneCueName(const string& name)
	{
		if( m_vecSelfPresentation.empty() )
			return;

		m_vecSelfPresentation[0]->SetSceneCueName(name);
	}

	void CEditModelGroup::SetResGroupMark(BYTE bResGroupMark)
	{
		m_vecSelfPresentation[0]->SetResGroupMark(bResGroupMark);
	}

	BYTE CEditModelGroup::GetResGroupMark()
	{
		return m_vecSelfPresentation[0]->GetResGroupMark();
	}

	void CEditModelGroup::SetEffectNameForGroup(string str)
	{
		m_strEffectNameForGroup = str;
	}

	string CEditModelGroup::GetEffectNameForGroup()
	{
		return m_strEffectNameForGroup;
	}

	void CEditModelGroup::SetIsCameraOriented(bool b)
	{
		m_vecSelfPresentation[0]->SetIsCameraOriented(b);
	}

	bool CEditModelGroup::GetIsCameraOriented()
	{
		return m_vecSelfPresentation[0]->GetIsCameraOriented();
	}

	bool CEditModelGroup::GetEffectName(string & strEffectName)
	{
		return m_vecSelfPresentation[0]->GetEffectName(strEffectName);
	}

	bool CEditModelGroup::GetEffectFileName(string & strEffectFileName)
	{
		return m_vecSelfPresentation[0]->GetEffectFileName(strEffectFileName);
	}

	float CEditModelGroup::GetHeight()
	{
		return m_vecSelfPresentation[0]->GetHeight();
	}

	float CEditModelGroup::GetHeight(const size_t index)
	{
		if ( index < m_vecSelfPresentation.size() )
			return m_vecSelfPresentation[index]->GetHeight();
		else
			return 0.0f;
	}

	void CEditModelGroup::SetModelTransState( const byte transState )
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun(&CMapEditObject::SetModelTransState),transState) );
	}

	byte CEditModelGroup::GetModelTransState(const size_t index) const
	{
		if ( index < m_vecSelfPresentation.size() )
			return m_vecSelfPresentation[index]->GetModelTransState();
		else
			return 0;
	}

	byte CEditModelGroup::GetModelTransState() const
	{
		return m_vecSelfPresentation[0]->GetModelTransState();
	}

	IEffectGroup * CEditModelGroup::GetEffectGroup()
	{
		return m_vecSelfPresentation[0]->GetEffectGroup();
	}

	IEffect * CEditModelGroup::GetEffect()
	{
		return m_vecSelfPresentation[0]->GetEffect(0);
	}

	void CEditModelGroup::GetFinalMatrix(CMatrix & mat)
	{
		m_vecSelfPresentation[0]->GetFinalMatrix(mat);
	}

	int CEditModelGroup::GetRenderStyle()
	{
		return m_vecSelfPresentation[0]->GetRenderStyle();
	}

	void CEditModelGroup::SetRenderStyle(int nRenderStyle)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetRenderStyle),nRenderStyle) );
	}

	void CEditModelGroup::IncAtten()
	{
		++m_nAtten;

		if ( m_nAtten > ATTENTRUM )
		{
			m_nAtten = ATTENTRUM;
			return;
		}

		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 mem_fun(&CMapEditObject::IncAtten));

		this->UpdateOverallAmbientRatio();
	}

	void CEditModelGroup::IncAffectRatio()
	{
		m_nAffectRatio += 4;
		if ( m_nAffectRatio > 256  )
		{
			m_nAffectRatio = 256;
			return;
		}

		this->UpdateOverallAmbientRatio();
	}

	void CEditModelGroup::DecAffectRatio()
	{
		m_nAffectRatio -= 4;
		if ( m_nAffectRatio < 0)
		{
			m_nAffectRatio = 0;
			return;
		}

		this->UpdateOverallAmbientRatio();
	}


	int CEditModelGroup::GetAffectRatio() const
	{
		return m_nAffectRatio;
	}

	void CEditModelGroup::SetAffectRatio( int nRatio)
	{
		m_nAffectRatio = nRatio;
		this->UpdateOverallAmbientRatio();
	}

	void CEditModelGroup::DecAtten()
	{
		--m_nAtten;

		if ( m_nAtten < -ATTENTRUM )
		{
			m_nAtten = -ATTENTRUM;
			return;
		}

		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 mem_fun(&CMapEditObject::DecAtten));
		this->UpdateOverallAmbientRatio();
	}

	int CEditModelGroup::GetAtten()
	{
		return m_nAtten;
	}

	void CEditModelGroup::SetAtten(int nAtten)
	{
		m_nAtten = nAtten;

		if ( m_nAtten > 0 )
		{
			for ( int i = 0; i < nAtten; ++i )
				for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
						 mem_fun(&CMapEditObject::IncAtten));
		}
		else
		{
			for ( int i = 0; i < -nAtten; ++i )
				for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
						 mem_fun(&CMapEditObject::DecAtten));
		}

		this->UpdateOverallAmbientRatio();
	}

	bool CEditModelGroup::GetHide()
	{
		return m_bHide;
	}

	float CEditModelGroup::GetShiftX()
	{
		return m_vecSelfPresentation[0]->GetShiftX();
	}

	float CEditModelGroup::GetShiftZ()
	{
		return m_vecSelfPresentation[0]->GetShiftZ();
	}

	float CEditModelGroup::GetLiftx()
	{
		return m_vecSelfPresentation[0]->GetLiftx();
	}

	bool CEditModelGroup::GetIsTranslucent()
	{
		return m_vecSelfPresentation[0]->GetIsTranslucent();
	}

	void CEditModelGroup::SetCastShadow( bool b )
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetCastShadow),b) );
	}

	bool CEditModelGroup::IsCastShadow()
	{
		return m_vecSelfPresentation[0]->IsCastShadow();
	}

	void CEditModelGroup::SetReceiveShadow(const bool bReceiveShadow)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
				 bind2nd(mem_fun(&CMapEditObject::SetReceiveShadow),bReceiveShadow) );
	}

	bool CEditModelGroup::IsReceiveShadow() const
	{
		return m_vecSelfPresentation[0]->IsReceiveShadow();
	}

	vector<string> CEditModelGroup::GetAniNameList() const
	{
		return m_vecSelfPresentation[0]->GetAniNameList();
	}

	void CEditModelGroup::SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed )
	{
		return m_vecSelfPresentation[0]->SetNextAnimation(szAniFileName, bLoop, DelayTime, AniSpeed);
	}

	void CEditModelGroup::SetAnimationFrame( const string& str, const int8 nFrame )
	{
		m_vecSelfPresentation[0]->SetAnimationFrame(str, nFrame);
	}

	int16 CEditModelGroup::GetAnimationFrame( const string& str ) const
	{
		return m_vecSelfPresentation[0]->GetAnimationFrame(str);
	}

	vector<int16> CEditModelGroup::GetAnimationFrames( const string& str )
	{
		return m_vecSelfPresentation[0]->GetAnimationFrames(str);
	}

	void CEditModelGroup::ChangeAnimation()
	{
		m_vecSelfPresentation[0]->ChangeAnimation();
	}

	void CEditModelGroup::CleanAniFrames(const string& str)
	{
		m_vecSelfPresentation[0]->CleanAniFrames(str);
	}

	void CEditModelGroup::DeleteAniFrame(const string& str, const int nCurrKey)
	{
		m_vecSelfPresentation[0]->DeleteAniFrame(str, nCurrKey);
	}

	void CEditModelGroup::SetIsVisibleByConfigure( const bool b )
	{
		m_vecSelfPresentation[0]->SetIsVisibleByConfigure(b);
	}

	bool CEditModelGroup::GetIsVisibleByConfigure() const
	{
		return m_vecSelfPresentation[0]->GetIsVisibleByConfigure();
	}

	void CEditModelGroup::SetLoginModelLogicName( const string& name )
	{
		m_vecSelfPresentation[0]->SetLoginModelLogicName(name);
	}

	string CEditModelGroup::GetLoginModelLogicName() const
	{
		return m_vecSelfPresentation[0]->GetLoginModelLogicName();
	}

	void  CEditModelGroup::SetLoginModelPalyAniFromStart(const bool b)
	{
		m_vecSelfPresentation[0]->SetLoginModelPalyAniFromStart(b);
	}

	void CEditModelGroup::SetIsLightEffect(const bool b)
	{
		m_vecSelfPresentation[0]->SetIsLightEffect(b);
	}

	bool CEditModelGroup::GetIsLightEffect() const
	{
		return m_vecSelfPresentation[0]->GetIsLightEffect();
	}

	void CEditModelGroup::SetUserOffset( CVector3f vec )
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun<void,CMapEditObject,CVector3f>(&CMapEditObject::SetUserOffset),vec) );
	}

	void CEditModelGroup::SetMoldeSaveProperty(const MoldeSaveProperty *pModeSavePro )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CVector3f worldpos = m_vecSelfPresentation[i]->sModelSavePro.vPosition;
			m_vecSelfPresentation[i]->sModelSavePro = *pModeSavePro;
			
			//////////////////////////////////////////////////////////////////////////
			///临时的，如果不加也可以让美术把地图保存一下
			if( worldpos != CVector3f(0.0f, 0.0f, 0.0f) && pModeSavePro->vPosition == CVector3f(0.0f, 0.0f, 0.0f) )
				m_vecSelfPresentation[i]->sModelSavePro.vPosition = worldpos;
			//////////////////////////////////////////////////////////////////////////
		}
	}

	void CEditModelGroup::SetWorldPosition(const CVector3f& pos)
	{
		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
			bind2nd(mem_fun<void,CMapEditObject,const CVector3f>(&CMapEditObject::SetWorldPosition),pos) );
	}

	void CEditModelGroup::SetPlayTimeLenMin( const DWORD dwTimeMin )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecSelfPresentation[i]->SetPlayTimeLenMin(dwTimeMin);
		}
	}

	void CEditModelGroup::SetPlayTimeLenMax( const DWORD dwTimeMax )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecSelfPresentation[i]->SetPlayTimeLenMax(dwTimeMax);
		}
	}

	void CEditModelGroup::SetPlayIntervalMin( const DWORD dwTimeMin )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecSelfPresentation[i]->SetPlayIntervalMin(dwTimeMin);
		}
	}

	void CEditModelGroup::SetPlayIntervalMax( const DWORD dwTimeMax )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecSelfPresentation[i]->SetPlayIntervalMax(dwTimeMax);
		}
	}

	DWORD CEditModelGroup::GetPlayTimeLenMin()
	{
		return m_vecSelfPresentation[0]->GetPlayTimeLenMin();
	}

	DWORD CEditModelGroup::GetPlayTimeLenMax()
	{
		return m_vecSelfPresentation[0]->GetPlayTimeLenMax();
	}

	DWORD CEditModelGroup::GetPlayIntervalMin()
	{
		return m_vecSelfPresentation[0]->GetPlayIntervalMin();
	}

	DWORD CEditModelGroup::GetPlayIntervalMax()
	{
		return m_vecSelfPresentation[0]->GetPlayIntervalMax();
	}

	DWORD CEditModelGroup::GetPlayTimeLen()
	{
		return m_vecSelfPresentation[0]->GetPlayTimeLen();
	}

	DWORD CEditModelGroup::GetCurPlayIntervalTime()
	{
		return m_vecSelfPresentation[0]->GetCurPlayIntervalTime();
	}

	void CEditModelGroup::SetBakingColorIsNotGreaterShadow( const bool b )
	{
		size_t size = m_vecSelfPresentation.size();
		for ( size_t i = 0; i < size; ++i )
		{
			m_vecSelfPresentation[i]->SetBakingColorIsNotGreaterShadow(b);
		}
	}

	bool CEditModelGroup::GetBakingColorIsNotGreaterShadow()
	{
		return m_vecSelfPresentation[0]->GetBakingColorIsNotGreaterShadow();
	}

	//void CEditModelGroup::UpdateBoundingBox( bool bAudioUpdate )
	//{
	//	size_t size = m_vecSelfPresentation.size();
	//	for ( size_t i = 0; i < size; ++i )
	//	{
	//		m_vecSelfPresentation[i]->UpdateBoundingBox(bAudioUpdate);
	//	}
	//}

	void CEditModelGroup::AddObjectRenderDialog( CTObject *pObject )
	{
 		for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
 			bind2nd(mem_fun(&CMapEditObject::AddObjectRenderDialog),pObject) );
	}

	void CEditModelGroup::SetVisible( bool b )
	{
		m_vecSelfPresentation[0]->SetVisible(b);
	}
}
