#pragma once
#include "CCfgArg.h"

class CNormalSkillServerSharedPtr;
class CMagicStateCfgServerSharedPtr;
class CTriggerStateCfgServerSharedPtr;
class CDamageChangeStateCfgServerSharedPtr;
class CCumuliTriggerStateCfgServerSharedPtr;
class CSpecialStateCfgServerSharedPtr;
class CAureMagicCfgServerSharedPtr;
class CMoveMagicCfgServerSharedPtr;
class CTransferableMagicCfgServerSharedPtr;
class CBulletMagicCfgServerSharedPtr;
class CPositionMagicCfgServerSharedPtr;
class CCastingProcessCfgServerSharedPtr;
class CShockWaveMagicCfgServerSharedPtr;
class CBarrierMagicCfgServerSharedPtr;
class CMagicEffServerSharedPtr;
class CBaseStateCfg;

class CPlayerSkillCfgArg
	:public CCfgArg
{
public:
	CPlayerSkillCfgArg(CNormalSkillServerSharedPtr* pCfg,uint32 uSkillLevel);
	~CPlayerSkillCfgArg(){}
	CNormalSkillServerSharedPtr* m_pCfg;
	uint32 m_uSkillLevel;
};

class CTriggerStateCfgArg
	:public CCfgArg
{
public:
	CTriggerStateCfgArg(CTriggerStateCfgServerSharedPtr* pCfg);
	~CTriggerStateCfgArg(){};
	CTriggerStateCfgServerSharedPtr* m_pCfg;
};

class CAureMagicCfgArg
	:public CCfgArg
{
public:
	CAureMagicCfgArg(CAureMagicCfgServerSharedPtr* pCfg);
	CAureMagicCfgServerSharedPtr* m_pCfg;
};

class CMagicStateCfgArg
	:public CCfgArg
{
public:
	CMagicStateCfgArg(CMagicStateCfgServerSharedPtr* pCfg);
	CMagicStateCfgServerSharedPtr* m_pCfg;
};

class CMagicStateCfgMultiArg
	:public CMagicStateCfgArg
{
public:
	CMagicStateCfgMultiArg(CMagicStateCfgServerSharedPtr* pCfg, const string& szArg);
	~CMagicStateCfgMultiArg();
	CCfgCalc*	m_pArg;
};

class CDamageChangeStateCfgArg
	:public CCfgArg
{
public:
	CDamageChangeStateCfgArg(CDamageChangeStateCfgServerSharedPtr* pCfg);
	~CDamageChangeStateCfgArg(){}
	CDamageChangeStateCfgServerSharedPtr* m_pCfg;
};

class CCumuliTriggerStateCfgArg
	:public CCfgArg
{
public:
	CCumuliTriggerStateCfgArg(CCumuliTriggerStateCfgServerSharedPtr* pCfg);
	~CCumuliTriggerStateCfgArg(){};
	CCumuliTriggerStateCfgServerSharedPtr* m_pCfg;
};

class CSpecialStateCfgArg
	:public CCfgArg
{
public:
	CSpecialStateCfgArg(CSpecialStateCfgServerSharedPtr* pCfg);
	~CSpecialStateCfgArg(){}
	CSpecialStateCfgServerSharedPtr* m_pCfg;
};

class CMoveMagicCfgArg
	:public CCfgArg
{
public:
	CMoveMagicCfgArg(CMoveMagicCfgServerSharedPtr* pCfg);
	~CMoveMagicCfgArg(){}
	CMoveMagicCfgServerSharedPtr* m_pCfg;
};

class CMoveMagicCfgMultiArg
	:public CCfgArg
{
public:
	CMoveMagicCfgMultiArg(CMoveMagicCfgServerSharedPtr* pCfg, const string& szArg);
	~CMoveMagicCfgMultiArg();
	CMoveMagicCfgServerSharedPtr* m_pCfg;
	CCfgCalc*	m_pArg;
};

class CTransferableMagicCfgArg
	:public CCfgArg
{
public:
	CTransferableMagicCfgArg(CTransferableMagicCfgServerSharedPtr* pCfg);
	CTransferableMagicCfgServerSharedPtr* m_pCfg;
};

class CBulletMagicCfgArg
	:public CCfgArg
{
public:
	CBulletMagicCfgArg(CBulletMagicCfgServerSharedPtr* pCfg);
	CBulletMagicCfgServerSharedPtr* m_pCfg;
};

class CPositionMagicCfgArg
	:public CCfgArg
{
public:
	CPositionMagicCfgArg(CPositionMagicCfgServerSharedPtr* pCfg, const string& szArg);
	~CPositionMagicCfgArg();
	CPositionMagicCfgServerSharedPtr* m_pCfg;
	CCfgCalc* m_pArg;
};

class CDelayPositionMagicCfgArg
	:public CCfgArg
{
public:
	CDelayPositionMagicCfgArg(uint32 uDelayTime, CPositionMagicCfgServerSharedPtr* pCfg);
	~CDelayPositionMagicCfgArg();

	uint32								m_uDelayTime;
	CPositionMagicCfgServerSharedPtr*	m_pCfg;
};

class CShockWaveMagicCfgArg
	:public CCfgArg
{
public:
	CShockWaveMagicCfgArg(CShockWaveMagicCfgServerSharedPtr* pCfg);
	CShockWaveMagicCfgServerSharedPtr* m_pCfg;
};

class CBarrierMagicCfgArg
	:public CCfgArg
{
public:
	CBarrierMagicCfgArg(CBarrierMagicCfgServerSharedPtr* pCfg);
	CBarrierMagicCfgServerSharedPtr* m_pCfg;
};

class CMagicEffCfgArg
	:public CCfgArg
{
public:
	CMagicEffCfgArg(CMagicEffServerSharedPtr* pCfg, const string& szArg);
	~CMagicEffCfgArg();
	CMagicEffServerSharedPtr* m_pCfg;
	CCfgCalc* m_pArg;
};

class CBaseStateCfgArg
	:public CCfgArg
{
public:
	CBaseStateCfgArg(CBaseStateCfg* pCfg);
	CBaseStateCfg* m_pCfg;
};

class CDisplayMsgCfgArg
	:public CCfgArg
{
public:
	CDisplayMsgCfgArg(uint32 uArg1, const string& szArg2);
	uint32 m_uArg1;
	string m_strArg2;
};


class CTwoCfgCalcArg
	:public CCfgArg
{
public:
	CTwoCfgCalcArg(const string& szArg1, const string& szArg2);
	~CTwoCfgCalcArg();
	CCfgCalc* m_pArg1;
	CCfgCalc* m_pArg2;
};

class CTwoStringArg
	:public CCfgArg
{
public:
	CTwoStringArg(const string& szArg1, const string& szArg2);
	string m_strArg1;
	string m_strArg2;
};

class CPosDelayCreateNPCArg
	:public CCfgArg
{
public:
	CPosDelayCreateNPCArg(uint32 uArg1, const string& szArg2);
	uint32 m_uArg1;
	string m_strArg2;
};
