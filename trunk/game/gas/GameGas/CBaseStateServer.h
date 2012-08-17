#pragma once
#include "CBaseStateCfgServer.h"
#include "CDistortedTick.h"


namespace sqr
{
	class CSyncVariantServer;
}
class CAllStateMgrServer;

class CStateVaraintPointerHolder
	: public CMagicStateMallocObject
{
	friend class CAllStateMgrServer;
private:
	CSyncVariantServer* m_pStateSubTree;
	CSyncVariantServer* m_pCategoryId;
	CSyncVariantServer* m_pDynamicId;
	CSyncVariantServer* m_pCount;
	CSyncVariantServer* m_pTime;
	CSyncVariantServer* m_pGuessStartTime;
	CSyncVariantServer* m_pSkillLevel;
	CSyncVariantServer* m_pStateIsSync;
	CSyncVariantServer* m_pStateIsFinish;
	CSyncVariantServer* m_pInstallerID;
	CSyncVariantServer* m_pCalcValue;
};


class CFighterDictator;
class CGenericTarget;

class CBaseStateServer
	: public CDistortedTick
	, public CMagicStateMallocObject
{
public:
	uint32							GetDynamicId()		{return m_uDynamicId;}
	CFighterDictator*				GetInstaller();											//获取施法者
	uint32							GetInstallerID() {return m_uInstallerID;}					//获取施法者EntityID
	virtual CFighterDictator*		GetOwner() {return NULL;}								//获取拥有者
	virtual CBaseStateCfgServer*	GetCfg() {return NULL;}									//获取配置表对象
	//virtual uint32					GetDynamicId()	{return 0;}								//获取动态ID
	CStateVaraintPointerHolder*		GetVaraintPointerHolder()	{return m_pVariantPointerHolder;}
	void							SetVaraintPointerHolder(CStateVaraintPointerHolder* pPointer) {m_pVariantPointerHolder = pPointer;}
	void							DeleteVaraintPointerHolder();
	int32							GetLeftTime();

protected:
	explicit CBaseStateServer(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller = NULL);
	explicit CBaseStateServer(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller, 
		int32 iTime, int32 iRemainTime);
	~CBaseStateServer();

	uint32							m_uDynamicId;

	//CFighterDictator*				m_pInstaller;							//施法者
	uint32							m_uInstallerID;							//施法者EntityID

	int32					m_iTime;			//总时间
	int32					m_iRemainTime;		//剩余时间
	uint64					m_uStartTime;		//开始时间

	CSkillInstServer*		m_pSkillInst;		//用于保存到数据库的CSkillInstServer指针，暂时复用以下三个中非空的任意一个

	CStateVaraintPointerHolder*		m_pVariantPointerHolder;
};

//在控制台打印状态信息
//#define COUT_STATE_INFO

