#include "stdafx.h"
#include "CReadCfg.h"
#include "CTxtTableFile.h"
#include "CDataCache.h"
#include "CCharacter.h"
#include "CCharacter.inl"
#include "StringHelper.h"
#include "CBaseScriptApp.h"

const char* gAlias = "cfg";
extern CDataCache*	GetCCache();

CReadCfg::CReadCfg()
{

}

void CReadCfg::InitData()
{
	ReadPropRes();
}

extern string GetCfgAlias();

void CReadCfg::ReadPropRes()
{
	CDataCache *pDataCache = GetCCache();
	if( pDataCache == NULL )
		return;

	string strAlias = GetCfgAlias() + "/";
	string strFile = strAlias + "model_res/PropRes.txt";
	CTxtTableFile table;
	table.Load(gAlias,strFile.c_str());
	PropResData data;
	int nTableHeight = table.GetHeight();
	for ( int i = 1; i < nTableHeight; ++i )
	{
		data.m_sName = table.GetString(i,"名称");
		data.m_sJob = table.GetString(i,"对应职业");
		data.m_sPart = table.GetString(i,"部位");
		data.m_sCovering = table.GetString(i,"遮挡关系");
		data.m_sModName = table.GetString(i,"模型文件名");
		data.m_sEffectFileName = table.GetString(i,"特效文件名");
		data.m_sManner = table.GetString(i,"样式名");
		replace(data.m_sCovering,"\"","");
		
		CPlayerModelPart* player = pDataCache->GetPlayerModePart(data.m_sName);
		player->SetName(data.m_sName.c_str());
		if (data.m_sJob.find("男") != -1)
		{
			player->SetEveryModelFileName("人类男",data.m_sModName.c_str());
			player->SetHumanMaleHidePiece(data.m_sCovering.c_str());
		}
		else if (data.m_sJob.find("女") != -1)
		{
			player->SetEveryModelFileName("人类女",data.m_sModName.c_str());
			player->SetHumanFeMaleHidePiece(data.m_sCovering.c_str());
		}
		else
		{
			player->SetEveryModelFileName("兽人男",data.m_sModName.c_str());
			player->SetOrcHidePiece(data.m_sCovering.c_str());
		}
		player->SetAniFileName("character/ani/zhujue/");
		player->SetEffectFileName(data.m_sEffectFileName.c_str());
		player->SetRenderStyle(data.m_sManner.c_str());
		player->SetPieceName(data.m_sPart.c_str());
	}
}

