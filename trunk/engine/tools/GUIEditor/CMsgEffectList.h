#pragma once


class CWndGroup;

class CMsgEffectList : public ::CListCtrl
{
	DECLARE_DYNAMIC(CMsgEffectList)

	::CEdit			m_Edit;
	int				m_nEditItem;
	CWndGroup*	m_pWndGroup;

public:
	CMsgEffectList();
	virtual ~CMsgEffectList();
	void Init( CWndGroup* pWndGroup );
	void OnUpdate();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnPaint();	
};
