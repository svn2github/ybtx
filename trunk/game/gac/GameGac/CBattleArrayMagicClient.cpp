#include "stdafx.h"
#include "CBattleArrayMagicClient.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCypherVariant.h"
#include "CoreCommon.h"
#include "FightDef.h"
#include "CCoreObjectFollower.h"
#include "CSyncVariantClientHolder.h"
#include "FXPlayer.h"
#include "CCharacterFollower.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBattleArrayMagicCfgClient::MapBattleArrayMagicCfgClientById CBattleArrayMagicCfgClient::ms_mapBattleArrayMagicById;

bool CBattleArrayMagicCfgClient::LoadConfig(const string& szFileName)
{
	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapBattleArrayMagicById);
	for( int i = 1; i < TabFile.GetHeight(); ++i )
	{
		CBattleArrayMagicCfgClient* pBattleArrayMagic = new CBattleArrayMagicCfgClient;
		pBattleArrayMagic->m_uId			= i;
		pBattleArrayMagic->m_sName			= TabFile.GetString(i, szMagic_Name);
		trimend(pBattleArrayMagic->m_sName);

		pBattleArrayMagic->m_uMaxAddUpValue = TabFile.GetInteger(i, szMagic_BattleArrayValue, 1000);
		pBattleArrayMagic->m_strPosFx		= TabFile.GetString(i, szMagic_BattleArrayPosFx);
		trimend(pBattleArrayMagic->m_strPosFx);
		pBattleArrayMagic->m_strPosSucFx	= TabFile.GetString(i, szMagic_BattleArrayPosSucFx);
		trimend(pBattleArrayMagic->m_strPosSucFx);
		pBattleArrayMagic->m_strAddUpSucFx	= TabFile.GetString(i, szMagic_BattleArrayAddUpSucFx);	
		trimend(pBattleArrayMagic->m_strAddUpSucFx);
		
		CBattleArrayMagicCfgClientSharedPtr* pBattleArrayMagicSharedPtr = new CBattleArrayMagicCfgClientSharedPtr(pBattleArrayMagic);
		ms_mapBattleArrayMagicById.insert(make_pair(pBattleArrayMagic->m_uId, pBattleArrayMagicSharedPtr));
	}
	return true;
}

void CBattleArrayMagicCfgClient::UnloadConfig()
{
	ClearMap(ms_mapBattleArrayMagicById);
}

CBattleArrayMagicCfgClientSharedPtr& CBattleArrayMagicCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CBattleArrayMagicCfgClient::MapBattleArrayMagicCfgClientById::iterator mapCfgItr;
	mapCfgItr = ms_mapBattleArrayMagicById.find(uId);

	if (mapCfgItr == ms_mapBattleArrayMagicById.end())
	{
		ExpStr << "阵法魔法" << uId << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

void CBattleArrayMagicClient::Transfer(const string& strPos,const string& strEntityID,VecBattleArray& vecBattleArray)
{
	vector<string> vecstrPos,vecstrEntityID;
	string split=",";
	sqr::split(strPos,split,vecstrPos);
	sqr::split(strEntityID,split,vecstrEntityID);
	if (vecstrPos.size()!=vecstrEntityID.size())
	{
		cout << "阵法位置数据与EntityID数据个数不匹配\n";
		return;
	}
	vector<string>::iterator it_pos=vecstrPos.begin();
	vector<string>::iterator it_ID=vecstrEntityID.begin();
	for (;it_pos!=vecstrPos.end()&&it_ID!=vecstrEntityID.end();++it_pos,++it_ID)
	{
		vector<string> vecsplitstrPos;
		sqr::split_fix_size(*it_pos,1,vecsplitstrPos);
		CPos pos;
		if (vecsplitstrPos.size()==1)
		{
			pos.x=0;
			pos.y=sqr::integer(vecsplitstrPos[0]);
		}
		else if (vecsplitstrPos.size()==2)
		{
			pos.x=sqr::integer(vecsplitstrPos[0]);
			pos.y=sqr::integer(vecsplitstrPos[1]);
		}
		vecBattleArray.push_back(make_pair(sqr::integer(*it_ID),pos));
	}
}

void CBattleArrayMagicClient::PositionTransfer(VecBattleArray& vecBattleArray,const CPos& pos)
{
	CPos PixelPos;
	PixelPos.x=(pos.x-5)*eGridSpanForObj;
	PixelPos.y=(pos.y-5)*eGridSpanForObj;
	VecBattleArray::iterator it= vecBattleArray.begin();
	for (;it!=vecBattleArray.end();++it)
	{
		(*it).second.x=((*it).second.x)*eGridSpanForObj+PixelPos.x;
		(*it).second.y=PixelPos.y - ((*it).second.y)*eGridSpanForObj;
	}
}

void CBattleArrayMagicClient::OnValueChanged(CCypherVariant *pVariant)
{
	if ("ArrayState"!=pVariant->GetName())
	{
		return;
	}
	const CSyncVariantClientHolder* pViewVariant = m_pCoreObj->GetViewVariantHolder(eCTI_Static);
	EBattleArrayState eArrayState = (EBattleArrayState)pVariant->GetNumber<uint8>();
	uint32 uID = pViewVariant->GetNumber<uint8>("Attribute/ArrayStationID");
	uint32 uCategoryID = pViewVariant->GetNumber<uint32>("Attribute/CategoryID");
	uint32 uSrcEntityID = pViewVariant->GetNumber<uint32>("Attribute/SrcEntityID");
	const CBattleArrayMagicCfgClientSharedPtr& pCfg = CBattleArrayMagicCfgClient::GetById(uCategoryID);
	switch (eArrayState)
	{
	case eBAS_InArray:
		{
			string strPosSucFx=pCfg->GetPosSucFx();
			strPosSucFx=sqr::erase(strPosSucFx,"(");
			strPosSucFx=sqr::erase(strPosSucFx,")");
			vector<string> sFXTable = CBaseFX::Split(strPosSucFx, ","); 
			PlayMagicFx(sFXTable[(uID-1)*2]+","+sFXTable[(uID-1)*2+1]);
			break;
		}
	case eBAS_OutArray:
	case eBAS_ReturnArray:
		{
			string strPosFx=pCfg->GetPosFx();
			strPosFx=sqr::erase(strPosFx,"(");
			strPosFx=sqr::erase(strPosFx,")");
			vector<string> sFXTable = CBaseFX::Split(strPosFx, ","); 
			PlayMagicFx(sFXTable[(uID-1)*2]+","+sFXTable[(uID-1)*2+1]);
			break;
		}
	case eBAS_FinishArray:
		{
			CCharacterFollower* pOwner = CCharacterFollower::GetCharacterByID(uSrcEntityID);

			vector<string> sFXTable = CBaseFX::Split(pCfg->GetAddUpSucFx(), ","); 
			CPlayerFX::PlayOnceFX(pOwner,sFXTable[0]+sFXTable[1]+".efx",sFXTable[1]);
		}
		break;
	default:
		break;
	}
}
