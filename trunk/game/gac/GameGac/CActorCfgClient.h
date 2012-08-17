#pragma once
#include "ActorDef.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CActorCfgClient : public CConfigMallocObject
{
public:
	static bool LoadConfig(const string& szFileName);
	static void UnloadConfig();
	
	static CActorCfgClient* GetActorCfgByActionState(EActionState eActionState);

	const string&				GetActorName() 			{ return m_strActorName; }	
	const EActionState			GetActionState()		{ return m_eActionState; }
	const EActionPlayPriority	GetPriorityLevel()		{ return m_ePriorityLevel; }
	const EActionPlayMode		GetActorPlayMode()		{ return m_eActorPlayMode; }
	const string&				GetPlayerAniName()		{ return m_strPlayerAniName; }
	const string&				GetNPCAniName()			{ return m_strNPCAniName; }
	const EActionState			GetNPCReplaceState()	{ return m_eNPCReplaceState; }
	const bool					IsFxFollowAni()			{ return m_bFxFollowAni; }
	const string&				GetFxPartPath() 		{ return m_strFxPartPath; }

	// 用于技能表所添动作替换
	static void SetReachUpAniName(string strAniName)	{ ChangeAniName(eAS_ReachUp, strAniName); }
	static void SetChannelAniName(string strAniName)	{ ChangeAniName(eAS_Channel, strAniName); }
	static void SetSingAniName(string strAniName)		{ ChangeAniName(eAS_Sing, strAniName); }
	static void SetCastAniName(string strAniName)		{ ChangeAniName(eAS_Cast, strAniName); }

private:
	typedef map<string, EActionPlayMode, less<string>, 
		TConfigAllocator<pair<string, EActionPlayMode > > >			MapActorPlayMode;
	typedef map<string, EActionPlayPriority, less<string>, 
		TConfigAllocator<pair<string, EActionPlayPriority > > >		MapActorPlayPriority;
	typedef map<EActionState, CActorCfgClient*, less<EActionState>, 
		TConfigAllocator<pair<EActionState, CActorCfgClient* > > >	MapActorActorCfgByState;

	static MapActorPlayMode						ms_mapActorPlayMode;
	static MapActorPlayPriority					ms_mapActorPlayPriority;
	static MapActorActorCfgByState				ms_mapActorAniCfgByState;

	string					m_strActorName;				// 动作名
	EActionState			m_eActionState;				// 动作状态
	EActionPlayPriority		m_ePriorityLevel;			// 播放优先级			
	EActionPlayMode			m_eActorPlayMode;			// 播放分类
	string					m_strPlayerAniName;			// 玩家动作资源名
	string					m_strNPCAniName;			// NPC动作资源名
	EActionState			m_eNPCReplaceState;			// 用来替换缺失动作的动作状态
	bool					m_bFxFollowAni;				// 特效跟随动作
	string					m_strFxPartPath;			// 特效部分路径

private:
	static void ChangeAniName(EActionState eActionState, string strAniName);
	static void BuildActionPlayPriority();
	static void BuildActorPlayMode();
};
