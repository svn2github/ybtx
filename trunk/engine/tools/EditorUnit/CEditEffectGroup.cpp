#include "stdafx.h"
#include "CEditEffectGroup.h"
#include "CEditEffectProp.h"

CEditEffectGroup::CEditEffectGroup( IEffectManager* pEffectMgr, const char* szFileName ) 
: CEffectGroup(pEffectMgr,szFileName)
{
}

int32 CEditEffectGroup::AddRef()
{
	return CRefObject::AddRef();
}

CEditEffectGroup::~CEditEffectGroup()
{
}

CEffectProp* CEditEffectGroup::CreateEffectPropFun(const char* szEffect)
{
	return new CEditEffectProp( this, m_pEffectMgr, szEffect );
}

bool CEditEffectGroup::CreateEffectProp(string GroupName, string Name)
{
	string strEffectProp = GroupName + "\\" + Name;

	EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.find(strEffectProp.c_str());
	if(iter!=m_EffectProps.end())
		return false;
	
	m_EffectProps[strEffectProp.c_str()] = CreateEffectPropFun(strEffectProp.c_str());
	((CEditEffectProp*)(m_EffectProps[strEffectProp.c_str()]))->SetTimeRange(1000);
	return true;
}

void	CEditEffectGroup::ReNameGroup(string strOld,string strNew)
{
	EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.begin();
	EMap< EString, CEffectProp* > mapTemp;
	EString strTemp = strOld.c_str();
	strTemp = strTemp + "\\";
	EString strReplace = strNew.c_str();
	strReplace = strReplace + "\\";
	for (;iter != m_EffectProps.end();)
	{
		EString str = iter->first.c_str();
		int pos = str.find(strTemp);
		if (pos != -1)
		{
			int linePos = str.find("\\");
			linePos+=1;
			str = str.substr(linePos,str.size()-linePos);
			str.insert(pos,strReplace);
			mapTemp.insert(make_pair(str,iter->second));
			m_EffectProps.erase(iter++);
			continue;
		}
		else
			iter++;
	}
	for (EMap< EString, CEffectProp* >::iterator iter = mapTemp.begin();iter != mapTemp.end();++iter)
	{
		m_EffectProps[iter->first] = iter->second;
	}
}

void	CEditEffectGroup::DeleteGroup(string strName)
{
	EString strGroup = strName.c_str();
	strGroup = strGroup + "\\";
	EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.begin();
	for (;iter != m_EffectProps.end();)
	{
		EString str = iter->first.c_str();
		int pos = str.find(strGroup);
		if (pos == 0)
		{
			iter->second->Release();
			m_EffectProps.erase(iter++);
		}
		else
			++iter;
	}
}

void CEditEffectGroup::InitEffectProps()
{
	EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.begin();
	while(iter!=m_EffectProps.end())
	{
		CEditEffectProp* pProp = (CEditEffectProp*)iter->second;
		pProp->InitTrackInfo();
		iter++;
	}
}

void CEditEffectGroup::SetName(EString szName)
{
	if(m_szName=="")
		m_szName = szName;
}

//void CEditEffectGroup::DeleteEffectProp(string GroupName, string Name)
//{
//	string strEffectProp = GroupName + "\\" + Name;
//	EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.find(strEffectProp.c_str());
//	if(iter!=m_EffectProps.end())
//	{
//		SafeDelete(iter->second);
//		m_EffectProps.erase(iter);
//	}
//}
