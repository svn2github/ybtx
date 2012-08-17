#include "stdafx.h"
#include "CEditEffectManager.h"
#include "CParticleSystem.h"
#include "CSprite.h"
#include "CPolyTrail.h"
#include "CCameraShake.h"
#include "CFlareSprite.h"
#include "CCameraFilter.h"
#include "CModelConsole.h"
#include "CDynamicLight.h"
#include "CNull.h"
#include "CTDimAudio.h"
#include "CModelRenderStyle.h"
#include "CLightning.h"
#include "CLeading.h"
#include "CShake.h"
#include "BaseHelper.h"
#include "Resource.h"
#include "CEditCameraFilter.h"
#include "CEditCameraShake.h"
#include "CEditDynamicLight.h"
#include "CEditFlareSprite.h"
#include "CEditLeading.h"
#include "CEditLighting.h"
#include "CEditModelConsole.h"
#include "CEditModelRenderStyle.h"
#include "CEditNull.h"
#include "CEditParticleSystem.h"
#include "CEditPolyTrail.h"
#include "CEditShake.h"
#include "CEditSprite.h"
#include "CEditTDimAudio.h"
#include "CEditCombination.h"
#include "StringHelper.h"
#include "CEffectGroup.h"
#include "CEditEffectGroup.h"
#include "CEffectCoder.h"
#include "CEditEffectProp.h"

CEditEffectManager*	CEditEffectManager::GetEditInst()
{
	return static_cast<CEditEffectManager*>(GetInst());
}

CEditEffectManager::CEditEffectManager( ) 
	: CEffectManager(), m_bLoadRes(true),m_pEffectGroup(NULL)
{
	CTDimAudio::s_bIsEffectEditor = true;
}

CEditEffectManager::~CEditEffectManager(void)
{
	ClearAll();
}

int CEditEffectManager::GetEffectGUIDNum()
{
	return m_EffectClassDesc.size();
}

const GUID CEditEffectManager::GetEffectGUID( int Num )
{
	if( Num < GetEffectGUIDNum() )
	{
		EffectClassDescMap_t::iterator it = m_EffectClassDesc.begin();
		for( int i = 0 ; i < Num; i++ )
			it++;
		return it->first;
	}
	return GUID();
}

const char* CEditEffectManager::GetDLLName( const GUID Guid )
{
	return NULL;
}

const char* CEditEffectManager::GetShowString( const GUID Guid, DWORD Time )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it != m_EffectClassDesc.end() )
		return it->second.m_EffectDesc->GetShowedInfo( Time );
	return NULL;
}

int CEditEffectManager::GetEffectUnitNum( const GUID Guid )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it != m_EffectClassDesc.end() )
		return it->second.m_EffectDesc->GetEffectUnitNum();
	return 0;
}

string CEditEffectManager::GetEffectUnitName( const GUID Guid, int Num )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it != m_EffectClassDesc.end() )
		return it->second.m_EffectDesc->GetEffectUnitName( Num );
	return NULL;
}

CEditUnitPropHelp* CEditEffectManager::CreateEffectUnitPropHelp( const GUID Guid, int Num )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it == m_EffectClassDesc.end() )
		return NULL;
	EBaseEffectUnit eBaseUnit = (EBaseEffectUnit)Num;
	CEditUnitPropHelp* pEditProp = NULL;
	switch(eBaseUnit)
	{
	case eParticleSystem:
		pEditProp = new CEditParticleSystemProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eSprite:
		pEditProp = new CEditSpriteProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case ePolyTrail:
		pEditProp = new CEditPolyTrailProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
	    break;
	case eCameraShake:
		pEditProp = new CEditCameraShakeProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
	    break;
	case eFlareSprite:
		pEditProp = new CEditFlareSpriteProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eCameraFilter:
		pEditProp = new CEditCameraFilterProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eModelConsole:
		pEditProp = new CEditModelConsoleProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
	    break;
	case eDynamicLight:
		pEditProp = new CEditDynamicLightProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
	    break;
	case eNull:
		pEditProp = new CEditNullProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eTDimAudio:
		pEditProp = new CEditTDimAudioProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eModelRenderStyle:
		pEditProp = new CEditModelRenderStyleProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eLightning:
		pEditProp = new CEditLightningProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eLeading:
		pEditProp = new CEditLeadingProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eShake:
		pEditProp = new CEditShakeProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eEfxCombination:
		pEditProp = new CEditCombinationProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	}
	return pEditProp;
}

