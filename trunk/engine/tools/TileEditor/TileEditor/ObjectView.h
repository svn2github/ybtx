#pragma once


#include "FileTree.h"

// CObjectView 窗体视图

class CObjectView : public CFormView
{
public:
	CObjectView();           // 动态创建所使用的受保护的构造函数
	virtual ~CObjectView();

public:
	enum { IDD = IDD_TEST };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy() { }

	DECLARE_MESSAGE_MAP()
public:
	void Init();
	CFileTree m_pathTree;
	CFileList m_fileList;

protected:
	virtual void PreSubclassWindow();
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};


