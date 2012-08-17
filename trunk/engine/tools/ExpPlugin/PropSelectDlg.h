#pragma once
#ifndef _PROPSELECTDLG_H_
#define _PROPSELECTDLG_H_

#include "checklist.h"
#include "afxwin.h"
#include <map>
#include "piececplist.h"
// 临时修改，解决编译不过问题
//#include "ParaAdapterCtrl\ParaAdapterDlg.h"

// CPropSelectDlg dialog
enum BACKTYPE
{
	BACKTYPE_TRUE,
	BACKTYPE_FALSE,
	BACKTYPE_CANCEL,
};
class CPiecePropDlg : public CDialog
{
	DECLARE_DYNAMIC(CPiecePropDlg)

	CPluginModel                  m_Model;
	SAVEPARAM               m_SaveParam;

	vector< ListNode* >		m_NodeType;
	Interface*              m_pInterface;
	BOOL                    m_Select;

	int                     m_WeightPerVertex;
	int                     m_FramePerSample;
	CCheckList              m_ExpTypeSelect;
	BOOL					m_SmoothAll;
	// 临时修改，解决编译不过问题
	//CParaAdapterDlg			m_ParaAdapterDlg;

public:
	CPiecePropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPiecePropDlg();

// Dialog Data
	enum { IDD = IDD_PIECEPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal( CString FileName, Interface* pInterface, BOOL Select ,BOOL IsExist);
    
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnSetPathBtn();
	afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};

// CPieceCompressPropDlg dialog

class CPieceCompressPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CPieceCompressPropDlg)

	CPluginModel*                  m_Model;
	vector< ListNodeCp* >		m_NodeType;
	BOOL                    m_Select;
public:
	CPieceCompressPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPieceCompressPropDlg();

	// Dialog Data
	enum { IDD = IDD_PIECECOMPRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CPieceCpList m_ExpCpSelect;
public:
	virtual INT_PTR DoModal( CPluginModel* tModel);
	virtual BOOL OnInitDialog();
};


// CSkeletalProp dialog

class CSkeletalProp : public CDialog
{
	DECLARE_DYNAMIC(CSkeletalProp)

	CPluginModel                  m_Model;
	SAVEPARAM               m_SaveParam;

	vector< ListNode* >		m_NodeType;
	Interface*              m_pInterface;
	BOOL                    m_Select;

	CString					m_AniName;

public:
	CSkeletalProp(CWnd* pParent = NULL);   // standard constructor
	INT_PTR FillOriAra(CString FileName, Interface* pInterface, BOOL Select );
	INT_PTR SaveSke(CString FileName);
	void	ReadARA(CString FileName,Interface* pInterface, BOOL Select);
	virtual ~CSkeletalProp();

// Dialog Data
	enum { IDD = IDD_SKELETALPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	virtual INT_PTR DoModal( CString FileName, Interface* pInterface, BOOL Select,BOOL IsExist );
    afx_msg void OnBnClickedOk();
	BOOL m_RootScale;
	BOOL m_IsExpAllFrame;
	BOOL m_IsCpSke;
};



class CAniGroupProp : public CDialog
{
	DECLARE_DYNAMIC(CAniGroupProp)

	CPluginModel                  m_Model;
	SAVEPARAM               m_SaveParam;

	vector< ListNode* >		m_NodeType;
	Interface*              m_pInterface;
	BOOL                    m_Select;

	CString					m_AniName;
	string					m_ResPath;
public:
	CAniGroupProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAniGroupProp();

	INT_PTR FillData(CString FileName, Interface* pInterface, BOOL Select );
	INT_PTR SaveAgp(CString FileName);
	BOOL	PointSke();
	BOOL	ReadAgp(CString FileName);
	BOOL	ReadSke(CString FileName);
	BACKTYPE	IsSaveSke(CString FileName);
	INT_PTR SaveSke(CString FileName);
	BOOL	HasResPath();


	// Dialog Data
	enum { IDD = IDD_AGPPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	virtual INT_PTR DoModal( CString FileName, Interface* pInterface, BOOL Select,BOOL IsExist );
	afx_msg void OnBnClickedOk();
	BOOL m_RootScale;
	BOOL m_IsExpAllFrame;
	BOOL m_IsCpSke;
};

//// CSoftBodyProp dialog
//
//class CSoftBodyProp : public CDialog
//{
//	DECLARE_DYNAMIC(CSoftBodyProp)
//
//	CModel                  m_Model;
//	SAVEPARAM               m_SaveParam;
//
//	vector< ListNode* >		m_NodeType;
//	Interface*              m_pInterface;
//	BOOL                    m_Select;
//
//public:
//	CSoftBodyProp(CWnd* pParent = NULL);   // standard constructor
//	virtual ~CSoftBodyProp();
//
//	// Dialog Data
//	enum { IDD = IDD_SOFTBODYPROP };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//
//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//protected:
//	virtual void OnOK();
//public:
//	virtual INT_PTR DoModal( CString FileName, Interface* pInterface, BOOL Select );
//};

#endif
