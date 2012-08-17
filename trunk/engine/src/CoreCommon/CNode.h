#pragma once

#include "CPos.h"
#include "CFindPathMallocObject.h"

NAMESPACE_BEGIN(sqr)

#pragma pack(push,1)

class CNode
	:public CFindPathMallocObject
{
public:

	//共用函数
	inline float GetG();
	inline void SetG(float fG);
	inline bool IsOpen(uint32 nSearchId);
	inline void Open(uint32 nSearchId);
	inline bool IsClose(uint32 nSearchId);
	inline void Close(uint32 nSearchId);

	inline void SetTargetId(uint16 nTargetId);
	inline uint16 GetTargetId();
	//A*使用的函数
	inline void SetFrontDir(uint8 nFatherDir);
	inline uint8 GetFrontDir();

	//连通区域寻路使用的函数
	inline void SetFatherRegion(uint32 nSearchId);
	inline bool IsFatherRegion(uint32 nSearchId);
	inline void SetFrontIndex(uint8 nIndex);
	inline uint8 GetFrontIndex();
private:

	float   m_fG;        // 移动的值
	uint8	m_nState;	//  	A* 寻路:	高位起 第一个bit 为 1 表示open, 0 表示close,  较低的4个bit代表寻路的上一个节点 方向, (中间3位无用)
						// 连通区域寻路:  高位起 第一个bit 为 1 表示open, 0 表示close, 第二个bit 表示是否为父连通区域, 较低的6个bit代表寻路的上一个节点在linklist的索引
	
	uint16	m_nTargetId;
	uint32  m_nSearchId;	//当前节点对应的查询id, 用来判断是否本次寻路探索过的节点
};

#pragma pack(pop)

NAMESPACE_END


