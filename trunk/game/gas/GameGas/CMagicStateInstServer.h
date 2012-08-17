#pragma once
#include "CSkillInstServer.h"
#include "CStateDBData.h"
#include "CMagicEffDataMgrServer.h"


//typedef map<uint32, CMagicEffDataMgrServer*> MapMEffDataMgrServer;		//三维：主魔法操作（魔法效果链）的个数：map

typedef list<CMagicEffDataMgrServer>			ListEffDataMgrServer;	//四维：魔法状态的叠加次数：list

class ICharacterDictatorCallbackHandler;
class CMagicStateCfgServerSharedPtr;



class CMagicStateCascadeDataMgr
{
public:
	CMagicStateCascadeDataMgr();
	~CMagicStateCascadeDataMgr();

	//CMagicStateCascadeDataMgr(uint8 uSkillLevel, const string& sSkillName, EAttackType eAttackType, bool bIsDot)
	//	: CSkillInstServer(uSkillLevel, sSkillName, eAttackType, bIsDot)
	//{

	//}

	//bool	GetIsCalc()						{return m_bIsCalc;}
	//void	SetIsCalc(bool b)				{m_bIsCalc = b;}
	CMagicEffDataMgrServer*	GetCurEffDataMgr()const;
	CMagicEffDataMgrServer*	GetLastEffDataMgr();

	ListEffDataMgrServer::iterator& GetItr(){return m_itrCascade;}
	void	SetItr(const ListEffDataMgrServer::iterator& it) { m_itrCascade = it; }

	void	SetItrBegin()					{m_itrCascade = m_listEffDataMgr.begin();}
	void	SetItrLast()					{m_itrCascade = m_listEffDataMgr.end(); if(!m_listEffDataMgr.empty()) m_itrCascade--;}
	bool	GetItrEnd()						{return m_itrCascade == m_listEffDataMgr.end();}
	void	AssertItrEnd();
	void	DoItrStep()						{++m_itrCascade;}
	void	ClearCascade()					{m_listEffDataMgr.clear();}
	void	AddCascade(CFighterDictator* pFrom)					{m_listEffDataMgr.push_back(CMagicEffDataMgrServer(pFrom));}
	void	DecCascadeBegin()				{m_listEffDataMgr.pop_front();}
	void	DecCascadeLast()				{m_listEffDataMgr.pop_back();}
	const ListEffDataMgrServer& GetEffDataMgrList()const;

	void	CopyDataWithRatioFrom(CMagicStateCascadeDataMgr* pSrc, float fRatio);	//从pSrc拷贝数据，并且附带比率

	void	SerializeToDB(CStoredObjDBDataVec* pStoredRet, ICharacterDictatorCallbackHandler* pHandler,
		uint32 uFighterGlobalID, uint32 uInstID);

	int32 CalcAllEffData();
	//CMagicStateCfgServerSharedPtr& GetCfg();
	//EMagicStateEffType GetMagicStateEffType() {return m_eMagicStateEffType;}

	//static EMagicStateEffType GetMagicStateEffTypeByTitle(const string& sTitle);

private:
	//static bool		InitMap()
	//{
	//	m_mapMagicStateEffType["可撤销"] = eMSET_Cancellable;
	//	m_mapMagicStateEffType["间隔"] = eMSET_Dot;
	//	m_mapMagicStateEffType["最终"] = eMSET_Final;
	//	return true;
	//}
	//static bool		m_bInit;

	//static map<string, EMagicStateEffType>	m_mapMagicStateEffType;

	//CMagicStateCfgServerSharedPtr*	m_pCfg;
	//bool	m_bIsCalc;						//计算标志位
	ListEffDataMgrServer					m_listEffDataMgr;
	ListEffDataMgrServer::iterator			m_itrCascade;
	//EMagicStateEffType						m_eMagicStateEffType;
};

