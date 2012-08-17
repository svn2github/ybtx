#include "stdafx.h"
#include "CNpcChaserQueueMgr.h"
#include "CCharacterDictator.h"
#include "CDir.h"
#include "CAppServer.h"
#include "CoreCommon.h"
#include "TSqrAllocator.inl"

CNpcChaserQueueMgr::CNpcChaserQueueMgr(CCharacterDictator* pOwner)
:m_pOwner(pOwner)
{
}

CNpcChaserQueueMgr::~CNpcChaserQueueMgr()
{
	m_mapNearChase.clear();
	m_mapMiddleChase.clear();
	m_mapFarChase.clear();
}

void CNpcChaserQueueMgr::DelChaser(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if (!pCharacter)
		return;
	float fDist = pCharacter->GetNpcAIHandler()->GetAttackScope();
	MapNpcChaser2Angle& MapChaser2Angle = GetChaserMap(fDist);
	MapNpcChaser2Angle::iterator it = MapChaser2Angle.find(uEntityID);
	for (; it != MapChaser2Angle.end(); it++)
	{
		if (it->second == uEntityID)
			it->second = 0;
	}
}

void CNpcChaserQueueMgr::GetRegulateNpcPlace(uint32 uEntityID, CFPos& Pos)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if (!pCharacter)
		return;

	CFPos ownpos, targetpos;
	m_pOwner->GetPixelPos(ownpos);
	pCharacter->GetPixelPos(targetpos);

	float fBottomSize = pCharacter->GetBottomSize();
	float fDist = pCharacter->GetNpcAIHandler()->GetAttackScope();
	float fRealDist = fDist + m_pOwner->GetBottomSize() + fBottomSize; 
	uint32 uCurAngle ;

	MapNpcChaser2Angle& mapNpcChase2Angle = GetChaserMap(fDist);
	MapNpcChaser2Angle::iterator iter = mapNpcChase2Angle.begin();
	for (; iter != mapNpcChase2Angle.end(); iter++)
	{
		if (iter->second == uEntityID)
			iter->second = 0;
	}

	CDir dirTarget;
	CVector2f vecDir;
	vecDir.x = targetpos.x - ownpos.x;
	vecDir.y = targetpos.y - ownpos.y;
	dirTarget.Set(vecDir);			//当前追击者相对于自己的角度

	if (m_pOwner->GetChaserTargetNum() == 1)
	{
		uCurAngle = dirTarget.uDir;
	}
	else
	{
		bool bFindPosInChaseMap = false;
		if (mapNpcChase2Angle.size() != 0)
		{
			uint32 uAngle = 256/mapNpcChase2Angle.size();
			int32 uRainNum = dirTarget.uDir/uAngle;
			bool bDirection = (dirTarget.uDir - uRainNum*uAngle) > ((uRainNum+1)*uAngle - dirTarget.uDir) ? true : false;
			if (bDirection)		//顺时针
			{
				for (uint32 v = 0; v <= (mapNpcChase2Angle.size()-1); v++)
				{
					(uRainNum+1) < (int32)mapNpcChase2Angle.size() ? uRainNum++ : uRainNum = 0;
					uint32 uLocalCurAngle = uRainNum*uAngle;
					MapNpcChaser2Angle::iterator iter = mapNpcChase2Angle.find(uLocalCurAngle);
					Ast (iter != mapNpcChase2Angle.end());
					if (iter->second == 0)
					{
						iter->second = uEntityID;
						uCurAngle = uLocalCurAngle;
						bFindPosInChaseMap = true;
						break;
					}
				}
			}
			else
			{
				for (uint32 v = 0; v <= (mapNpcChase2Angle.size()-1); v++)
				{
					uint32 uLocalCurAngle = uRainNum*uAngle;
					MapNpcChaser2Angle::iterator iter = mapNpcChase2Angle.find(uLocalCurAngle);
					Ast (iter != mapNpcChase2Angle.end());
					if (iter->second == 0)
					{
						iter->second = uEntityID;
						uCurAngle = uLocalCurAngle;
						bFindPosInChaseMap = true;
						break;
					}
					(uRainNum-1) < 0 ? uRainNum = (int32)mapNpcChase2Angle.size()-1 : uRainNum --;
				}
			}
		}
		if (!bFindPosInChaseMap)
			uCurAngle = (uint32)CAppServer::Inst()->GetFrameTime()%256 - 1;
	}

	CDir dirCur;
	dirCur.SetDir(uCurAngle);
	CVector2f vecDirf;
	dirCur.Get(vecDirf);
	CFPos DstPos;
	DstPos.x = ownpos.x + (fRealDist-1.0f)*eGridSpanForObj * vecDirf.x;
	DstPos.y = ownpos.y + (fRealDist-1.0f)*eGridSpanForObj * vecDirf.y;
	Pos = DstPos;
}

CNpcChaserQueueMgr::MapNpcChaser2Angle& CNpcChaserQueueMgr::GetChaserMap(float fAttackDis)
{
	if (fAttackDis <= 2.0f)
	{
		if (m_mapNearChase.empty())
		{
			uint32 uChaseNumber = 8;
			uint32 uAngle = 256/uChaseNumber;
			for (uint32 i = 0; i < uChaseNumber; i++)
				m_mapNearChase[i*uAngle] = 0;
		}
		return m_mapNearChase;
	}
	else if (fAttackDis > 2.0f && fAttackDis <= 4.5f)
	{
		if (m_mapMiddleChase.empty())
		{
			uint32 uChaseNumber = 16;
			uint32 uAngle = 256/uChaseNumber;
			for (uint32 i = 0; i < uChaseNumber; i++)
				m_mapMiddleChase[i*uAngle] = 0;
		}
		return m_mapMiddleChase;
	}
	else 
	{
		if (m_mapFarChase.empty())
		{
			uint32 uChaseNumber = 32;
			uint32 uAngle = 256/uChaseNumber;
			for (uint32 i = 0; i < uChaseNumber; i++)
				m_mapFarChase[i*uAngle] = 0;
		}
		return m_mapFarChase;
	}
}

