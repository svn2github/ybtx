#pragma once

#ifndef _CHECKLIST_H_
#define _CHECKLIST_H_

#include "Model.h"

enum NODETYPE
{
	enumNT_MESH,
	enumNT_SKELETALMESH,
	enumNT_SKELETAL,
	enumNT_UNKNOW,
};

struct ListNode
{
	ListNode( NODETYPE NodeType, INode* MaxNode )
	{
		m_NodeType = NodeType;
		m_MaxNode = MaxNode;
		if( NodeType == enumNT_SKELETAL )
			m_ExpType = enumPT_SKELETAL;
		if( NodeType == enumNT_MESH )
			m_ExpType = enumPT_NONEANI;
		if( NodeType == enumNT_SKELETALMESH )
			m_ExpType = enumPT_SKELETALANI;
	};

	PIECETYPE m_ExpType;
	NODETYPE m_NodeType;
	INode*   m_MaxNode;
};

// CCheckList
#define WM_SELECTCHANGED 0x7801

class CCheckList : public CListCtrl
{
	DECLARE_DYNAMIC(CCheckList)

public:
	CCheckList();
	virtual ~CCheckList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif
