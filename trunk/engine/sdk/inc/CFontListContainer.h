#pragma once
#include "CGraphicMallocObject.h"

struct CFontListNode : public CGraphicMallocObject
{
	CFontListNode(wchar_t ch): m_Node(ch), m_Next(NULL), m_PreV(NULL){}
	wchar_t				m_Node;
	CFontListNode*		m_Next;
	CFontListNode*		m_PreV;
};

class CFontList : public CGraphicMallocObject
{
public:
	CFontList();
	~CFontList();
	
	void clear();
	CFontListNode* head();
	CFontListNode* tail();

	CFontListNode* push_front(wchar_t ch);
	void pop_back();

	void movetohead(CFontListNode* node);
private:
	CFontListNode*	m_pHead;
	CFontListNode*	m_pTail;
};

