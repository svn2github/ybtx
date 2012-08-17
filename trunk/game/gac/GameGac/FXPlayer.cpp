#include "stdafx.h"
#include "FXPlayer.h"
#include "CAppClient.h"
#include "CoreCommon.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "StringHelper.h"
#include "CRenderSystemClient.h"
#include "CEntityClient.h"
#include "CEntityClientManager.h"
#include "PatternRef.inl"

CTargetDirectionFX* CTargetDirectionFX::ms_pTargetDirectionFX = NULL;
vector<string> CBaseFX::Split(const string& src, const string& delimit)  
{  
	vector<string> v;
	if( src.empty() || delimit.empty() )
	{
		v.push_back("");
		v.push_back("");
		return v;
	}

	int m,n;
	m=src.find_first_of("\"");  
	n=src.find_last_of("\"");  
	string str=src.substr(m>=0?m+1:0,n-1>0?n-1:src.size());  

	basic_string<TCHAR>::size_type delim_len = delimit.size();
	int32 index = std::string::npos, last_search_position = 0;  
	while((index=str.find(delimit, last_search_position)) != std::string::npos)  
	{  
		if(index == last_search_position)  
			v.push_back("");  
		else  
			v.push_back( str.substr(last_search_position, index - last_search_position) );  
		last_search_position = index + delim_len;  
	}  
	string last_one = str.substr(last_search_position);  
	v.push_back( last_one.empty()? "":last_one );
	return v;  
}

void CBaseFX::FixFXPath(string& strPath) 
{  
	int32 index = std::string::npos, last_search_position = 0;  
	while((index=strPath.find("//", last_search_position)) != std::string::npos)  
	{  
		strPath.replace(index,strlen("//"),"/");
	}  
}


CBaseFX::CBaseFX()
:m_uMask(0)
{

}

CBaseFX::~CBaseFX()
{
	CAppClient::Inst()->UnRegisterTick(this);
}

void CBaseFX::PlayFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime)
{
	string strFXNameFile = strFXNameGroup;
	FixFXPath(strFXNameFile);
	m_sName = strFXNameFile + "," + strFXName;
	if (uDelayTime==0)
	{
		PlayEffect(strFXNameFile, strFXName);
	}
	else
	{
		CAppClient::Inst()->RegisterTick(this,uDelayTime);
	}
}

void CBaseFX::ReplayFX()
{
	vector<string> sFXTable = Split(m_sName, ",");
	PlayEffect(sFXTable[0].c_str(), sFXTable[1].c_str());
}

void CBaseFX::OnTick()
{
	CAppClient::Inst()->UnRegisterTick(this);
	vector<string> strFXTable = Split(m_sName, ",");
	PlayEffect(strFXTable[0].c_str(), strFXTable[1].c_str());
}

CPlayerFX::CPlayerFX(CEntityClient* pEntity)
:m_bLoop(false)
{
	if (pEntity)
	{
		m_uEntityID=pEntity->GetEntityID();
	}
}

CPlayerFX::~CPlayerFX()
{
}

void CPlayerFX::PlayOnceFX(CEntityClient* pSrcEntity ,const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime)
{
	if(NULL == pSrcEntity)
		return;

	CPlayerFX* pFX =new CPlayerFX(pSrcEntity);
	pFX->PlayFX(strFXNameGroup,strFXName,uDelayTime);
}

void CPlayerFX::PlayLoopFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime)
{
	if (m_uMask==0)
	{
		m_bLoop=true;
		PlayFX(strFXNameGroup,strFXName,uDelayTime);
	}
}

void CPlayerFX::PlayEffect(const string &strFXNameGroup, const string &strFXName)
{
	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uEntityID);
	if (pEntity&&pEntity->GetRenderObject())
	{
		int iLoop = m_bLoop?-1:0;
		uint32 uMask = pEntity->GetRenderObject()->AddEffect(strFXNameGroup.c_str(),strFXName.c_str(), iLoop,NULL);
		if (m_bLoop)
		{
			m_uMask = uMask;
		}
	}
	if (!m_bLoop)
	{
		CancelFx();
	}
}

void CPlayerFX::CancelFx()
{
	StopFX();
	delete this;
}

void CPlayerFX::StopFX()
{
	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uEntityID);
	if (pEntity&&pEntity->GetRenderObject()&&m_uMask!=0)
	{
		pEntity->GetRenderObject()->DelEffect(m_uMask);
		m_uMask = 0;
	}
}

