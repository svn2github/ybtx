#pragma once

// CBasePropList
class CWndGroup;
class CWndImageProp;

class CBasePropList : public ::CListCtrl
{
	DECLARE_DYNAMIC(CBasePropList)

	::CEdit			m_Edit;
	::CComboBox		m_WndFont;
	::CComboBox		m_Lock;
	::CComboBox		m_BoldFont;

	int				m_nEditItem;

	CWndGroup*		m_pWndGroup;
	CWndImageProp*	mWndImageProp;
	map<unsigned, CString> m_Flag2String; 
	map<CString, unsigned> m_String2Flag; 
public:
	CBasePropList();
	virtual ~CBasePropList();
	void Init( CWndGroup* pWndGroup );
	void SetToItem();
	void ItemToProp( bool bCheck = true );

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnPaint();	
public:
	enum ITEM_ID
	{
		ITEM_ID_CAPTION = 0,
		ITEM_ID_STYLE,
		ITEM_ID_XPOS,
		ITEM_ID_YPOS,
		ITEM_ID_WIDTH,
		ITEM_ID_HEIGHT,
		ITEM_ID_GROUPID,
		ITEM_ID_FONTID,
		ITEM_ID_FONTSIZE,
		ITEM_ID_BOLDFONT,
		ITEM_ID_DESC,
		ITEM_ID_LOCKFLAG,
		ITEM_ID_USERCLASS,
		ITEM_ID_USERINFO,
		ITEM_ID_CLICKFX,
		ITEM_ID_OPENFX,
		ITEM_ID_CLOSEFX,
		ITEM_ID_STYLE2,
		ITEM_ID_SOURCE
	};
};