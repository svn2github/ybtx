#include "stdafx.h"
#include "CCharacter.h"
#include "CCharacter.inl"
#include "CDataCache.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CRenderObject.h"

CCharacter::CCharacter()
:m_fScaling(0.0)
{

}

CDataCache::CDataCache()
{
	CGraphic::GetInst()->SetAsynSwitch(false);
}

CDataCache::~CDataCache()
{
	ModelPART_MAP::iterator beg = m_mapPlayerModelPart.begin();
	ModelPART_MAP::iterator end = m_mapPlayerModelPart.end();
	for ( beg; beg != end; ++beg )
	{
		SafeDelete(beg->second);
	}
}

void CDataCache::AddCharacter(CCharacter* pCharacter, int type)
{
	if (type == 0)//人物
	{
		if(m_mapCharacter.find(pCharacter->GetName()) != m_mapCharacter.end())
			cout<<"配置表中\""<<pCharacter->GetName()<<"\"项重复"<<endl;

		m_mapCharacter.insert(make_pair(pCharacter->GetName(),pCharacter));
		m_vecCharacterName.push_back(pCharacter->GetName());
		AdvAddRef(pCharacter);
	}
	else if(type == 1)//IntObj
	{
		if(m_mapIntObj.find(pCharacter->GetName()) != m_mapIntObj.end())
			cout<<"配置表中\""<<pCharacter->GetName()<<"\"项重复"<<endl;
		m_mapIntObj.insert(make_pair(pCharacter->GetName(),pCharacter));
		m_vecIntObjName.push_back(pCharacter->GetName());
		AdvAddRef(pCharacter);
	}
}

void CDataCache::AddNPC(const TCHAR* name, const TCHAR* AItype)
{
	string sName(name);
	string sAItype(AItype);
	if(m_mapNpcCommon.find(sName) != m_mapNpcCommon.end())
		cout<<"Npc_Common配置表中\""<<sName<<"\"项重复"<<endl;
	m_mapNpcCommon.insert(make_pair(sName,NPCData(sName,sAItype)));
}

string CDataCache::getNpcAi(const string& name)
{
	NPCDATA_MAP::iterator it = m_mapNpcCommon.find(name);
	if (it != m_mapNpcCommon.end())
	{
		return it->second.sAItype;
	}
	return "";
}

void CDataCache::ShowIntNpcName(vector<string>& characterNames)
{
	if( m_vecCharacterName.empty() )
		return;

	string name = "";
	CHARACTERNAME_VEC::iterator it = m_vecCharacterName.begin();
	for(;it != m_vecCharacterName.end();++it)
	{
		name = *it;
		name = GetNpc_ObjNameWithMod(name,eMT_Character);
		characterNames.push_back(name);
	}
}

void CDataCache::ShowIntObjName(vector<string>& objectNames)
{
	if( m_vecIntObjName.empty() )
		return;

	string name = "";
	CHARACTERNAME_VEC::iterator it = m_vecIntObjName.begin();
	for(;it != m_vecIntObjName.end();++it)
	{
		string name = *it;
		name = GetNpc_ObjNameWithMod(name,eMT_IntObj);
		objectNames.push_back(name);
	}
}

void CDataCache::DisPlayCharacter(std::string name, CRenderObject *pRenderObj ,EModelType type)
{
	SQR_TRY
	{
		CHARACTER_MAP* pMap;
		if (type == eMT_Character)
		{
			pMap = &m_mapCharacter;
		}
		else if(type == eMT_IntObj)
		{
			pMap = &m_mapIntObj;
		}

 		CGraphic::GetInst()->SetAsynSwitch(false);
		 
		CHARACTER_MAP::iterator it = pMap->find(name);
		if(it == pMap->end())
		{
			return;                                              
		}
		CCharacter* pCharacter = it->second;
		if(pCharacter->GetAniFileName() == "")
		{
			cout<<pCharacter->GetName() <<"的AniFileName为空,请查看转换是否有问题"<<endl;
			return;
		}
		MODELPIECE_MAP::iterator itor = pCharacter->m_mapModelPiece.begin();
		
		for(;itor != pCharacter->m_mapModelPiece.end();++itor)
		{
			CModelPiece* modelPiece = itor->second;
			if(modelPiece->GetModelFileName() == "")
			{
				cout<<modelPiece->GetIndexName() <<"的ModelFile为空,请查看转换是否有问题"<<endl;
				break;
			}
			m_curModelFileName = modelPiece->GetModelFileName();
			pRenderObj->AddPiece(RFT_ARP,modelPiece->GetModelFileName().c_str(),modelPiece->GetPieceName().c_str(),
								modelPiece->GetRenderStyle().c_str());
			pRenderObj->AddPiece(RFT_ARA,pCharacter->GetAniFileName().c_str(),modelPiece->GetPieceName().c_str(),0);
		}
		if(pCharacter->GetScaling() != 0.0)
		{
			pRenderObj->SetScale(pCharacter->GetScaling());
		}
		}
	SQR_CATCH(exp)
	{
		cout<< name <<": "<< exp.ErrorMsg()<<endl;
	}
	SQR_TRY_END;
}

string CDataCache::GetNpc_ObjRealName(string& strName)
{
	int lBracket = strName.find("(");
	if (lBracket == -1)
		return strName;
	string strResult = strName;
	strResult = strName.substr(0,lBracket-2);
	return strResult;
}

string CDataCache::GetNpc_ObjNameWithMod(string& strName,EModelType type)
{
	string strMod = "";
	if (type == eMT_Character)
	{
		strMod = m_mapCharacter[strName]->GetBodyModFileName();
	}
	else
	{
		strMod = m_mapIntObj[strName]->GetBodyModFileName();
	}
	string strResult = strName + "  (" + strMod + ")";
	return strResult;
}

void  CDataCache::GetAnimateNames(CRenderObject* renderObj, vector<string>& agpNames)
{
	SQR_TRY
	{
		if(renderObj==NULL)
			return;

		CAnimationGroup* pAni = renderObj->GetAnimateGroup();
		if(pAni==NULL)
			return;

		string name = "";
		int num = pAni->GetAnimationNum();
		for(int i=0;i<num;i++)
		{
			name = pAni->GetAnimationName(i);
			agpNames.push_back(name);
		}
	}
	SQR_CATCH(exp)
	{
		cout<< exp.ErrorMsg()<<endl;
	}
	SQR_TRY_END;
}

void CDataCache::AddPlayerModelPart(CPlayerModelPart* pModelPart)
{
	if(!pModelPart)
		return;

	m_vecPlayerModelPart.push_back(pModelPart);
	AdvAddRef(pModelPart);
}

CPlayerModelPart* CDataCache::GetPlayerModePart(const string& str)
{
	if(!m_mapPlayerModelPart.count(str))
	{
		m_mapPlayerModelPart[str] = new CPlayerModelPart;
		m_vecPlayerModelPart.push_back(m_mapPlayerModelPart[str]);
	}
	return m_mapPlayerModelPart[str];
}



