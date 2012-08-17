#include "stdafx.h"
#include "CModelInfoClient.h"
#include "CTxtTableFile.h"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "FXPlayer.h"
#include "CPropertyCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CModelInfoClient::MapModelInfo CModelInfoClient::ms_mapModelInfo;
CModelInfoClient::MapModelClassType			CModelInfoClient::ms_mapModelClassType;
CModelInfoClient::MapModelClassString			CModelInfoClient::ms_mapModelClassString;
string CModelInfoClient::ms_strFXPath="fx/setting/zbei/";

bool CModelInfoClient::LoadConfig(const string& szFileName)
{
	InitMapClassType();
	InitMapClassString();
	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	for( int i = 1; i < TabFile.GetHeight(); ++i )
	{	
		CModelInfoClient ModelInfo;
		ModelInfo.m_strName	= TabFile.GetString(i, "名称");
		trimend(ModelInfo.m_strName);
		ModelInfo.m_strPieceName	= TabFile.GetString(i, "部位");
		trimend(ModelInfo.m_strPieceName);
		ModelInfo.m_strHidePiece	= TabFile.GetString(i, "遮挡关系");
		trimend(ModelInfo.m_strHidePiece);
		ModelInfo.m_strModelFileName	= TabFile.GetString(i, "模型文件名");
		trimend(ModelInfo.m_strModelFileName);
		ModelInfo.m_strFXFileName	= TabFile.GetString(i, "特效文件名");
		trimend(ModelInfo.m_strFXFileName);
		ModelInfo.m_strStyleName	= TabFile.GetString(i, "样式名");
		trimend(ModelInfo.m_strStyleName);
		string strModelClass = TabFile.GetString(i, "对应职业");
		trimend(strModelClass);
		vector<string> vecModelClassTable = CBaseFX::Split(strModelClass, ",");

		vector<string>::iterator it = vecModelClassTable.begin();
		for (;it!=vecModelClassTable.end();++it)
		{
			MapModelClassType::iterator it_Map = ms_mapModelClassType.find(*it);
			EModelClassType eModelClass = it_Map->second;
			if (eModelClass<eMCT_Count)
			{
				CModelInfoClient* pExpendModelInfo = new CModelInfoClient(&ModelInfo);
				pExpendModelInfo->MakeFXFile(eModelClass);
				ms_mapModelInfo.insert(make_pair(make_pair(pExpendModelInfo->m_strName,eModelClass),pExpendModelInfo));
			}
			else if(eMCT_Male_Human == eModelClass )
			{
				uint32 eExpendClass = eMCT_Male_Warrior;
				for ( ;eExpendClass<=eMCT_Male_Priest;++eExpendClass)
				{
					CModelInfoClient* pExpendModelInfo = new CModelInfoClient(&ModelInfo);
					pExpendModelInfo->MakeFXFile((EModelClassType)eExpendClass);
					ms_mapModelInfo.insert(make_pair(make_pair(pExpendModelInfo->m_strName,(EModelClassType)eExpendClass),pExpendModelInfo));
				}
			}
			else if(eMCT_Female_Human ==eModelClass )
			{
				uint32 eExpendClass = eMCT_Female_Warrior;
				for ( ;eExpendClass<=eMCT_Female_Priest;++eExpendClass)
				{
					CModelInfoClient* pExpendModelInfo = new CModelInfoClient(&ModelInfo);
					pExpendModelInfo->MakeFXFile((EModelClassType)eExpendClass);
					ms_mapModelInfo.insert(make_pair(make_pair(pExpendModelInfo->m_strName,(EModelClassType)eExpendClass),pExpendModelInfo));
				}
			}
		}
	}
	return true;
}

void CModelInfoClient::UnloadConfig()
{
	ClearMap(ms_mapModelInfo);
}

CModelInfoClient::CModelInfoClient(const CModelInfoClient* pModelInfoClient)
{
	m_strName = pModelInfoClient->m_strName;
	m_strPieceName = pModelInfoClient->m_strPieceName;
	m_strHidePiece = pModelInfoClient->m_strHidePiece;
	m_strModelFileName = pModelInfoClient->m_strModelFileName;
	m_strFXFileName = pModelInfoClient->m_strFXFileName;
	m_strStyleName = pModelInfoClient->m_strStyleName;
}

void CModelInfoClient::MakeFXFile(EModelClassType eClassType)
{
	vector<string> vecFXFileName = CBaseFX::Split(m_strFXFileName, ",");
	if (vecFXFileName.size()==1)
	{
		string ClassString = ms_mapModelClassString[eClassType];
		string strFXFileName = vecFXFileName[0];
		m_strFXFileName = "\""+ms_strFXPath+ClassString+strFXFileName+"\"";
	}
}

CModelInfoClient* CModelInfoClient::GetModelInfo(const TCHAR* szName,EClass eClass, ECharSex eSex)
{
	string strModelClass = CPropertyCfg::GetClassNameString(eClass);
	if (eClass<=eCL_Priest)
	{
		if (eCS_Female!=eSex)
		{
			strModelClass=strModelClass+"男";
		}
		else
		{
			strModelClass=strModelClass+"女";
		}
	}
	MapModelClassType::iterator it = ms_mapModelClassType.find(strModelClass);
	if (it!=ms_mapModelClassType.end())
	{
		MapModelInfo::iterator itModelInfo = ms_mapModelInfo.find(make_pair(szName,it->second));
		if (itModelInfo!=ms_mapModelInfo.end())
		{
			return itModelInfo->second;
		}
	}
	return NULL;
}