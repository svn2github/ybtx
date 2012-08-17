#pragma once

#ifndef _PIECECPLIST_H_
#define _PIECECPLIST_H_

#include "Model.h"

// CPieceCpList
enum NODECPTYPE
{
	enumNT_COMPRESS,
	enumNT_UNCOMPRESS,
};
struct ListNodeCp
{
	ListNodeCp( NODECPTYPE NodeType,CPluginMesh* mesh )
	{
		m_Mesh = mesh;
		m_NodeType = NodeType;
		if( NodeType == enumNT_COMPRESS)
			m_ExpType = enumPT_COMPRESS;
		if( NodeType == enumNT_UNCOMPRESS)
			m_ExpType = enumPT_UNCOMPRESS;
	};

	PIECECPTYPE m_ExpType;
	NODECPTYPE m_NodeType;
	CPluginMesh* m_Mesh;
};
class CPieceCpList : public CListCtrl
{
	DECLARE_DYNAMIC(CPieceCpList)

public:
	CPieceCpList();
	virtual ~CPieceCpList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif
