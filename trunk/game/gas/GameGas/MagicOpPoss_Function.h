#pragma once
#include "CPosMagicOpServer.h"
#include "CPos.h"
#include "PatternCOR.h"
#include "CDistortedTick.h"

class CRelShockWaveMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CDelayRelShockWaveMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CDelayRelPositionMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CRelPositionMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CRelPositionMagicByDirAndDistanceMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CRelPositionMagicRandomInAreaMOP
	: public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CRelPosBulletMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};
class CRelPosMoveMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CRelBattleArrayMagicMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,const CFPos& pos);
};

// 猎杀--矮人骑士
class CPursueKillMOP
	:public CPosMagicOpServer
{
public:
	typedef vector<CFighterDictator*>	VecFighter;
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
	void QueryObj(VecFighter& vecCoreObj, CFighterDictator* pFrom, float fArea, ESimpleRelation eRelation);
};

//图腾 指定地点
class CCreateTotemMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CCreateTotemByDirAndDistanceMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CSetupDynamicBarrierMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, const CFPos& pos);
	CCfgArg* InitArg(const string& szArg);
};

class CUseDeadBobyMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

// 清除地点魔法
class CPositionMagicServer;
class CEraseEnemyPositionMagicMOP
	:public CPosMagicOpServer
{
public:
	typedef vector<uint32> VecPosMagic;
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
	void QueryObj(VecPosMagic& vecCoreObj, CFighterDictator* pFrom, const CFPos& pos, float fArea, ESimpleRelation eRelation);
};

class CPosCreateNPCMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CPosCreateNPCLevelSameMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CPosDelayCreateNPCMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos)		;
	CCfgArg* InitArg(const string& szArg);
};

class CTargetPosCreateNPCMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CTargetPosCreateNPCLevelSameMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CPosCreateDeadNPCMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};
class CCreateShadowMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CPostionPlayFXMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

class CPosCreateNpcWithMasterMOP
	:public CPosMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos);
};

