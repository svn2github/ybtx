#include "StdAfx.h"
#include "MapEffectCtrl/MapEffectCtrlImp.h"
#include "PathManager/CPathManager.h"
#include "CEffectGroup.h"
#include "CPkgDirectory.h"
#include "CEditContext.h"

DEF_UI_IMP(CMapEffectCtrlImp);

const wstring PATH_ALIAS_EFFECT = L"res";
const string  localEffectPath   = "fx/setting/mapfx_local_yb/";
const string  ambientEffectPath = "fx/setting/mapfx_ambient_yb/";

CMapEffectCtrlImp::CMapEffectCtrlImp( void* param )
{
	m_IEffectGroupForSceneLight = NULL;
}

CMapEffectCtrlImp::~CMapEffectCtrlImp( void )
{
	m_IEffectGroupForSceneLight = NULL;

	map<string, void *>::iterator beg = m_LocalEffectGroupList.begin();
	map<string, void *>::iterator end = m_LocalEffectGroupList.end();
	for ( beg; beg != end; ++beg )
	{
		if( beg->second )
			beg->second = NULL;
	}
	m_LocalEffectGroupList.clear();
}

void CMapEffectCtrlImp::Update()
{

}

bool CMapEffectCtrlImp::_ProcMsg( const CMsg& msg )
{
	return false;
}

void CMapEffectCtrlImp::GetEffectFiles(const string& effectDir, EffectNameList& mapEffectGroupList)
{
	CPkgDir pkgDir;
	string fileName = "", groupName = "";
	int nFileNum = pkgDir.Open(PATH_ALIAS_EFFECT.c_str(), effectDir.c_str());
	for ( int m = 0; m < nFileNum; ++m )
	{
		pkgDir.GetFileName(m, fileName);
		size_t npos = fileName.rfind(".dir");
		if( npos != -1 )
		{
			string group = fileName.substr(0, npos);
			m_mapLocalEffectNameList[group];
		}
	}

	groupName = "";
	string strEffectGroupPath = "";
	EffectNameList::iterator beg = m_mapLocalEffectNameList.begin();
	EffectNameList::iterator end = m_mapLocalEffectNameList.end();
	for ( beg; beg != end; ++beg )
	{
		groupName		  = beg->first;
		strEffectGroupPath = effectDir + groupName;
		int nFileNum = pkgDir.Open(PATH_ALIAS_EFFECT.c_str(), strEffectGroupPath.c_str());
		for ( int m = 0; m < nFileNum; ++m )
		{
			pkgDir.GetFileName(m, fileName);
			size_t npos = fileName.rfind(".efx");
			if( npos != -1 )
			{
				fileName = fileName.substr(0, npos);
				mapEffectGroupList[groupName].push_back(fileName);
			}
		}
	}
}

void CMapEffectCtrlImp::LoadMapLocalEffect( )
{
	this->GetEffectFiles(localEffectPath, m_mapLocalEffectNameList);

	string groupname, effectname, effectpathname;
	EffectNameList::iterator iter = m_mapLocalEffectNameList.begin();
 	for (iter = m_mapLocalEffectNameList.begin(); iter != m_mapLocalEffectNameList.end(); ++iter)
 	{
 		groupname = iter->first + "/";

 		vector<string>::iterator iterFXItem = iter->second.begin();
 		for ( ; iterFXItem != iter->second.end(); ++iterFXItem )
 		{
 			effectname = *iterFXItem;
			effectname = groupname + effectname;
 			effectpathname = localEffectPath + effectname + ".efx";
 
 			void *p = NULL;
 			if( this->CreateEffectGroup(effectpathname, effectname, &p) )
			{
				m_LocalEffectGroupList[effectname] = p;
				m_mapLocalEffectFileList[effectname] = effectpathname;
			}
 		}
 	}
}

