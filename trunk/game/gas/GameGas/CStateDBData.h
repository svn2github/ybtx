#pragma once
#include "CStaticObject.h"
#include "FightDef.h"
#include "TFighterAllocator.h"
#include "TMagicStateAllocator.h"
#include "TMagicAllocator.h"

class CStateDBData
	: public virtual CStaticObject
	, public CMagicStateMallocObject
{
public:
	CStateDBData() {}
	CStateDBData(EStateGlobalType eType, 
		const TCHAR* sName, int32 iTime, int32 iRemainTime, int32 iCount, float fProbability,
		uint32 uCancelableInstID, uint32 uDotInstID, uint32 uFinalInstID,
		uint8 uSkillLevel, const TCHAR* sSkillName,
		EAttackType eAttackType, bool bIsDot, bool bFromEqualsOwner);
	~CStateDBData() {}

	EStateGlobalType					GetType()					{return m_eType;}
	const TCHAR*						GetName()					{return m_sName.c_str();}
	int32								GetTime()					{return m_iTime;}
	int32								GetRemainTime()				{return m_iRemainTime;}
	int32								GetCount()					{return m_iCount;}
	float								GetProbability()			{return m_fProbability;}
	uint32								GetCancelableInstID()		{return m_uCancelableInstID;}
	uint32								GetDotInstID()				{return m_uDotInstID;}
	uint32								GetFinalInstID()			{return m_uFinalInstID;}
	uint8 								GetSkillLevel()				{return m_uSkillLevel;}
	const TCHAR*						GetSkillName()				{return m_sSkillName.c_str();}
	EAttackType							GetAttackType()				{return m_eAttackType;}
	bool								GetIsDot()					{return m_bIsDot;}
	bool								GetFromEqualsOwner()		{return m_bFromEqualsOwner;}

	void								SetType(EStateGlobalType eType)			{m_eType = eType;}
	void								SetName(TCHAR* sName)					{m_sName = sName;}
	void								SetTime(int32 iTime)					{m_iTime = iTime;}
	void								SetRemainTime(int32 iRemainTime)		{m_iRemainTime = iRemainTime;}
	void								SetCount(int32 iCount)					{m_iCount = iCount;}
	void								SetProbability(float fProbability)		{m_fProbability = fProbability;}
	void								SetCancelableInstID(uint32 uCancelableInstID)		{m_uCancelableInstID = uCancelableInstID;}
	void								SetDotInstID(uint32 uDotInstID)				{m_uDotInstID = uDotInstID;}
	void								SetFinalInstID(uint32 uFinalInstID)			{m_uFinalInstID = uFinalInstID;}
	void 								SetSkillLevel(uint8 uSkillLevel)			{m_uSkillLevel = uSkillLevel;}
	void								SetSkillName(TCHAR* sSkillName)				{m_sSkillName = sSkillName;}
	void								SetAttackType(EAttackType eAttackType)		{m_eAttackType = eAttackType;}
	void								SetIsDot(bool bIsDot)						{m_bIsDot = bIsDot;}
	void								SetFromEqualsOwner(bool bFromEqualsOwner)	{m_bFromEqualsOwner = bFromEqualsOwner;}

	EStateGlobalType m_eType;
	string m_sName;
	int32 m_iTime;
	int32 m_iRemainTime;
	int32 m_iCount;
	float m_fProbability;
	uint32 m_uCancelableInstID;
	uint32 m_uDotInstID;
	uint32 m_uFinalInstID;
	uint8 m_uSkillLevel;
	string m_sSkillName;
	EAttackType m_eAttackType;
	bool m_bIsDot;
	bool m_bFromEqualsOwner;
};

class CStoredObjDBData
	: public virtual CStaticObject
	, public CMagicStateMallocObject
{
public:
	CStoredObjDBData() {}
	CStoredObjDBData(uint32 uInstID, uint32 uCascadeID, uint32 uMOPID, int32 iMOPArg, uint32 uMOPRet)
		: m_uInstID(uInstID),
		m_uCascadeID(uCascadeID),
		m_uMOPID(uMOPID),
		m_iMOPArg(iMOPArg),
		m_uMOPRet(uMOPRet)
	{}

	uint32		GetInstID()			{return m_uInstID;}
	uint32		GetCascadeID()		{return m_uCascadeID;}
	uint32		GetMOPID()			{return m_uMOPID;}
	int32		GetMOPArg()			{return m_iMOPArg;}
	uint32		GetMOPRet()			{return m_uMOPRet;}

	void		SetInstID(uint32 uInstID)			{m_uInstID = uInstID;}
	void		SetCascadeID(uint32 uCascadeID)		{m_uCascadeID = uCascadeID;}
	void		SetMOPID(uint32 uMOPID)				{m_uMOPID = uMOPID;}
	void		SetMOPArg(int32 iMOPArg)			{m_iMOPArg = iMOPArg;}
	void		SetMOPRet(uint32 uMOPRet)			{m_uMOPRet = uMOPRet;}

	uint32 m_uInstID;
	uint32 m_uCascadeID;
	uint32 m_uMOPID;
	int32 m_iMOPArg;
	uint32 m_uMOPRet;
};

