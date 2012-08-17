#include "stdafx.h"
#include "CRegionAmbientEffectMgr.h"
#include "UserMsg.h"
#include "CEffectGroup.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMesh.h"
#include "ToolSetModel.h"
#include "ToolRegion.h"
#include "CMapEditObject.h"
#include "SceneRegionMgr.h"

sqr::CMapRegionAmbientEffectMgr::CMapRegionAmbientEffectMgr()
{
}

sqr::CMapRegionAmbientEffectMgr::~CMapRegionAmbientEffectMgr()
{
	RegionAmbFxObjectMap::iterator beg = m_RgnAmbientFxObjectList.begin();
	RegionAmbFxObjectMap::iterator end = m_RgnAmbientFxObjectList.end();
	for ( beg; beg != end; ++beg )
	{
		map<string, CTObject *> fxObjectList = beg->second;
		map<string, CTObject *>::iterator subbeg = fxObjectList.begin();
		map<string, CTObject *>::iterator subend = fxObjectList.end();
		for ( subbeg; subbeg != subend; ++subbeg )
		{
			subbeg->second = NULL;
		}

		fxObjectList.clear();
	}
	m_RgnAmbientFxObjectList.clear();
}

void sqr::CMapRegionAmbientEffectMgr::CreateRgnAmbientFxObject( const string& rgnname, const string& effectname )
{
	HWND hRegionWnd = CToolRegion::GetInst()->GetRegionView();

	CVector3f * pV = NULL;
	::SendMessage(hRegionWnd, WM_REGIONVIEW_REBIRTHFX, (WPARAM)(rgnname+"@"+effectname).c_str(), (LPARAM)pV);
}

void sqr::CMapRegionAmbientEffectMgr::AddRgnAmbientFxObject( const string& rgnname, const string& effectname, CTObject * p )
{
	m_RgnAmbientFxObjectList[rgnname].insert(make_pair(effectname, p));
}