CPositionFX::CPositionFX(CRenderScene* pRenderScene,const CFPos& PixelPos)
:m_Pos(PixelPos)
{
	m_pPosRenderObj=pRenderScene->CreateRenderObject(NULL);
	m_pPosRenderObj->SetModelHandler( this );
	m_pPosRenderObj->SetNodeHandler(this);
	m_pPosRenderObj->SetCanBeSelect(false);
}

CPositionFX::~CPositionFX()
{
	if (CClientRenderScene::HasInst())
	{
		if(m_pPosRenderObj) 
		{
			m_pPosRenderObj->SetModelHandler(NULL);
			m_pPosRenderObj->SetNodeHandler(NULL);
			CClientRenderScene::GetInst()->DestroyRenderObject(m_pPosRenderObj);
			m_pPosRenderObj=NULL;
		}
	}
}

void CPositionFX::CppPlayPosFX(const TCHAR* strFXNameGroup, const TCHAR* strFXName, const CFPos& PixelPos)
{
	PlayOnceFX(strFXNameGroup,strFXName,PixelPos,0);
}

void CPositionFX::PlayOnceFX(const string &strFXNameGroup, const string &strFXName, const CFPos& PixelPos, uint32 uDelayTime)
{
	if (CClientRenderScene::HasInst())
	{
		CPositionFX* pFX = new CPositionFX(CClientRenderScene::GetInst(),PixelPos);
		pFX->PlayFX(strFXNameGroup,strFXName,uDelayTime);
	}
}

void CPositionFX::PlayEffect(const string &strFXNameGroup, const string &strFXName)
{
	if (!CClientRenderScene::HasInst())
	{
		delete this;
		return;
	}
	CRenderScene* pRenderScene=CClientRenderScene::GetInst();

	float fRatio = eGridSpanForObj / eGridSpan;
	pRenderScene->AttachRenderObject(*m_pPosRenderObj,CVector3f(m_Pos.x / fRatio, 0, m_Pos.y / fRatio));

	m_pPosRenderObj->AddEffect( strFXNameGroup.c_str(), strFXName.c_str(), CEffectPlayer::PLAY_ONCE, NULL );
	//m_pPosRenderObj->AddPiece( RFT_ARE, strFXNameGroup.c_str(), "", strFXName.c_str() );
}

void CPositionFX::OnDestroyed()
{
	m_pPosRenderObj = NULL;
	delete this;
}

void CPositionFX::OnLinkAnimationEnd(const TCHAR * szName)
{
	delete this;
}

CLeadingFX::CLeadingFX(CEntityClient* pEntity)
:CPlayerFX(pEntity)
,m_uTargetID(0)
{
	
}

void CLeadingFX::CancelFx()
{
	StopFX();
	Release();
}

void CLeadingFX::StopFX()
{
	m_mapDest.clear();
	m_uTargetID = 0;
	CPlayerFX::StopFX();
}

void CLeadingFX::PlayOnceFX(CEntityClient* pSrcEntity, const string &strFXNameGroup, const string &strFXName, CVector3f& vec3f, uint32 uDelayTime)
{
	if( pSrcEntity->GetRenderObject()) 
	{	
		CLeadingFX* pFX = new CLeadingFX(pSrcEntity);
		pFX->m_mapDest.insert(make_pair(1.0f,vec3f));
		pFX->PlayFX(strFXNameGroup,strFXName,uDelayTime);
	}	

}

void CLeadingFX::PlayLoopFX(const string &strFXNameGroup, const string &strFXName, CEntityClient* pTarget, uint32 uDelayTime)
{
	if( pTarget && pTarget->GetRenderObject()&&pTarget->GetRenderObject()->IsValid()&&m_uMask==0) 
	{
		m_uTargetID = pTarget->GetEntityID();
		m_bLoop=true;
		PlayFX(strFXNameGroup,strFXName,uDelayTime);
	}	
}

void CLeadingFX::PlayLoopFX(const string &strFXNameGroup, const string &strFXName, CVector3f& vec3f, uint32 uDelayTime)
{
	if( m_uMask==0) 
	{
		m_mapDest.insert(make_pair(1.0f,vec3f));
		m_bLoop=true;
		PlayFX(strFXNameGroup,strFXName,uDelayTime);
	}	
}

