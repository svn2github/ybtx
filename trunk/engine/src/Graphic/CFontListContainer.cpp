#include "StdAfx.h"
#include "CFontListContainer.h"
#include "TSqrAllocator.inl"

CFontList::CFontList()
: m_pHead(NULL)
, m_pTail(NULL)
{
}

CFontList::~CFontList()
{
	clear();
}

void CFontList::clear()
{
	while (m_pHead)
	{
		CFontListNode* tempHead = m_pHead;
		m_pHead = m_pHead->m_Next;
		delete tempHead;
	}
}

CFontListNode* CFontList::head()
{
	return m_pHead;
}

CFontListNode* CFontList::tail()
{
	return m_pTail;
}

CFontListNode* CFontList::push_front(wchar_t ch)
{
	CFontListNode* tempNode = new CFontListNode(ch);
	// fill the tempNode 
	tempNode->m_Next = m_pHead;
	if (m_pHead)
		m_pHead->m_PreV = tempNode;
	else
		m_pTail = tempNode;

	// and refresh the head and tail
	m_pHead = tempNode;
	
	return tempNode;
}

void CFontList::pop_back()
{
	if (m_pTail)
	{
		CFontListNode* tempNode = m_pTail;
		
		// refresh the head and tail
		m_pTail = m_pTail->m_PreV;	// maybe null
		if (m_pTail)
			m_pTail->m_Next = NULL;	
		else
			m_pHead = NULL;
		
		// eraser the node
		delete tempNode;	
	}
}

void CFontList::movetohead(CFontListNode* node)
{
	// make sure that here have two element at least
	// and the node is not the head
	if (m_pHead == m_pTail || m_pHead == node || !node)
		return;

	// remove the node 
	CFontListNode* tempPreNode = node->m_PreV;
	CFontListNode* tempNexNode = node->m_Next;
		//pre node
	if (tempPreNode)
		tempPreNode->m_Next = node->m_Next;
		//next node
	if (tempNexNode)
		tempNexNode->m_PreV = node->m_PreV;
	// if it is the tail node
	if (m_pTail == node)
		m_pTail = tempPreNode;

	// insert the node at the head
	node->m_PreV = NULL;
	node->m_Next = m_pHead;
	m_pHead->m_PreV = node;
		// and refresh the head and tail
	m_pHead = node;
}