class CSkillCoolDownDBData
	: public virtual CStaticObject
	, public CFighterMallocObject
{
public:
	CSkillCoolDownDBData() {}
	CSkillCoolDownDBData(const TCHAR* sSkillName, uint32 uLeftTime, bool bSwitchEquipSkill)
		:m_uLeftTime(uLeftTime)
		,m_bSwitchEquipSkill(bSwitchEquipSkill)
	{
		m_sSkillName = sSkillName;
	}
	uint32 GetLeftTime()					{return m_uLeftTime;}
	const TCHAR* GetSkillName()					{return m_sSkillName.c_str();}
	bool GetIsSwitchEquipSkill()			{return m_bSwitchEquipSkill;}
	void SetLeftTime(uint32 uTime)		{m_uLeftTime = uTime;}
	void SetSkillName(TCHAR* SkillName)		{m_sSkillName = SkillName;}
	void SetIsSwitchEquipSkill(bool bSwitchEquipSkill)	{m_bSwitchEquipSkill = bSwitchEquipSkill;}
private:
	uint32 m_uLeftTime;
	string m_sSkillName;
	bool m_bSwitchEquipSkill;
};

class CAureMagicDBData
	: public virtual CStaticObject
	, public CMagicMallocObject
{
public:
	CAureMagicDBData() {}
	CAureMagicDBData(const TCHAR* sAureMagicName, uint8 uSkillLevel, const TCHAR* sSkillName, EAttackType eAttackType)
		:m_uSkillLevel(uSkillLevel),
		m_eAttackType(eAttackType)
	{
		m_sAureMagicName = sAureMagicName;
		m_sSkillName = sSkillName;
	}
	const TCHAR* GetSkillName()					{return m_sSkillName.c_str();}
	uint8 GetSkillLevel()					{return m_uSkillLevel;}
	uint32 GetAttackType()					{return m_eAttackType;}
	const TCHAR* GetAureMagicName()				{return m_sAureMagicName.c_str();}
	void SetSkillName(TCHAR* SkillName)		{m_sSkillName = SkillName;}
	void SetSkillLevel(uint8 uLevel)		{m_uSkillLevel = uLevel;}
	void SetAttackType(EAttackType eEype)	{m_eAttackType = eEype;}
	void SetAureMagicName(TCHAR* MagicName)		{m_sAureMagicName = MagicName;}
private:
	string m_sSkillName;
	uint8 m_uSkillLevel;
	EAttackType m_eAttackType;
	string m_sAureMagicName;
};







template <class DBDataType, class AllocatorType = allocator<DBDataType> >
class CDBDataVec
	: public virtual CStaticObject
{
public:
	DBDataType* CreateDBData();
	~CDBDataVec();
	DBDataType* Get(uint32 uIndex)			{return m_vecDBData[uIndex];}
	uint32 GetCount()						{return m_vecDBData.size();}
	void PushBack(DBDataType* aDBData)		{m_vecDBData.push_back(aDBData);}
	DBDataType* GetFront()					{return m_vecDBData.front();}
	void PopFront()							{m_vecDBData.pop_front();}
	bool Empty()							{return m_vecDBData.empty();}
private:
	deque<DBDataType*, AllocatorType>		m_vecDBData;
};

	class CStateDBDataVec
		: public CDBDataVec<CStateDBData, TMagicStateAllocator<CStateDBData> >
		, public CMagicStateMallocObject
	{

	};

	class CStoredObjDBDataVec
		: public CDBDataVec<CStoredObjDBData, TMagicStateAllocator<CStoredObjDBData> >
		, public CMagicStateMallocObject
	{

	};

class CStateDBDataSet
	: public virtual CStaticObject
	, public CMagicStateMallocObject
{
public:
	CStateDBDataSet();
	void Release();
	~CStateDBDataSet();
	CStateDBDataVec*			GetStateVec()			{return m_pStateVec;}
	CStoredObjDBDataVec*		GetStoredObjVec()		{return m_pStoredObjVec;}
	CStateDBDataVec*			m_pStateVec;
	CStoredObjDBDataVec*		m_pStoredObjVec;
};


#ifdef _WIN32
	typedef CDBDataVec<CSkillCoolDownDBData>		CSkillCoolDownDBDataVec;
#else
	class CSkillCoolDownDBDataVec
		: public CDBDataVec<CSkillCoolDownDBData, TFighterAllocator<CSkillCoolDownDBData> >
		, public CFighterMallocObject
	{

	};
#endif

class CSkillCoolDownDBDataMgr
	: public virtual CStaticObject
	, public CFighterMallocObject
{
public:
	CSkillCoolDownDBDataMgr();
	void Release();
	~CSkillCoolDownDBDataMgr();
	CSkillCoolDownDBDataVec* GetSkillCoolDownDBDateVec()	{ return m_pSkillCoolDownVec; }
	void SetSkillCoolDownDBDateVec(CSkillCoolDownDBDataVec* pDBDataVec)		{ m_pSkillCoolDownVec = pDBDataVec; }
private:
	CSkillCoolDownDBDataVec*		m_pSkillCoolDownVec;
};


#ifdef _WIN32
	typedef CDBDataVec<CAureMagicDBData>		CAureMagicDBDataVec;
#else
	class CAureMagicDBDataVec
		: public CDBDataVec<CAureMagicDBData, TMagicAllocator<CAureMagicDBData> >
		, public CMagicMallocObject
	{

	};
#endif


class CAureMagicDBDataMgr
	: public virtual CStaticObject
	, public CMagicMallocObject
{
public:
	CAureMagicDBDataMgr();
	~CAureMagicDBDataMgr();
	CAureMagicDBDataVec* GetAureMagicDBDateVec()	{ return m_pAureMagicVec; }
	void SetAureMagicDBDateVec( CAureMagicDBDataVec* pDBDataVec )	{m_pAureMagicVec = pDBDataVec;}

private:
	CAureMagicDBDataVec*		m_pAureMagicVec;
};

//#define COUT_STATEDB_INFO

