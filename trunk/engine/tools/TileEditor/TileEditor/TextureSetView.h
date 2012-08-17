#pragma once

#include <BluelyG.h>
#include "texturedbox.h"
#include "afxcmn.h"
#include "FileTree.h"
#include "yabffw.h"

class CTextureSetView : public CFormView
{
public:
	CTextureSetView();           
	virtual ~CTextureSetView();

public:
	enum { IDD = IDD_TEXTURESET };

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void PostNcDestroy() { }
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	void Init();
	CTexturedBox m_layer1;
	CTexturedBox m_layer2;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CFileTree m_PathTree;
	CFileList m_FileList;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void SetLayerHot(DWORD dwLayer);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};