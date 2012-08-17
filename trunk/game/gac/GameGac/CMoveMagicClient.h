#pragma once
#include "CMagicClient.h"
#include "CDistortedTick.h"
#include "CVector3.h"
#include "CPos.h"
#include "PatternCOR.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CFighterMallocObject.h"
#include "CMoveMagicDefs.h"

DefineCfgSharedPtr(CMoveMagicCfgClient)

class CFighterFollower;

class CMoveMagicCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CMoveMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CMoveMagicCfgClientSharedPtr*> > >		MapMoveMagicCfgClientByID;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CMoveMagicCfgClientSharedPtr&		GetByID(uint32 uID);				
	const uint32		GetID()					{return m_uID;}
	const string&		GetStartAct()			{return m_strStartAct;}
	const string&		GetStartFx()			{return m_strStartFx;}
	const string&		GetProcessAct()			{return m_strProcessAct;}
	const string&		GetProcessFx()			{return m_strProcessFx;}
	const string&		GetEndAct()				{return m_strEndAct;}
	const string&		GetEndFx()				{return m_strEndFx;}
	float 				GetSpeed()				{return m_fSpeed;}
	EMoveActionType		GetActionType()			{return m_eActionType;}

private:
	static MapMoveMagicCfgClientByID			m_mapCfgByID;
	uint32				m_uID;					// 编号
	float				m_fSpeed;				// 速度
	EMoveActionType		m_eActionType;			// 动作
	string				m_strStartAct;
	string				m_strStartFx;
	string				m_strProcessAct;
	string				m_strProcessFx;
	string				m_strEndAct;
	string				m_strEndFx;
};

class CMoveMagicClient
	: public CDistortedTick
	, public CPtCORHandler
	, public CFighterMallocObject
{
public:
	CMoveMagicClient(CFighterFollower* pFighter, const CMoveMagicCfgClientSharedPtr& pCfg);
	~CMoveMagicClient();

private:
	void OnTick();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);

	CFighterFollower*		m_pFighter;
	CMoveMagicCfgClientSharedPtr		m_pCfg;

	CVector3f m_vSrcPosition;
	CVector3f m_vCurPosition;
	CVector3f m_vDstPosition;

	float m_uPassTime;
	float m_fAllTime;
	float m_fHeightSrcSpeed;
	bool m_bIsMove;
	bool m_bPlayEndAni;
};