void CEditEffectManager::DestroyEffectUnit( IEffectUnit* pEffectUnit )
{
	SafeRelease( pEffectUnit );
}

void CEditEffectManager::DestroyEffectUnitProp( IEffectUnitProp* pEffectUnit )
{
	SafeRelease( pEffectUnit );
}

void CEditEffectManager::ClearAll()
{
	//for( map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.begin(); 
	//	itGroup != m_pEffects.end(); itGroup++ )
	//{
	//	for( map< string, CEditEffect* >::iterator itEffect = itGroup->second.begin();
	//		itEffect != itGroup->second.end(); itEffect++ )
	//	{
	//		SafeRelease( itEffect->second );
	//	}
	//}
	//m_pEffects.clear();

	SafeRelease(m_pEffectGroup);
	ClearEffectDesc();
	ClearRecycle();
	m_pEffectGroups.clear();
	m_Recycle.clear();
	m_pEffectGroup = NULL;
	m_GroupPropsTree.clear();
}

void CEditEffectManager::GetEffectName( map< string, vector< string > >& EffectName )
{
	if (!GetEditEffectGroup())
		return;
	EMap< EString, CEffectProp* >::iterator itGroup = GetEditEffectGroup()->GetEffectPropMap().begin();
	while (itGroup!=GetEditEffectGroup()->GetEffectPropMap().end())
	{
		EString strFullName = itGroup->first;
		int pos = strFullName.rfind('\\');
		string groupName = strFullName.substr(0,pos).c_str();
		string effectPropName = strFullName.substr(pos+1,strFullName.length()-pos-1).c_str();
		m_GroupPropsTree[groupName].push_back(effectPropName);
		EffectName[groupName].push_back(effectPropName);
		itGroup++;
	}
}

BOOL CEditEffectManager::CreateEffectGroup( string GroupName )
{
	if( GroupName.empty() || m_GroupPropsTree.find( GroupName.c_str() ) != m_GroupPropsTree.end() )
		return FALSE;
	m_GroupPropsTree[GroupName] = vector<string>();
	if (!m_pEffectGroup)
	{
		m_pEffectGroup = new CEditEffectGroup(this,"");
		m_pEffectGroup->AddRef();
	}
	return TRUE;
}

CEditEffectProp* CEditEffectManager::CreateEffect( string GroupName, string Name )
{
	if( GroupName.empty() || Name.empty() || m_GroupPropsTree.find( GroupName.c_str() ) == m_GroupPropsTree.end() )
		return NULL;
	//if( m_pEffects[GroupName].find( Name ) != m_pEffects[GroupName].end() )
	//	return NULL;
	if(!GetEditEffectGroup()->CreateEffectProp(GroupName,Name))
		return NULL;

	string strEffect = GroupName + "\\" + Name;
	//new CEditEffect(this,GetEditEffectGroup()->GetEffectProp(strEffect.c_str()));
	return (CEditEffectProp*)GetEditEffectGroup()->GetEffectProp(strEffect.c_str());
	//m_pEffects[GroupName][Name]->InitEffect(GetEditEffectGroup()->GetEffectProp(strEffect.c_str()));
	//return m_pEffects[GroupName][Name];
}

HRESULT CEditEffectManager::CreateEffect( IEffectGroup* pEffectGroup, const char* szEffectName, IEffect** pEffect )
{
	string effectName = szEffectName;
	slash_to_backslash(effectName);

	CEffectProp* pEffectProp = ((CEffectGroup*)pEffectGroup)->GetEffectProp( effectName.c_str() );
	if( !pEffectProp )
		return GERROR;
	SQR_TRY	
	{
		*pEffect = new CEditEffect( this, pEffectProp );
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("特效创建失败,出现难以分配问题,可能由于美术引起:");
		exp.AppendMsg(szEffectName);
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	return 0;
}

CEditEffectProp* CEditEffectManager::GetEditEffectProp( string GroupName, string Name )
{
	if( GroupName.empty() || Name.empty() || m_GroupPropsTree.find( GroupName.c_str() ) == m_GroupPropsTree.end() )
		return NULL;

	string strEffect = GroupName + "\\" + Name;
	
	if(GetEditEffectGroup()->GetEffectPropMap().find(strEffect.c_str()) == GetEditEffectGroup()->GetEffectPropMap().end())
		return NULL;
	return (CEditEffectProp*)GetEditEffectGroup()->GetEffectPropMap()[strEffect.c_str()];
	//if( m_pEffects[GroupName].find( Name ) == m_pEffects[GroupName].end() )
	//	return NULL;
	//return m_pEffects[GroupName][Name];
}

BOOL CEditEffectManager::ReNameGroup( string Old, string New )
{
	if( Old == New )
		return FALSE;

	map<string,vector<string>>::iterator iter = m_GroupPropsTree.find(Old.c_str());
	if(iter == m_GroupPropsTree.end())
		return FALSE;
	m_GroupPropsTree[New] = iter->second;
	m_GroupPropsTree.erase(iter);

	//map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.find( Old );
	//if( itGroup == m_pEffects.end() )
	//	return FALSE;
	//m_pEffects[ New ] = itGroup->second;
	//m_pEffects.erase( itGroup );

	CEditEffectGroup* pGroup =GetEditEffectGroup();
	pGroup->ReNameGroup(Old,New);
	return TRUE;
}

BOOL CEditEffectManager::ReNameEffect( string GroupName, string Old, string New )
{
	if( Old == New )
		return FALSE;
	map<string,vector<string>>::iterator itGroup = m_GroupPropsTree.find( GroupName.c_str() );
	if( itGroup == m_GroupPropsTree.end() )
		return FALSE;

	CEditEffectGroup* pGroup = GetEditEffectGroup();
	if( pGroup == NULL )
		return FALSE;

	EMap< EString, CEffectProp* > ff = pGroup->GetEffectPropMap();
	EMap< EString, CEffectProp* >::iterator itProp = pGroup->GetEffectPropMap().begin();
	while ( itProp != pGroup->GetEffectPropMap().end() )
	{
		EString strFullName = itProp->first;
		int pos = strFullName.rfind('\\');
		string groupName = strFullName.substr(0,pos).c_str();
		string effectPropName = strFullName.substr(pos+1,strFullName.length()-pos-1).c_str();
		if( effectPropName == Old && groupName == GroupName )
		{
			string newFullName = groupName + "\\" + New;
			pGroup->GetEffectPropMap()[newFullName.c_str()] = itProp->second;
			pGroup->GetEffectPropMap().erase( itProp );
			return TRUE;
		}
		itProp++;
	}

	return FALSE;
}

BOOL CEditEffectManager::DeleteGroup( string Name )
{
	//map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.find( Name );
	//if( itGroup == m_pEffects.end() )
	//	return FALSE;

	map<string,vector<string>>::iterator itGroup = m_GroupPropsTree.find( Name.c_str() );
	if(itGroup == m_GroupPropsTree.end())
		return FALSE;
	m_GroupPropsTree.erase(Name);

 	CEditEffectGroup* pGroup = GetEditEffectGroup();
//  	EMap< EString, CEffectProp* >::iterator itProp = pGroup->GetEffectPropMap().begin();
//  	while (itProp!=pGroup->GetEffectPropMap().end())
//  	{
//  		SafeRelease(itProp->second);
//  		itProp++;
//  	}
	pGroup->DeleteGroup(Name);
	//for( map< string, CEditEffect* >::iterator itEffect = itGroup->second.begin();
	//	itEffect != itGroup->second.end(); itEffect++ )
	//	SafeRelease(itEffect->second);
	//m_pEffects.erase( itGroup );
	
	//ClearEffectGroup(Name);
	return TRUE;
}

BOOL CEditEffectManager::DeleteEffect( string GroupName, string Name )
{
// 	map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.find( GroupName );
// 	if( itGroup == m_pEffects.end() )
// 		return FALSE;

	if(!m_pEffectGroup)
		return FALSE;
// 	EMap< EString, IEffectGroup* >::iterator itGroup = m_pEffectGroups.find( GroupName.c_str() );
// 	if(itGroup == m_pEffectGroups.end())
// 		return FALSE;

// 	map< string, CEditEffect* >::iterator itEffect = itGroup->second.find( Name );
// 	if( itEffect == itGroup->second.end() )
// 		return FALSE;
// 	SafeRelease(itEffect->second);
// 	itGroup->second.erase( itEffect );
	string strEffectProp = GroupName + "\\" + Name; 

	CEditEffectGroup* pGroup = (CEditEffectGroup*)GetEditEffectGroup();

	EMap< EString, CEffectProp* >::iterator itProp = pGroup->GetEffectPropMap().find(strEffectProp.c_str());
	if(itProp==pGroup->GetEffectPropMap().end())
		return FALSE;
	SafeRelease(itProp->second);
	pGroup->GetEffectPropMap().erase(itProp);

	return TRUE;
}

BOOL CEditEffectManager::MoveEffectToGroup( string Name, string SrcGroupName, string DestGroupName )
{
// 	map< string, map< string, CEditEffect* > >::iterator itDestGroup = m_pEffects.find( DestGroupName );
// 	if( itDestGroup == m_pEffects.end() )
// 		return FALSE;

	if ( m_GroupPropsTree.find( DestGroupName.c_str() ) == m_GroupPropsTree.end() )
		return FALSE;

	string strSrcEffectProp = SrcGroupName + "\\" + Name; 
	string strDesEffectProp = DestGroupName + "\\" + Name; 

	CEditEffectGroup* pGroup = (CEditEffectGroup*)GetEditEffectGroup();
	EMap< EString, CEffectProp* >::iterator itDesProp = pGroup->GetEffectPropMap().find(strDesEffectProp.c_str());
	if(itDesProp != pGroup->GetEffectPropMap().end())
		return FALSE;
	
// 	EMap< EString, IEffectGroup* >::iterator itDestGroup = m_pEffectGroups.find( DestGroupName.c_str() );
// 	if( itDestGroup == m_pEffectGroups.end())
// 		return FALSE;

// 	map< string, map< string, CEditEffect* > >::iterator itSrcGroup = m_pEffects.find( SrcGroupName );
// 	if( itSrcGroup == m_pEffects.end() )
// 		return FALSE;
	
	EMap< EString, CEffectProp* >::iterator itSrcProp = pGroup->GetEffectPropMap().find(strSrcEffectProp.c_str());
	if(itSrcProp == pGroup->GetEffectPropMap().end())
		return FALSE;

// 	EMap< EString, IEffectGroup* >::iterator itSrcGroup = m_pEffectGroups.find( SrcGroupName.c_str() );
// 	if(itSrcGroup==m_pEffectGroups.end())
// 		return FALSE;

// 	map< string, CEditEffect* >::iterator itEffect = itDestGroup->second.find( Name );
// 	if( itEffect != itDestGroup->second.end() )
// 		return FALSE;


	//itEffect = itSrcGroup->second.find( Name );
	//if( itEffect == itSrcGroup->second.end() )
	//	return FALSE;

	//itProp = pSrcGroup->GetEffectPropMap().find(Name.c_str());
	//if(itProp == pSrcGroup->GetEffectPropMap().end())
	//	return FALSE;

	//itDestGroup->second[Name] = itEffect->second;
	//itSrcGroup->second.erase( itEffect );

	pGroup->GetEffectPropMap()[strDesEffectProp.c_str()] = itSrcProp->second;
	pGroup->GetEffectPropMap().erase(itSrcProp);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CEditEffectManager::Render( uint32 uCurTime )
{
	for( EffectClassDescMap_t::iterator it = m_EffectClassDesc.begin(); it != m_EffectClassDesc.end(); ++it )
		it->second.m_EffectDesc->Render( uCurTime);
}

int CEditEffectManager::LoadEffectGroups( string LoadName )
{
	CEditEffectGroup* pEffectGroup;
	CEffectManager::CreateEffectGroup(LoadName.c_str(),(IEffectGroup**)&pEffectGroup);
	//合并到m_pEffectGroup中
	if(m_pEffectGroup==NULL)
	{
		m_pEffectGroup = (CEditEffectGroup*)CreateEffectGroupFun(LoadName.c_str());
		m_pEffectGroup->AddRef();
	}

	if(m_pEffectGroup)
		m_pEffectGroup->SetName(LoadName.c_str());

	{
		EMap< EString, CEffectProp* >& epm = ((CEditEffectGroup*)m_pEffectGroup)->GetEffectPropMap();
		EMap< EString, CEffectProp* >::iterator itProp = ((CEditEffectGroup*)pEffectGroup)->GetEffectPropMap().begin();
		while (itProp!=((CEditEffectGroup*)pEffectGroup)->GetEffectPropMap().end())
		{
			EString sEffect = itProp->first;
			EMap< EString, CEffectProp* >::iterator itEffect = epm.find(sEffect);
			if(itEffect==epm.end())
			{
				CEditEffectProp* pProp = (CEditEffectProp*)itProp->second;
				CEditEffectProp* newProp =  new CEditEffectProp((CBaseEffectGroup*)m_pEffectGroup,CEffectManager::GetInst(), pProp->GetFullEffectName());
				epm[sEffect] = newProp;

				for(int i = 0;i<pProp->GetTracksNum();i++)
				{
					EMap< int, IEffectUnitProp* > tMapEffectUnitProp;
					EMap<int,IEffectUnitProp*>::iterator iter = pProp->GetTrack(i).begin();
					while (iter!=pProp->GetTrack(i).end())
					{
						CEffectUnitProp* pUnitProp = (CEffectUnitProp*)iter->second;
						tMapEffectUnitProp[iter->first] = pUnitProp;
						pUnitProp->AddRef();
						SafeRelease(pUnitProp->m_pEffectProp);
						pUnitProp->m_pEffectProp = newProp;
						newProp->AddRef();						
						iter++;
					}
					newProp->m_Tracks.push_back(tMapEffectUnitProp);
				}
				newProp->m_AttachKey = pProp->GetAttachKeys();
			
				newProp->m_TimeRange		 = pProp->GetTimeRange();
				for(int i = 0;i<pProp->GetAttachAniNum();i++)
					newProp->m_AttachAniName.push_back(pProp->GetAttachAniName(i));
				newProp->m_UserDesc		 = pProp->GetUserDesc();
				newProp->m_bLoadRes		 = pProp->GetLoadRes();
				newProp->m_bFrameSynch    = pProp->GetFrameSynch();       
				newProp->m_bForbidScale	 = pProp->GetForbidScale();
				newProp->InitTrackInfo();
			}
			itProp++;
		}
	}

	CreateEditEffectMap(m_pEffectGroup);
	SafeRelease(pEffectGroup);
	return 0;
}

int CEditEffectManager::SaveEffectGroups( string SaveName, CEditEffectGroup* pGroup )
{
	if (!m_pEffectGroup)
		return -1;
	m_pEffectGroup->SetName(SaveName.c_str());
	if(pGroup==NULL)
		pGroup = m_pEffectGroup;

	size_t nPos = SaveName.rfind(".are");
	if( nPos != -1 )
	{
		SaveName = SaveName.substr(0, nPos);
		SaveName += ".efx";
	}

	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境

	SetFileAttributes( SaveName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	CEditEffectWriteFile File( SaveName.data(), ios::binary|ios::out );

	DWORD Res = MAKEFOURCC( 'E', 'F', 'X', 0 );
	DWORD Version = ARE_VERSION;
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );


	EMap< EString, CEffectProp* >& epm = pGroup->GetEffectPropMap();
	EMap< EString, CEffectProp* >::iterator itProp = pGroup->GetEffectPropMap().begin();
	map< string,map<string,CEffectProp*>> tEffectPropMap;
	while (itProp!=pGroup->GetEffectPropMap().end())
	{
		string skey = itProp->first.c_str();
		int pos = skey.find("\\");
		Ast(pos!=-1);
		string GroupName = skey.substr(0,pos);
		string EffectName = skey.substr(pos+1,skey.size());
		tEffectPropMap[GroupName][EffectName] = itProp->second;
		itProp++;
	}

	int Size = 0;
	int GroupSize = tEffectPropMap.size();
	File.write( (TCHAR*)&GroupSize, sizeof(int) );

	for(map<string,map<string,CEffectProp*>>::iterator it = tEffectPropMap.begin();it!=tEffectPropMap.end();it++)
	{
		Size = it->first.size();
		File.write( (TCHAR*)&Size, sizeof(int) );
		File.write( (TCHAR*)it->first.c_str(), Size );

		int EffectSize = it->second.size();
		File.write( (TCHAR*)&EffectSize, sizeof(int) );
		string strEffectName = "";
		for( map< string, CEffectProp* >::iterator itEffectProp = it->second.begin();
			itEffectProp != it->second.end(); itEffectProp++ )
		{
			strEffectName = itEffectProp->first;
			Size = strEffectName.size();
			
			File.write( (TCHAR*)&Size, sizeof(int) );
			File.write( (TCHAR*)strEffectName.c_str(), Size );
			CEditEffectProp* pProp = (CEditEffectProp*)itEffectProp->second;
			if( FAILED( pProp->SaveBin( File, strEffectName ) ) )
				return GERROR;
		}
	}

	File.close();
	tEffectPropMap.clear();

	locale::global(loc);//恢复全局locale	

	return 0;
}

int CEditEffectManager::SaveEffectGroupsByChunk( string SaveName )
{
	CEffectFormatMgr formatMgr;
	//formatMgr.Save(SaveName,m_pEffectGroup);
	return 0;
}

///保存拆分各个具体的effect
void CEditEffectManager::SplitEffectPackSave( const string& SaveName,CEditEffectGroup* pEditEffectGroup )
{
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境

	//判断第一级文件夹是否存在
	string firstfodername = "", secondfodername = "", rootpath = "", groupname = "", effectname = "", saveeffectname = "";
	WIN32_FIND_DATA FindData;
	HANDLE hFind; 
	size_t npos = 0;
	int GroupSize = 1, EffectSize = 1, Size = 0;

	string filename = SaveName;

	replace(filename, "/", "\\");

 	npos = filename.rfind("\\");
 	if ( npos != -1 )
	{
 		firstfodername = filename.substr(npos+1, filename.length());
		rootpath = filename.substr(0, npos);
		rootpath += "\\";
	}
 
 	npos = firstfodername.rfind(".");
 	if ( npos != -1 )
 		firstfodername = firstfodername.substr(0, npos);
 
 	firstfodername = rootpath + firstfodername;
 
	hFind = FindFirstFile((firstfodername.c_str()),&FindData);
 
 	if (hFind == INVALID_HANDLE_VALUE)
 	{
 		string log;
 		BOOL bCreat = FALSE;
 
 		bCreat = ::CreateDirectory(firstfodername.c_str(), NULL);
 		if( bCreat == FALSE )
 		{
 			log = firstfodername + "创建文件夹失败";
 			MessageBox(NULL,log.c_str(),"",MB_OK);
 			return;
 		}
 	}

	CEditEffectGroup* pGroup = (CEditEffectGroup*)pEditEffectGroup;
	EMap< EString, CEffectProp* >::iterator itProp = pGroup->GetEffectPropMap().begin();
	map< string,map<string,CEffectProp*>> tEffectPropMap;
	while (itProp!=pGroup->GetEffectPropMap().end())
	{
		string skey = itProp->first.c_str();
		int pos = skey.find("\\");
		Ast(pos!=-1);
		string GroupName = skey.substr(0,pos);
		string EffectName = skey.substr(pos+1,skey.size());
		tEffectPropMap[GroupName][EffectName] = itProp->second;
		itProp++;
	}

	for( map< string, map< string, CEffectProp* > >::iterator itGroup = tEffectPropMap.begin(); 
		itGroup != tEffectPropMap.end(); ++itGroup )
	{
		groupname = itGroup->first;

		//判断第二级文件夹是否存在
		secondfodername = firstfodername + "\\" + groupname;

		hFind = FindFirstFile((secondfodername.c_str()),&FindData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			string log;
			BOOL bCreat = FALSE;

			bCreat = ::CreateDirectory(secondfodername.c_str(), NULL);
			if( bCreat == FALSE )
			{
				log = secondfodername + "创建文件夹失败";
				MessageBox(NULL,log.c_str(),"",MB_OK);
				return;
			}
		}

		for( map< string, CEffectProp* >::iterator itEffect = itGroup->second.begin();
			itEffect != itGroup->second.end(); ++itEffect )
		{
			effectname = itEffect->first;
			saveeffectname = secondfodername + "\\" + effectname + ".efx";

			hFind = FindFirstFile((saveeffectname.c_str()),&FindData);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				DWORD attr = GetFileAttributes(saveeffectname.c_str());
				if(attr&FILE_ATTRIBUTE_READONLY)
				{
					string log;
					log += saveeffectname;
					log +="\n";	
					log +="文件是只读的,只有获取修改权限才可以保存!!";
					MessageBox( 0, log.c_str(), "提示", 0 );
					continue;
				}
			}

			//save
			{
				CEditEffectWriteFile File( saveeffectname.data(), ios::binary|ios::out );
				if( File.fail() )
				{
					MessageBox( 0, "找不到文件", "error", 0 );
					return ;
				}

				DWORD Res = MAKEFOURCC( 'E', 'F', 'X', 0 );
				DWORD Version = ARE_VERSION;

				File.write( (TCHAR*)&Res, sizeof(DWORD) );
				File.write( (TCHAR*)&Version, sizeof(DWORD) );
				File.write( (TCHAR*)&GroupSize, sizeof(int) );


				Size = groupname.size();
				File.write( (TCHAR*)&Size, sizeof(int) );
				File.write( (TCHAR*)groupname.c_str(), Size );
				File.write( (TCHAR*)&EffectSize, sizeof(int) );

				Size = effectname.size();
				File.write( (TCHAR*)&Size, sizeof(int) );
				File.write( (TCHAR*)itEffect->first.c_str(), Size );

				CEditEffectProp* pProp = (CEditEffectProp*)itEffect->second;
				if( FAILED( pProp->SaveBin( File, effectname ) ) )
				{
					string log = saveeffectname + "保存失败";
					MessageBox(NULL,log.c_str(),"",MB_OK);
				}

				File.close();
			}
		}
	}
	tEffectPropMap.clear();

	locale::global(loc);//恢复全局locale
}

void	CEditEffectManager::SetFramSynch(bool bResult)
{
	//for( map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.begin(); 
	//	itGroup != m_pEffects.end(); ++itGroup )
	//{
	//	for( map< string, CEditEffect* >::iterator itEffect = itGroup->second.begin();
	//		itEffect != itGroup->second.end(); ++itEffect )
	//		itEffect->second->GetEditEffectProp()->SetFrameSynch(bResult);
	//}
}

void	CEditEffectManager::SetFramSynch(string strGroup,string strEffect,bool bResult)
{
	//for( map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.begin(); 
	//	itGroup != m_pEffects.end(); ++itGroup )
	//{
	//	if(itGroup->first == strGroup)
	//	{
	//		for( map< string, CEditEffect* >::iterator itEffect = itGroup->second.begin();
	//			itEffect != itGroup->second.end(); ++itEffect )
	//		{
	//			if (itEffect->first == strEffect)
	//			{
	//				itEffect->second->GetEditEffectProp()->SetFrameSynch(bResult);
	//			}
	//		}
	//	}
	//}
}

void	CEditEffectManager::SetFramSynch(string strEffect,bool bResult)
{
	//for( map< string, map< string, CEditEffect* > >::iterator itGroup = m_pEffects.begin(); 
	//	itGroup != m_pEffects.end(); ++itGroup )
	//{
	//	for( map< string, CEditEffect* >::iterator itEffect = itGroup->second.begin();
	//		itEffect != itGroup->second.end(); ++itEffect )
	//	{
	//		if (itEffect->first == strEffect)
	//		{
	//			itEffect->second->GetEditEffectProp()->SetFrameSynch(bResult);
	//		}
	//	}
	//}
}

IEffectUnitProp* CEditEffectManager::CreateEffectUnitProp( const GUID Guid, int Num )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it == m_EffectClassDesc.end() )
		return NULL;
	EBaseEffectUnit eBaseUnit = (EBaseEffectUnit)Num;
	IEffectUnitProp* pEditProp = NULL;
	switch(eBaseUnit)
	{
	case eParticleSystem:
		pEditProp = new CEditParticleSystemProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eSprite:
		pEditProp = new CEditSpriteProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case ePolyTrail:
		pEditProp = new CEditPolyTrailProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eCameraShake:
		pEditProp = new CEditCameraShakeProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eFlareSprite:
		pEditProp = new CEditFlareSpriteProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eCameraFilter:
		pEditProp = new CEditCameraFilterProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eModelConsole:
		pEditProp = new CEditModelConsoleProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eDynamicLight:
		pEditProp = new CEditDynamicLightProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eNull:
		pEditProp = new CEditNullProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eTDimAudio:
		pEditProp = new CEditTDimAudioProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eModelRenderStyle:
		pEditProp = new CEditModelRenderStyleProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eLightning:
		pEditProp = new CEditLightningProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eLeading:
		pEditProp = new CEditLeadingProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eShake:
		pEditProp = new CEditShakeProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	case eEfxCombination:
		pEditProp = new CEditCombinationProp(Num,m_EffectClassDesc[Guid].m_EffectDesc);
		break;
	}
	return pEditProp;
}

void CEditEffectManager::ResetClassDesc()
{
	IEffectClassDesc* pEffectDesc = CreateEffectClassDesc();
	const GUID guid = pEffectDesc->GetClassID();
	if( m_EffectClassDesc.find( guid ) == m_EffectClassDesc.end() )
		m_EffectClassDesc[guid].m_EffectDesc = pEffectDesc;
}

CEditUnitPropHelp* CEditEffectManager::EffectUnitPropCast_ToPropHelp(IEffectUnitProp* pProp)
{
	if (!pProp)
		return NULL;
	EBaseEffectUnit eBaseUnit = (EBaseEffectUnit)pProp->GetID();
	CEditUnitPropHelp* pEditProp = NULL;
	switch(eBaseUnit)
	{
	case eParticleSystem:
		pEditProp = dynamic_cast<CEditParticleSystemProp*>(pProp);
		break;
	case eSprite:
		pEditProp = dynamic_cast<CEditSpriteProp*>(pProp);
		break;
	case ePolyTrail:
		pEditProp = dynamic_cast<CEditPolyTrailProp*>(pProp);
		break;
	case eCameraShake:
		pEditProp = dynamic_cast<CEditCameraShakeProp*>(pProp);
		break;
	case eFlareSprite:
		pEditProp = dynamic_cast<CEditFlareSpriteProp*>(pProp);
		break;
	case eCameraFilter:
		pEditProp = dynamic_cast<CEditCameraFilterProp*>(pProp);
		break;
	case eModelConsole:
		pEditProp = dynamic_cast<CEditModelConsoleProp*>(pProp);
		break;
	case eDynamicLight:
		pEditProp = dynamic_cast<CEditDynamicLightProp*>(pProp);
		break;
	case eNull:
		pEditProp = dynamic_cast<CEditNullProp*>(pProp);
		break;
	case eTDimAudio:
		pEditProp = dynamic_cast<CEditTDimAudioProp*>(pProp);
		break;
	case eModelRenderStyle:
		pEditProp = dynamic_cast<CEditModelRenderStyleProp*>(pProp);
		break;
	case eLightning:
		pEditProp = dynamic_cast<CEditLightningProp*>(pProp);
		break;
	case eLeading:
		pEditProp = dynamic_cast<CEditLeadingProp*>(pProp);
		break;
	case eShake:
		pEditProp = dynamic_cast<CEditShakeProp*>(pProp);
		break;
	case eEfxCombination:
		pEditProp = dynamic_cast<CEditCombinationProp*>(pProp);
		break;
	}
	return pEditProp;
}