void CLeadingFX::PlayEffect(const string &strFXNameGroup, const string &strFXName)
{
	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uEntityID);
	if (pEntity&&pEntity->GetRenderObject())
	{
		int iLoop = m_bLoop?-1:0;
		uint32 uMask = pEntity->GetRenderObject()->AddEffect(strFXNameGroup.c_str(),strFXName.c_str(), iLoop,NULL);
		if (m_bLoop)
		{
			m_uMask = uMask;
		}
		pEntity->GetRenderObject()->SetEffectUnitHandler(uMask,this,"",eLeading);
	}
	if (!m_bLoop)
	{
		Release();
	}
}

void	CLeadingFX::AddLeadingPos(CVector3f& vec3f)
{
	if (!m_mapDest.empty())
	{
		EMap<float,CVector3f>::iterator it = m_mapDest.end();
		float fIndex =	((--it)->first)+1.0f;
		m_mapDest.insert(make_pair(fIndex,vec3f));
	}
}

bool CLeadingFX::RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS )
{
	IEffectUnitProp* pProp = pUnit->GetProp();
	if(/* !bAlphaBlend ||*/ !pUnit->IsInRenderTime() )
	{	
		return false;
	}

	CEntityClient* pTarget = CEntityClientManager::GetInst()->GetEntityByID(m_uTargetID);
	if(pTarget)
	{
		EMap<float,CVector3f>& MapLeading = pUnit->GetDestMap();
		CVector3f vec3f = pTarget->GetRenderObject()->GetCenterPosition();
		MapLeading[1.0f] = vec3f;
		return true;
	}
	else if (!m_mapDest.empty())
	{
		EMap<float,CVector3f>& MapLeading = pUnit->GetDestMap();
		MapLeading = m_mapDest;
		return true;
	}

	return false;
}

void* CLeadingFX::operator new( size_t stAllocateBlock)
{
	return	CFXMallocObject::operator new(stAllocateBlock);
}

void* CLeadingFX::operator new( size_t stAllocateBlock, void* ptr)
{
	return CFXMallocObject::operator new(stAllocateBlock,ptr);
}

void* CLeadingFX::operator new[]( size_t stAllocateBlock)
{
	return CFXMallocObject::operator new[](stAllocateBlock);
}

void CLeadingFX::operator delete(void* pMem)
{
	return CFXMallocObject::operator delete(pMem);
}

void CLeadingFX::operator delete(void* pMem1, void* pMem2)
{
	return CBaseFX::operator delete(pMem1,pMem2);
}

void CLeadingFX::operator delete[](void* pMem)
{
	return CBaseFX::operator delete[](pMem);
}


CMagicFX::CMagicFX()
{
	m_RefByMagic.SetHolder(this);
	m_pRenderObj=CClientRenderScene::GetInst()->CreateRenderObject(NULL);
	m_pRenderObj->SetNodeHandler(this);
	m_pRenderObj->SetCanBeSelect(false);
}

CMagicFX::~CMagicFX()
{
	if (CClientRenderScene::HasInst())
	{
		if(m_pRenderObj) 
		{
			m_pRenderObj->SetNodeHandler(NULL);
			CClientRenderScene::GetInst()->DestroyRenderObject(m_pRenderObj);
			m_pRenderObj=NULL;
		}
	}
}

void CMagicFX::OnDestroyed()
{
	m_pRenderObj = NULL;
	delete this;
}

void CMagicFX::PlayLoopFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime)
{
	PlayFX(strFXNameGroup,strFXName,uDelayTime);
}

void CMagicFX::PlayEffect(const string &strFXNameGroup, const string &strFXName)
{
	if (m_uMask!=0)
	{
		m_pRenderObj->DelEffect(m_uMask);
	}
	m_uMask =m_pRenderObj->AddEffect(strFXNameGroup.c_str(),strFXName.c_str(), -1,NULL);
}

void CMagicFX::CancelFx()
{
	delete this;
}


CTargetDirectionFX::CTargetDirectionFX(const string &strFXNameGroup, const string &strFXName,const CFPos& PixelPos)
:m_Pos(PixelPos)
{
	uint32 uEntityID = CEntityClientManager::GetInst()->GetDirectorID();
	CEntityClient* pEntity = CEntityClientManager::GetEntityByID(uEntityID);
	if (!pEntity)
	{
		delete this;
		return;
	}
	m_uMask  = pEntity->GetRenderObject()->AddEffect(strFXNameGroup.c_str(),strFXName.c_str(), -1,NULL);
	pEntity->GetRenderObject()->SetEffectUnitHandler(m_uMask,this,"",eNull);
}

