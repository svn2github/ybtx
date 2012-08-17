#pragma once
#include "CNpcAI.h"

typedef map<uint8, CFPos* , less<uint8>, TNpcAIAllocator<pair<uint8, CFPos*> > > MemPathMapType;
struct MemPathPointData
	: public CNpcAIMallocObject 
{
	MemPathMapType m_mapMovePath;
	MemPathMapType m_mapRevesePointPath;
	CFPos	m_onceDstPos;
	CFPos	m_LocalPos;
	CFPos	m_LocalMirrorPos;
};
typedef map<uint32,MemPathPointData*, less<uint32>, TNpcAIAllocator<pair<uint32, MemPathPointData*> > > MemberPathMapByID;

class CDummyAI
	:public CNpcAI
{
public:
	CDummyAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	virtual ~CDummyAI();
	virtual ECharacterInnerHandlerType GetType() const { return ECharacterInnerHandlerType_Dummy;}
	virtual void MoveToDest(float,float);
	void NotifyEventToMember(ENpcEvent eNpcEvent, ENpcEventCodeIndex eNpcEventCodeIndex);
	bool GetIsInitMemberPath() {return m_bIsInitMemberPath;}
	void CreatePatrolPath();//创建根据成员ID保存巡逻路径的MAP
	CFPos GetMemberPathPoint(uint32 uMemID, uint8 uMovePoint,bool bMoveDir);
	CFPos GetMemberFirstDstPos(uint32 uMemID);
	CFPos GetMemberReBornPosition(uint32 uMemID);
	bool IsInitMemberPathPoint(uint32 uMemID);
	bool MemberCanFight(CCharacterDictator* pTargetChar);

	void CreateMemeberPathPoint();
	float CalculateAngle(float x1,float y1,float x2,float y2);
	void CalculateMemberLocalPos();
	CFPos ChangeLocalPosToWorldPos(CFPos originPos,CFPos localPos,float fAngle);
	CFPos CalculateDistancePosToDest(CFPos beginPos,CFPos endPos,float fDistance);
	void UpdateMemberPos();
	void SetMemberSynState(bool synState);
	bool CheckMemberSynState();
	void NotifyMemberBeAttacked(CCharacterDictator* pAttacker);
	void NotifyMemberRemoveEnemy(CCharacterDictator* pAttacker);
	void NotifyClearAllEnmity();
	void OnMemberJoinGroup(CCharacterDictator* pMember);
	void CopyEnmityListToMember(CMemberAI* pMemAI, CNpcEventMsg* pEvent = NULL);
	void OnMemberAllDead();
	virtual void OnMoveToPathPoint(uint8 point);
private:
	MemberPathMapByID m_mapMemberPathByID;
public:
	bool					m_bIsWaitForAdjustFinish;
	bool					m_bIsInitMemberPath;
	bool					m_bIsMoveToFirstPoint;
};