void	CEditEffectManager::CreateEditEffectMap(IEffectGroup* pEffectGroup)
{
	if (!pEffectGroup)
		return;
	CEditEffectGroup* pGroup = (CEditEffectGroup*)pEffectGroup;
	pGroup->InitEffectProps();
}

IEffectGroup* CEditEffectManager::CreateEffectGroupFun(const char* szFileName)
{
	return new CEditEffectGroup( this, szFileName );
}

CEditEffectGroup*	CEditEffectManager::GetEditEffectGroup()
{
	if (m_pEffectGroup)
	{
		return (CEditEffectGroup*)m_pEffectGroup;
	}
	else
		return NULL;
}

void	CEditEffectManager::	ClearEffectGroup(string strName)
{
	//EString name = strName.c_str();
	//EMap<EString,IEffectGroup*>::iterator it = m_pEffectGroups.begin();
	//if (it != m_pEffectGroups.end())
	//{
	//	CEditEffectGroup* pEGroup = static_cast<CEditEffectGroup*>(it->second);
	//	pEGroup->DeleteGroup(strName);
	//}
}

void	CEditEffectManager::ChangeGroupName(string strNew)
{
// 	EString strOld = m_pEffectGroups.begin()->first;
// 	m_pEffectGroups[strNew.c_str()] = m_pEffectGroups.begin()->second;
// 	EMap<EString,IEffectGroup*>::iterator it = m_pEffectGroups.find(strOld);
// 	m_pEffectGroups.erase(it);

	CEditEffectGroup* pGroup = (CEditEffectGroup*)GetEditEffectGroup();
	EString strOld = pGroup->GetEffectPropMap().begin()->first;
	pGroup->GetEffectPropMap()[strNew.c_str()] = pGroup->GetEffectPropMap().begin()->second;
	EMap<EString,CEffectProp*>::iterator itProp = pGroup->GetEffectPropMap().find(strOld);
	pGroup->GetEffectPropMap().erase(itProp);
}