void sqr::CMapRegionAmbientEffectMgr::DeleteRgnAmbientFxObject( const string& rgnname, const string& effectname )
{
	RegionAmbFxObjectMap::iterator iter = m_RgnAmbientFxObjectList.find(rgnname);
	if( iter != m_RgnAmbientFxObjectList.end() )
	{
		map<string, CTObject *>::iterator subiter = iter->second.find(effectname);
		if( subiter != iter->second.end() )
		{
			CTObjectPtr pObject = subiter->second;
			if( pObject )
			{
				CEditModelGroup *pModelGroup = pObject->GetModelGroup();
				pModelGroup->SetEditState(sqr::EES_DELETE);

				int nCenterGridIndex = pModelGroup->GetCenterGridIndex();
				SGrid &gridCenter = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nCenterGridIndex);
				gridCenter.vecCenterObjects.erase(
 					remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), pObject), gridCenter.vecCenterObjects.end() );
			}
			iter->second.erase(subiter);
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::SetAmientFxPlayTime( const string& regname, const string& effectname, const DWORD dwPlayTimeMin, const DWORD dwPlayTimeMax )
{
	RegionAmbFxObjectMap::iterator iter = m_RgnAmbientFxObjectList.find(regname);
	if( iter != m_RgnAmbientFxObjectList.end() )
	{
		map<string, CTObject *>::iterator subiter = iter->second.find(effectname);
		if( subiter != iter->second.end() )
		{
			CTObject *pObject = subiter->second;
			if( pObject )
			{
				CEditModelGroup *pModelGroup = pObject->GetModelGroup();
				if( pModelGroup )
				{
					pModelGroup->SetPlayTimeLenMin(dwPlayTimeMin);
					pModelGroup->SetPlayTimeLenMax(dwPlayTimeMax);
				}
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::GetAmientFxPlayTime( const string& regname, const string& effectname, DWORD& dwPlayTimeMin, DWORD& dwPlayTimeMax )
{
	RegionAmbFxObjectMap::iterator iter = m_RgnAmbientFxObjectList.find(regname);
	if( iter != m_RgnAmbientFxObjectList.end() )
	{
		map<string, CTObject *>::iterator subiter = iter->second.find(effectname);
		if( subiter != iter->second.end() )
		{
			CTObject *pObject = subiter->second;
			if( pObject )
			{
				CEditModelGroup *pModelGroup = pObject->GetModelGroup();
				if( pModelGroup )
				{
					dwPlayTimeMin = pModelGroup->GetPlayTimeLenMin();
					dwPlayTimeMax = pModelGroup->GetPlayTimeLenMax();
				}
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::SetAmientFxIntervalTime( const string& regname, const string& effectname, const DWORD dwPlayTimeMin, const DWORD dwPlayTimeMax )
{
	RegionAmbFxObjectMap::iterator iter = m_RgnAmbientFxObjectList.find(regname);
	if( iter != m_RgnAmbientFxObjectList.end() )
	{
		map<string, CTObject *>::iterator subiter = iter->second.find(effectname);
		if( subiter != iter->second.end() )
		{
			CTObject *pObject = subiter->second;
			if( pObject )
			{
				CEditModelGroup *pModelGroup = pObject->GetModelGroup();
				if( pModelGroup )
				{
					pModelGroup->SetPlayIntervalMin(dwPlayTimeMin);
					pModelGroup->SetPlayIntervalMax(dwPlayTimeMax);
				}
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::GetAmientFxIntervalTime( const string& regname, const string& effectname, DWORD& dwPlayTimeMin, DWORD& dwPlayTimeMax )
{
	RegionAmbFxObjectMap::iterator iter = m_RgnAmbientFxObjectList.find(regname);
	if( iter != m_RgnAmbientFxObjectList.end() )
	{
		map<string, CTObject *>::iterator subiter = iter->second.find(effectname);
		if( subiter != iter->second.end() )
		{
			CTObject *pObject = subiter->second;
			if( pObject )
			{
				CEditModelGroup *pModelGroup = pObject->GetModelGroup();
				if( pModelGroup )
				{
					dwPlayTimeMin = pModelGroup->GetPlayIntervalMin();
					dwPlayTimeMax = pModelGroup->GetPlayIntervalMax();
				}
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::SwitchShowAllRgnAmbientEffect( const bool b )
{
	RegionAmbFxObjectMap::iterator beg = m_RgnAmbientFxObjectList.begin();
	RegionAmbFxObjectMap::iterator end = m_RgnAmbientFxObjectList.end();
	for ( beg; beg != end; ++beg )
	{
		map<string, CTObject *> fxObjects = beg->second;
		map<string, CTObject *>::iterator iter = fxObjects.begin();
		for ( iter; iter != fxObjects.end(); ++iter )
		{
			if( b )
				iter->second->GetModelGroup()->ShowSelf();
			else
				iter->second->GetModelGroup()->HideSelf();
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::RenderAllReginAmbientFx()
{
	RegionAmbFxObjectMap::iterator beg = m_RgnAmbientFxObjectList.begin();
	RegionAmbFxObjectMap::iterator end = m_RgnAmbientFxObjectList.end();
	for ( beg; beg != end; ++beg )
	{
		map<string, CTObject *> fxObjects = beg->second;
		map<string, CTObject *>::iterator iter = fxObjects.begin();
		for ( iter; iter != fxObjects.end(); ++iter )
		{
			iter->second->Render();
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::RenderLogicReginAmbFxByPlayer()
{

}

void sqr::CMapRegionAmbientEffectMgr::SaveRgnAmbientFxInfo( FILE *fp, bool bResize, bool bSplit )
{
	COperator *pOperator = CMapEditApp::GetInst()->GetOperator();

	size_t size = m_RgnAmbientFxObjectList.size();
	fwrite(&size, sizeof(size), 1, fp);

	map<int, int>::iterator iiter;
	RegionAmbFxObjectMap::iterator beg = m_RgnAmbientFxObjectList.begin();
	RegionAmbFxObjectMap::iterator end = m_RgnAmbientFxObjectList.end();
	for ( beg; beg != end; ++beg )
	{
		map<string, CTObject *> fxObjectList = beg->second;
		size_t fxnum = fxObjectList.size();
		if( fxnum == 0 )
			continue;

		string strRgnName = beg->first;

		DWORD dwLen = strRgnName.size();
		fwrite(&dwLen, sizeof(dwLen), 1, fp);
		fwrite(strRgnName.c_str(), dwLen, 1, fp);

		fwrite(&fxnum, sizeof(fxnum), 1, fp);

		map<string, CTObject *>::iterator subbeg = fxObjectList.begin();
		map<string, CTObject *>::iterator subend = fxObjectList.end();
		for ( subbeg; subbeg != subend; ++subbeg )
		{
			CTObjectPtr p = subbeg->second;
			CEditModelGroup *pModelGroup = p->GetModelGroup();

			///login model 跳过
			if( pModelGroup->m_bReginEffect )
			{
				string strEffectName = "";
				pModelGroup->GetEffectName(strEffectName);
				if( strEffectName.empty() )
				{
					MessageBox(NULL,"该模型没有绑定特效","",MB_OK);
					continue;
				}

				///save model property
				CMapEditObject *pMapEditObject = NULL;
				pMapEditObject = pModelGroup->GetSelfPresentation(0);
				if( pModelGroup == NULL )
				{
					MessageBox(NULL,"没有模型，不保存","",MB_OK);
					continue;
				}

				strEffectName = subbeg->first;
				DWORD dwLen = strEffectName.size();
				fwrite(&dwLen,sizeof(int),1,fp);
				fwrite(strEffectName.c_str(),dwLen,1,fp);

				if( bResize && !bSplit )
				{
					int nOldGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
					iiter = pOperator->m_OldMapGridIndexMap.find(nOldGridIndex);
					if( iiter != pOperator->m_OldMapGridIndexMap.end() )
						pMapEditObject->sModelSavePro.nCenterGridIndex = iiter->second;
				}
				else
				{
					if( !bResize && bSplit )
					{
						int nOldGridIndex = pMapEditObject->sModelSavePro.nCenterGridIndex;
						iiter = pOperator->m_SplitOldMapGridIndexMap.find(nOldGridIndex);
						if( iiter != pOperator->m_SplitOldMapGridIndexMap.end() )
							pMapEditObject->sModelSavePro.nCenterGridIndex = iiter->second;
					}
				}

				MoldeSaveProperty sModelSavePro = pMapEditObject->sModelSavePro;

				CVector3f offset;
				pMapEditObject->GetImageOffsetToAdjustedCenterGrid(offset);
				sModelSavePro.vPosition = offset;
				fwrite( &sModelSavePro, sizeof(sModelSavePro),  1, fp);

				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::LoadRgnAmbientFxInfoInfo( FILE * fp )
{
	size_t rgnambientsize = 0;
	fread(&rgnambientsize, sizeof(rgnambientsize), 1, fp);

	for ( size_t i = 0; i < rgnambientsize; ++i )
	{
		string strRgnName = "";
		DWORD  dwLen = 0;
		fread(&dwLen, sizeof(DWORD), 1, fp);
		strRgnName.resize(dwLen);
		fread((TCHAR*)strRgnName.data(), dwLen, 1, fp);

		size_t fxnum = 0;
		fread(&fxnum, sizeof(fxnum), 1, fp);
		for ( size_t j = 0; j < fxnum; ++j )
		{
			string strEffectName = "";
			DWORD  dwItemNameLen = 0;
			fread(&dwItemNameLen,sizeof(DWORD),1,fp);
			strEffectName.resize(dwItemNameLen);
			fread((TCHAR*)strEffectName.data(),dwItemNameLen,1,fp);

			MoldeSaveProperty sModelSavePro;
			fread( &sModelSavePro, sizeof(MoldeSaveProperty), 1, fp);

			this->CreateRgnAmbientFxObject(strRgnName, strEffectName);
			CTObject *pObject = NULL;
			pObject = CToolSetModel::GetInst()->GetLastActiveObject();

			if( CMapEditApp::GetInst()->GetOperator()->GetRebirthResult() ) 
			{
				if( CToolSetModel::GetInst()->Rebirth(&sModelSavePro, 100, "") )
				{
					DWORD dwPlayMin = 0, dwPlayMax = 0, dwIntervalMin = 0, dwIntervalMax = 0;
					CSceneRegionMgr::GetInst()->GetAmientFxPlayTime(strRgnName, strEffectName, dwPlayMin, dwPlayMax);
					CSceneRegionMgr::GetInst()->GetAmientFxIntervalTime(strRgnName, strEffectName, dwIntervalMin, dwPlayMax);

					CSceneRegionMgr::GetInst()->SetAmientFxPlayTime(strRgnName, strEffectName, dwPlayMin, dwPlayMax);
					CSceneRegionMgr::GetInst()->SetAmientFxIntervalTime(strRgnName, strEffectName, dwIntervalMin, dwPlayMax);
				}
			}
		}
	}
}

void sqr::CMapRegionAmbientEffectMgr::SaveRgnAmbientFXInfoAsClient( FILE *fp, map<string, string>& rgnEffectFileNameList, map<string, int>& rgnEffectNameIndexList )
{
	rgnEffectFileNameList.clear();//first effet name; second effect file name
	rgnEffectNameIndexList.clear();

	RegionAmbFxObjectMap::iterator beg = m_RgnAmbientFxObjectList.begin();
	RegionAmbFxObjectMap::iterator end = m_RgnAmbientFxObjectList.end();
	for ( beg; beg != end; ++beg )
	{
		map<string, CTObject *> fxObjectList = beg->second;
		size_t fxnum = fxObjectList.size();
		if( fxnum == 0 )
			continue;

		map<string, CTObject *>::iterator subbeg = fxObjectList.begin();
		map<string, CTObject *>::iterator subend = fxObjectList.end();
		for ( subbeg; subbeg != subend; ++subbeg )
		{
			CTObjectPtr p = subbeg->second;
			CEditModelGroup *pModelGroup = p->GetModelGroup();

			///login model 跳过
			if( pModelGroup->m_bReginEffect )
			{
				string strEffectFileName = "", strEffectName = "";

				pModelGroup->GetEffectFileName(strEffectFileName);
				pModelGroup->GetEffectName(strEffectName);

				if( !strEffectFileName.empty() && !strEffectName.empty() )
					rgnEffectFileNameList[strEffectName] = strEffectFileName;
			}
		}
	}

	size_t size = rgnEffectFileNameList.size();
	fwrite(&size, sizeof(size), 1, fp);

	int nIndex = 0;
	map<string, string>::iterator namebeg = rgnEffectFileNameList.begin();
	map<string, string>::iterator nameend = rgnEffectFileNameList.end();
	for ( namebeg; namebeg != nameend; ++namebeg )
	{
		string strEffectName	 = namebeg->first;
		string strEffectFileName = namebeg->second;

		int nLength = strEffectFileName.length();
// 		fwrite(&nLength,				  sizeof(nLength), 1, fp);
// 		fwrite(strEffectFileName.c_str(), nLength,		   1, fp);

		nLength = strEffectName.length();
		fwrite(&nLength,				  sizeof(nLength), 1, fp);
		fwrite(strEffectName.c_str(),     nLength,		   1, fp);

		rgnEffectNameIndexList[strEffectName] = nIndex++;
	}
}