void CMapEffectCtrlImp::LoadMapAmbientEffect()
{
	this->GetEffectFiles(ambientEffectPath, m_mapAmbientEffectNameList);

	string ambientEffectPath = "\\fx\\setting\\mapfx_ambient_yb.efx";

	void * pEffectGroup = NULL;
	IEffectManager::GetInst()->CreateEffectGroup(ambientEffectPath.c_str(),(IEffectGroup **)&pEffectGroup);
	if( !pEffectGroup )
	{
		MessageBox(NULL, "fx\\mapfx_ambient_yb.efx 无法载入特效文件", "提示", MB_OK);
		return;
	}

	m_IEffectGroupForSceneLight = (IEffectGroup*)pEffectGroup;
}

bool CMapEffectCtrlImp::CreateEffectGroup( const string& strFXFileName, const string& effectname, void ** p )
{
	IEffectManager::GetInst()->CreateEffectGroup(strFXFileName.c_str(),(IEffectGroup **)p);
	if( !*p )
	{
		string log = strFXFileName;
		log = log + "无法载入特效文件";

		MessageBox(NULL, log.c_str(), "提示", MB_OK);
		return false;
	}

	return true;
}

int CMapEffectCtrlImp::GetLocalEffectGroupCount()
{
	return m_mapLocalEffectNameList.size();
}

void CMapEffectCtrlImp::GetLocalEffectGroupName( const int index, string& groupname )
{
	EffectNameList::iterator iter = m_mapLocalEffectNameList.begin();
	for ( int m = 0; m < index; ++m )
		++iter;

	if( iter != m_mapLocalEffectNameList.end() )
		groupname = iter->first;
}

int CMapEffectCtrlImp::GetLocalEffectUnitCount( const string& groupname )
{
	EffectNameList::iterator iter = m_mapLocalEffectNameList.find(groupname);
	if( iter != m_mapLocalEffectNameList.end() )
	{
		return (iter->second).size();
	}

	return 0;
}

void CMapEffectCtrlImp::GetLocalEffectUnitName( const int index, const string& groupname, string& effectname )
{
	EffectNameList::iterator iter = m_mapLocalEffectNameList.find(groupname);
	if( iter != m_mapLocalEffectNameList.end() )
	{
		effectname = (iter->second)[index];
	}
}

int CMapEffectCtrlImp::GetAmbientEffectGroupCount()
{
	return m_mapAmbientEffectNameList.size();
}

void CMapEffectCtrlImp::GetAmbientEffectGroupName( const int index, string& groupname )
{
	EffectNameList::iterator iter = m_mapAmbientEffectNameList.begin();
	for ( int m = 0; m < index; ++m )
		++iter;

	if( iter != m_mapAmbientEffectNameList.end() )
		groupname = iter->first;
}

int CMapEffectCtrlImp::GetAmbientEffectUnitCount( const string& groupname )
{
	EffectNameList::iterator iter = m_mapAmbientEffectNameList.find(groupname);
	if( iter != m_mapAmbientEffectNameList.end() )
	{
		return (iter->second).size();
	}

	return 0;
}

void CMapEffectCtrlImp::GetAmbientEffectUnitName( const int index, const string& groupname, string& effectname )
{
	EffectNameList::iterator iter = m_mapAmbientEffectNameList.find(groupname);
	if( iter != m_mapAmbientEffectNameList.end() )
	{
		effectname = (iter->second)[index];
	}
}

void CMapEffectCtrlImp::GetEffectFileName( const string& effectname, string& effectfilename )
{
	string neweffectname = effectname;
	replace(neweffectname.begin(), neweffectname.end(), '\\', '/');
	if( m_mapLocalEffectFileList.find(neweffectname) != m_mapLocalEffectFileList.end() )
		effectfilename = m_mapLocalEffectFileList[neweffectname];
}

void CMapEffectCtrlImp::SetActiveEffectName( const string& name )
{
	m_strActiveEffectName = name;

	CRenderNode* pObject = m_pOwnContext->GetActiveObj();
	if( pObject )
	{
		string effectfileName;
		this->GetEffectFileName(name, effectfileName);
		CEditModel* pEditModel = ((CEditModel*)m_pOwnContext->GetActiveObj());
		pEditModel->AddEffect(effectfileName.c_str(), name.c_str(), CEffectPlayer::PLAY_LOOP, NULL);
	}
}

std::string CMapEffectCtrlImp::GetActiveEffectName( const string& name )
{
	return m_strActiveEffectName;
}

void CMapEffectCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
}

