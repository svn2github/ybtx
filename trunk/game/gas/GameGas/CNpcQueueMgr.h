#pragma once
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"

/************************************************************************/
/*					Npc队列管理器管理Npc和主人之间的相对位置
*/
/************************************************************************/

class CNpcQueueMember;

class CNpcQueueMgr 
	: public CNpcAIMallocObject
{
public:
	CNpcQueueMgr();
	~CNpcQueueMgr();
public:
	int32 AddMemberAndGetAngle(uint32 uEntityID);
	void RemoveMember(uint32 uEntityID);
private:
	typedef list<CNpcQueueMember*, TNpcAIAllocator<CNpcQueueMember*> > CNpcQueue;
	CNpcQueue m_lNpcQueue;
};