CTargetDirectionFX::~CTargetDirectionFX()
{
	if (ms_pTargetDirectionFX==this)
	{
		ms_pTargetDirectionFX = NULL;
	}
}

void CTargetDirectionFX::CppPlayTargetDirFX(const TCHAR* strFXNameGroup, const TCHAR* strFXName,const CFPos& PixelPos)
{
	CppCancelTargetDirFX();
	CTargetDirectionFX* pFX = new CTargetDirectionFX(strFXNameGroup,strFXName,PixelPos);
	pFX->Release();
	ms_pTargetDirectionFX = pFX;
}

void CTargetDirectionFX::CppCancelTargetDirFX()
{
	if (ms_pTargetDirectionFX)
	{
		uint32 uEntityID = CEntityClientManager::GetInst()->GetDirectorID();
		CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(uEntityID);
		if (pEntity&&pEntity->GetRenderObject()&&ms_pTargetDirectionFX->m_uMask!=0)
		{
			pEntity->GetRenderObject()->DelEffect(ms_pTargetDirectionFX->m_uMask);
		}
	}
}

bool CTargetDirectionFX::RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS )
{
	IEffectUnitProp* pProp = pUnit->GetProp();
	if(/* !bAlphaBlend ||*/ !pUnit->IsInRenderTime() )
	{	
		return false;
	}

	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID( CEntityClientManager::GetInst()->GetDirectorID());
	CFPos Pos = pEntity->GetPixelPos();
	CVector2f vec2f(m_Pos.x-Pos.x,m_Pos.y-Pos.y);
	CDir dir;
	dir.Set(vec2f);
	float fRotate = dir.uDir* CMath::pi *2/256;
	CVector3f vec3f =matWorld.GetAxis(3);
	matWorld.SetRotateY(fRotate);
	matWorld._41 = vec3f.x;
	matWorld._42 = vec3f.y;
	matWorld._43 = vec3f.z;
	return true;
}

CLineDirectionFX::CLineDirectionFX(CEntityClient* pEntity, uint32 uTargetEntityID)
:m_uEntityID(pEntity->GetEntityID())
,m_uTargetEntityID(uTargetEntityID)
{
}


void CLineDirectionFX::PlayOnceFX(CEntityClient* pSrcEntity, const string &strFXNameGroup, const string &strFXName, uint32 uTargetEntityID)
{
	if( pSrcEntity->GetRenderObject()) 
	{	
		CLineDirectionFX* pFX = new CLineDirectionFX(pSrcEntity,uTargetEntityID);
		pFX->PlayEffect(strFXNameGroup,strFXName);
	}	
}

void CLineDirectionFX::PlayEffect(const string &strFXNameGroup, const string &strFXName)
{
	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uEntityID);
	if (pEntity&&pEntity->GetRenderObject())
	{
		uint32 uMask = pEntity->GetRenderObject()->AddEffect(strFXNameGroup.c_str(),strFXName.c_str(), 0,NULL);
		pEntity->GetRenderObject()->SetEffectUnitHandler(uMask,this,"xz");
	}
	Release();
}


bool CLineDirectionFX::RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS )
{
	CEntityClient* pTargetEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uTargetEntityID);
	CEntityClient* pEntity = CEntityClientManager::GetInst()->GetEntityByID(m_uEntityID);
	IEffectUnitProp* pProp = pUnit->GetProp();
	if(/* !bAlphaBlend ||*/ !pUnit->IsInRenderTime()||!pEntity||!pTargetEntity)
	{	
		return false;
	}

	CFPos TargetPos = pTargetEntity->GetPixelPos();
	CFPos Pos = pEntity->GetPixelPos();
	CVector2f vec2f(TargetPos.x-Pos.x,TargetPos.y-Pos.y);
	CDir dir;
	dir.Set(vec2f);
	float fRotate = dir.uDir* CMath::pi *2/256;
	CVector3f vec3f =matWorld.GetAxis(3);
	matWorld.SetRotateY(fRotate);
	matWorld._41 = vec3f.x;
	matWorld._42 = vec3f.y;
	matWorld._43 = vec3f.z;
	return true;
}