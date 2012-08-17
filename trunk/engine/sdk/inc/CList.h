
#ifndef	_CList_H
#define	_CList_H

//---------------------------------------------------------------------------
#include "CNode.h"
//---------------------------------------------------------------------------

namespace sqr
{
namespace BaseDataStruct
{
class CList
{
protected:
	CNode m_ListHead; // 头节点（不是第一个节点）
	CNode m_ListTail; // 尾节点（不是最后的节点）
public:
	CList(void);
	CNode* GetHead(void); // 取得第一个节点
	CNode* GetTail(void); // 取得最后一个节点
	void AddHead(CNode *pNode); // 在最前面增加一个节点
	void AddTail(CNode *pNode); // 在最后面增加一个节点
	CNode* RemoveHead(void); // 删除第一个节点
	CNode* RemoveTail(void); // 删除最后一个节点
	bool IsEmpty(void); // 是否是个空的链表
	long GetNodeCount(void);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 函数:	CList
// 功能:	构造
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
inline CList::CList(void)
{
	m_ListHead.m_pNext = &m_ListTail;
	m_ListTail.m_pPrev = &m_ListHead;
}
//-------------------------------------------------------------------------
// 函数:	IsEmpty
// 功能:	是否为空
// 参数:	void
// 返回:	bool
//---------------------------------------------------------------------------
inline bool CList::IsEmpty(void)
{
	return (m_ListHead.GetNext() == NULL);
}
//-------------------------------------------------------------------------
// 函数:	GetHead
// 功能:	取得链表的头
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CList::GetHead(void)
{
	return m_ListHead.GetNext();
}
//-------------------------------------------------------------------------
// 函数:	GetTail
// 功能:	取得链表的尾
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CList::GetTail(void)
{
	return m_ListTail.GetPrev();
}
//-------------------------------------------------------------------------
// 函数:	AddHead
// 功能:	在头部增加一个节点
// 参数:	CNode*
// 返回:	bool
//---------------------------------------------------------------------------
inline void CList::AddHead(CNode *pNode)
{
	m_ListHead.InsertAfter(pNode);
}
//-------------------------------------------------------------------------
// 函数:	AddTail
// 功能:	在末尾增加一个节点
// 参数:	CNode*
// 返回:	void
//---------------------------------------------------------------------------
inline void CList::AddTail(CNode *pNode)
{
	m_ListTail.InsertBefore(pNode);
}
//-------------------------------------------------------------------------
// 函数:	RemoveHead
// 功能:	删除第一个节点
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CList::RemoveHead(void)
{
	CNode* pNode = m_ListHead.GetNext();
	if (pNode)
		pNode->Remove();
	return pNode;
}
//-------------------------------------------------------------------------
// 函数:	RemoveTail
// 功能:	删除最后一个节点
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CList::RemoveTail(void)
{
	CNode* pNode = m_ListTail.GetPrev();
	if (pNode)
		pNode->Remove();
	return pNode;
}
//-------------------------------------------------------------------------
// 函数:	GetNodeCount
// 功能:	取得节点个数
// 参数:	void
// 返回:	long
//---------------------------------------------------------------------------
inline long CList::GetNodeCount(void)
{
	long nNode = 0;
	CNode* pNode = GetHead();
	while (pNode)
	{
		pNode = pNode->GetNext();
		nNode++;
	}
	return nNode;
}
//-------------------------------------------------------------------------
}
}

::BaseDataStruct;
#endif
