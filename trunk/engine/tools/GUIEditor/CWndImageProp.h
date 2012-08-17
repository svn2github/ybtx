#pragma once

class CWndGroup;


class CWndImageProp : public ::CListCtrl
{
	DECLARE_DYNAMIC(CWndImageProp)

	::CComboBox	m_Lock;
	::CEdit		m_Edit;
	int			m_nEditItem;
	IMAGE*		m_pImage;
	WND_IMAGE_LIST* m_ImageList;

	CPoint		m_CurPos;
	LPCTSTR     m_CurID;

	CWndGroup* m_pWndGroup;

	map<unsigned, CString> m_Flag2String; 
	map<CString, unsigned> m_String2Flag; 

	vector<int> m_vChooseImgList;

public:
	CWndImageProp();
	virtual ~CWndImageProp();
	void ResetImageProp( IMAGE* pImage, CWndGroup* pWndGroup );
	void UpdateProp();
	void MoveText( UINT uMessage, WPARAM wParam, LPARAM lParam, SQRWnd* pEditWnd );
	void DrawEditLine( CGraphic* pGraphic, SQRWnd* pEditWnd, int n );
	void Flip( bool Vert );

	void SetImageList( WND_IMAGE_LIST* imageList );
	void SetChooseImgList(vector<int> chooseImgList);
	IMAGE* GetImage(){return m_pImage;}

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnPaint();	
};
