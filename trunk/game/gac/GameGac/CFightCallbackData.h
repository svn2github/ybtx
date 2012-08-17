#pragma once
#include "CStaticObject.h"
#include "CFighterMallocObject.h"

class CFightInfoData
	: public virtual CStaticObject
	, public CFighterMallocObject
{
public:
	CFightInfoData() {}
	~CFightInfoData() {}

	EFightInfoIndex	GetInfoIndex()			{return m_eInfoIndex;}
	uint32			GetObj1ID()				{return m_uObj1ID;}
	uint32			GetObj2ID()				{return m_uObj2ID;}
	const TCHAR*	GetSkillName()			{return m_sSkillName.c_str();}
	const TCHAR*	GetStateName()			{return m_sStateName.c_str();}
	const TCHAR*	GetArgName()			{return m_sArgName.c_str();}
	int32			GetHPChanged()			{return m_iHPChanged;}
	uint32			GetHPExtra()			{return m_uHPExtra;}
	uint8			GetAttackType()			{return m_uAttackType;}
	uint8			GetHour()				{return m_uHour;}
	uint8			GetMinute()				{return m_uMinute;}
	uint8 			GetSecond()				{return m_uSecond;}
	uint16			GetMillisecond()		{return m_uMillisecond;}
	uint32			GetRealHPChanged()		{return m_uRealHPChanged;}

	void			SetInfoIndex(EFightInfoIndex eInfoIndex){ m_eInfoIndex = eInfoIndex; }
	void			SetObj1ID(uint32 uObj1ID)				{ m_uObj1ID = uObj1ID; }
	void			SetObj2ID(uint32 uObj2ID)				{ m_uObj2ID = uObj2ID; }
	void			SetSkillName(const TCHAR* sSkillName)	{ m_sSkillName = sSkillName; }
	void			SetStateName(const TCHAR* sStateName)	{ m_sStateName = sStateName; }
	void			SetArgName(const TCHAR* sArgName)		{ m_sArgName = sArgName; }
	void			SetHPChanged(int32 iHPChanged)			{ m_iHPChanged = iHPChanged; }
	void			SetHPExtra(uint32 uHPExtra)				{ m_uHPExtra = uHPExtra; }
	void			SetAttackType(uint8 uAttackType)		{ m_uAttackType = uAttackType; }
	void			SetHour(uint8 uHour)					{ m_uHour = uHour; }
	void			SetMinute(uint8 uMinute)				{ m_uMinute = uMinute; }
	void 			SetSecond(uint8 uSecond)				{ m_uSecond = uSecond; }
	void			SetMillisecond(uint16 uMillisecond)		{ m_uMillisecond = uMillisecond; }
	void			SetRealHPChanged(uint32 uRealHPChanged)	{ m_uRealHPChanged = uRealHPChanged; }

private:
	EFightInfoIndex m_eInfoIndex;
	uint32 m_uObj1ID;
	uint32 m_uObj2ID;
	string m_sSkillName;
	string m_sStateName;
	string m_sArgName;
	int32 m_iHPChanged; 
	uint32 m_uHPExtra;
	uint8 m_uAttackType;
	uint8 m_uHour;
	uint8 m_uMinute;
	uint8 m_uSecond;
	uint32 m_uMillisecond;
	uint32 m_uRealHPChanged;
};


class CBuffData
	: public virtual CStaticObject
	, public CFighterMallocObject
{
public:
	CBuffData() {}
	~CBuffData() {}

	uint32			GetObjID()			{ return m_uObjID; }
	uint32			GetDynamicId()		{ return m_uDynamicId; }
	const TCHAR*	GetName()			{ return m_sName.c_str(); }
	uint32			GetCount()			{ return m_uCount; }
	int32			GetTime()			{ return m_iTime; }
	float			GetRemainTime()		{ return m_fRemainTime; }
	bool			GetDecrease()		{ return m_bDecrease; }
	int32			GetSkillLevel()		{ return m_uSkillLevel; }
	float			GetDotInterval()	{ return m_fDotInterval; }
	uint32			GetInstallerID()	{ return m_uInstallerID; }
	int32			GetCalcValue()		{ return m_iCalcValue; }

	void			SetObjID(uint32 uObjID)				{ m_uObjID = uObjID; }
	void			SetDynamicId(uint32 uDynamicId)		{ m_uDynamicId = uDynamicId; }
	void			SetName(const TCHAR* sName)			{ m_sName = sName; }
	void			SetCount(uint32 uCount)				{ m_uCount = uCount; }
	void			SetTime(int32 iTime)				{ m_iTime = iTime; }
	void			SetRemainTime(float fRemainTime)	{ m_fRemainTime = fRemainTime; }
	void			SetDecrease(bool bDecrease)			{ m_bDecrease = bDecrease; }
	void			SetSkillLevel(int32 uSkillLevel)	{ m_uSkillLevel = uSkillLevel; }
	void			SetDotInterval(float fDotInterval)	{ m_fDotInterval = fDotInterval; }
	void			SetInstallerID(uint32 uInstallerID)	{ m_uInstallerID = uInstallerID; }
	void			SetCalcValue(int32 iCalcValue)		{ m_iCalcValue = iCalcValue; }

private:
	uint32 m_uObjID;
	uint32 m_uDynamicId;
	string m_sName;
	uint32 m_uCount;
	int32 m_iTime;
	float m_fRemainTime;
	bool m_bDecrease;
	uint32 m_uSkillLevel;
	float m_fDotInterval;
	uint32 m_uInstallerID;
	int32 m_iCalcValue;
};