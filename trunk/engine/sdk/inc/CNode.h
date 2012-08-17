
#ifndef	_Node_H
#define	_Node_H
//---------------------------------------------------------------------------
#ifdef _WIN32

#pragma warning(disable: 4275)
#pragma warning(disable: 4251)

#endif

namespace sqr
{
namespace BaseDataStruct
{
class CNode
{
protected:
	CNode* m_pNext;
	CNode* m_pPrev;

public:
	CNode(void);
	virtual ~CNode() {};
	CNode* GetNext(void);
	CNode* GetPrev(void);
	void InsertBefore(CNode* pNode);
	void InsertAfter(CNode* pNode);
	void Clear();
	void Remove(void);
	bool IsLinked(void);

	friend class CList;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 函数:	CNode
// 功能:	构造
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
inline CNode::CNode(void)
{
	m_pNext = NULL;
	m_pPrev = NULL;
}
//---------------------------------------------------------------------------
// 函数:	GetNext
// 功能:	下一个结点
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CNode::GetNext(void)
{
	//if(m_pNext == NULL)
	//	return NULL;
	if (m_pNext->m_pNext)
		return m_pNext;
	return NULL;
}
//---------------------------------------------------------------------------
// 函数:	GetPrev
// 功能:	前一个结点
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline CNode* CNode::GetPrev(void)
{
	if (m_pPrev->m_pPrev)
		return m_pPrev;
	return NULL;
}
//----------------------------------------------------------------------------
// 函数:	InsertBefore
// 功能:	在前面插入一个结点
// 参数:	CNode*
// 返回:	void
//---------------------------------------------------------------------------
inline void CNode::InsertBefore(CNode *pNode)
{
	//KASSERT(m_pPrev);
	if (!pNode || !m_pPrev)
		return;

	pNode->m_pPrev = m_pPrev;
	pNode->m_pNext = this;
	m_pPrev->m_pNext = pNode;
	m_pPrev = pNode;
}
//---------------------------------------------------------------------------
// 函数:	InsertAfter
// 功能:	在后面插入一个结点
// 参数:	CNode*
// 返回:	void
//---------------------------------------------------------------------------
inline void CNode::InsertAfter(CNode *pNode)
{
	//KASSERT(m_pNext);
	if (!pNode || !m_pNext)
		return;
	pNode->m_pPrev = this;
	pNode->m_pNext = m_pNext;
	m_pNext->m_pPrev = pNode;
	m_pNext = pNode;
}
// 清空节点的指针
inline void CNode::Clear()
{
	m_pPrev = NULL;
	m_pNext = NULL;
}

//---------------------------------------------------------------------------
// 函数:	Remove the node
// 功能:	从链表中删除这个结点
// 参数:	void
// 返回:	CNode*
//---------------------------------------------------------------------------
inline void CNode::Remove(void)
{
	//KASSERT(m_pPrev);
	//KASSERT(m_pNext);
	if (m_pPrev)
		m_pPrev->m_pNext = m_pNext;
	if (m_pNext)
		m_pNext->m_pPrev = m_pPrev;
	m_pPrev = NULL;
	m_pNext = NULL;
}
//---------------------------------------------------------------------------
// 函数:	IsLinked
// 功能:	这个节点是否链接
// 参数:	void
// 返回:	bool
//---------------------------------------------------------------------------
inline bool CNode::IsLinked(void)
{
	return (m_pPrev && m_pNext);
}
//--------------------------------------------------------------------------------
}
}

::BaseDataStruct;

#endif
