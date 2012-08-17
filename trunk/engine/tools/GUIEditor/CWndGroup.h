#pragma once
#include "afxcmn.h"
#include ".\ceditcommand.h"
#include "resource.h"
#include "TreeCtrlEx.h"
#include "GUIXmlResSaver.h"

// CWndGroup form view
class CGUIEditorDlg;
class CWndImageProp;

namespace sqr
{
	class CGraphic;
}

class CWndProp;
class CWndStyle;
// 为了防止出现BUG做的一个修改
struct ItemNode
{	
	// 数据是否已经使用过一次
	BOOL		isUser;
	
	// 保存树子节点的句柄
	HTREEITEM	hItem;	

	// 树形列的名字	
	wchar_t		szBuffer[64];
};

#define EDIT_POSITION 1
#define EDIT_TEXTURE  2

#define Align_Left			0
#define Align_Right			1
#define Align_Top			2
#define Align_Bottom		3
#define Align_Width			4
#define Align_Height		5
#define Align_Count			6

struct CreateInfo
{
	wstring szClassName;
	wstring szCreateInfo;
	CString szFileName;  //add by lx    --窗口对应的文件
};

class CWndGroup 
	: public CFormView
	, public GUIXmlResSaverClass
{
	CGUIEditorDlg*	m_pMainDlg;
	CWndProp*		m_PropWnd;
	CWndStyle*		m_StyleWnd;
	CString			path;
	HTREEITEM       ChoosehItem;
	BOOL			bFocus;
	CWndImageProp*  m_pImageProp;

public:
	enum { IDD = IDD_WND_GROUP };
	CWndGroup( CGUIEditorDlg* pMainDlg );           // protected constructor used by dynamic creation
	virtual ~CWndGroup();

	bool			InitCfg();
	void			addMoveChildWnd( SQRWnd *pWnd );
	void			findResWnd( SQRWnd *pWnd, HTREEITEM hItem );
	void			OnShow( UINT uMessage, WPARAM wParam, LPARAM lParam );
	void			DrawEditLine( CGraphic* pGraphic, int n );
	void			CreateChild();
	void			SaveItem( ofstream& File, HTREEITEM hItem );
	void			SaveItemUtf8( ofstream& File, HTREEITEM hItem );
	HTREEITEM		ImportItem_03( ifstream& File, HTREEITEM hItem, SQRWnd* pParent );
	HTREEITEM		ImportItem_04( ifstream& File, HTREEITEM hItem, SQRWnd* pParent, CString FileName = L""); 

	SQRWnd*			GetEditWnd(){ return m_pCurEditWnd; }
	void			SetPropWnd( CWndProp* pProp, CWndStyle* pStyle ){ m_PropWnd = pProp; m_StyleWnd = pStyle; };
	void			UpdateToPropWnd( unsigned nFlag );
	void			UpdateItemToProp();
	void			HideCurShowWin();
	void			Check();
	void			ReCheck();
	void			CtrlC();
	void			CtrlV();
	void			CtrlD();
	void			InitWnd( SQRWnd* pWnd );
	BOOL			isHold(wchar_t *name);
	BOOL			isUser(wchar_t *name);
	void			MoveWnd();
	CGUIEditorDlg*	GetRootWnd() { return m_pMainDlg; }
	void            SetImageProp(CWndImageProp *imageProp){ m_pImageProp = imageProp; };
	void			WinListCreateFromRes();

	virtual BOOL	Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
					const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL	DestroyWindow();

	CRect			CurWndRect(int n);
	void			PushSelectedWnd(SQRWnd* wnd);
	void			ClearSelectedWnd();
	vector<SQRWnd*>*GetSelectedWndList();
	/**
	\brief
		设置当前语言
	*/	
	void			SetCurLanguage( int32 iCurLan );		
protected:
	virtual void PostNcDestroy(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnMessageEditPos( UINT uMessage, WPARAM wParam, LPARAM lParam );
	virtual void OnMessageEditTex( UINT uMessage, WPARAM wParam, LPARAM lParam );
	virtual void OnMessagePreview( UINT uMessage, WPARAM wParam, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrlEx		m_WndTree;
	CImageList		m_ImageList;
	HTREEITEM		m_hRoot;
	CString			m_strFile;	
	HTREEITEM       m_RememberItem;

	WORD			m_CursorID;
	WORD			m_EditID;

	CFPos			m_HitPos;
	CFPos			m_CurPos;

	SQRWnd*		m_pCurEditWnd;
	SQRWnd*		m_pCurMainWnd;
	SQRWnd*		m_pPrevEditWnd;
	SQRWnd*		m_pPrevMainWnd;
	
	//对树形列表的保存
	typedef vector<ItemNode> TreeItemList;
	TreeItemList m_TreeItemList;

	//显示窗口的列表
	vector<SQRWnd* >			mShowWndlist; 
	//显示窗口的列表
	vector<SQRWnd* >			mChooseWndlist; 
	map<SQRWnd*, CreateInfo>	m_CreateInfo;
	//按ctrl+left mouse选中的窗体
	vector<SQRWnd*>				m_SelectedWnd;

	//保存所有的文件名队列
	vector<CString> m_FileNameList;			//add by lx
	//保存临时删除的文件名的队列
	vector<CString> m_DelFileNameList;		//add by lx

	CEditCommand	m_ComandList;
	string			m_PreString;
	string			m_CurString;

	afx_msg void OnBnClickedArrow()		  { m_CursorID = IDC_EDITOR_ARROW;		    }
	afx_msg void OnBnClickedWndMove()     { m_CursorID = IDC_EDITOR_WNDMOVE;        }
	afx_msg void OnBnClickedClock()       { m_CursorID = IDC_EDITOR_CLOCK;          }
	afx_msg void OnBnClickedButton()	  { m_CursorID = IDC_EDITOR_BUTTON;	        }
	afx_msg void OnBnClickedCheckbutton() { m_CursorID = IDC_EDITOR_CHECKBUTTON;	}
	afx_msg void OnBnClickedCombobox()	  { m_CursorID = IDC_EDITOR_COMBOBOX;	  	}
	afx_msg void OnBnClickedEdit()		  { m_CursorID = IDC_EDITOR_EDIT;			}
	afx_msg void OnBnClickedRichwnd()	  { m_CursorID = IDC_EDITOR_RICHWND;	 	}
	afx_msg void OnBnClickedListctrl()	  { m_CursorID = IDC_EDITOR_LISTCTRL;		}
	afx_msg void OnBnClickedProgress()	  { m_CursorID = IDC_EDITOR_PROGRESS;		}
	afx_msg void OnBnClickedScrollbar()   { m_CursorID = IDC_EDITOR_SCROLLBAR;	    }
	afx_msg void OnBnClickedSpin()		  { m_CursorID = IDC_EDITOR_SPIN;			}
	afx_msg void OnBnClickedStatic()	  { m_CursorID = IDC_EDITOR_STATIC;	        }
	afx_msg void OnBnClickedTreectrl()	  { m_CursorID = IDC_EDITOR_TREECTRL;		}
	afx_msg void OnBnClickedChart()		  { m_CursorID = IDC_EDITOR_CHART;			}
	afx_msg void OnBnClickedAicon()       { m_CursorID = IDC_EDITOR_AICON;			}
	afx_msg void OnBnClickedVideo()       { m_CursorID = IDC_EDITOR_VIDEO;			}
	afx_msg void OnBnClickedFlashcontrol(){ m_CursorID = IDC_EDITOR_FLASHCONTROL;	}
	afx_msg void OnBnClickedIEcontrol()		{ m_CursorID = IDC_EDITOR_IECONTROL;	}

	afx_msg void OnBnClickedAlignLeft();
	afx_msg void OnBnClickedAlignRight();
	afx_msg void OnBnClickedAlignTop();
	afx_msg void OnBnClickedAlignBottom();
	afx_msg void OnBnClickedAlignWidth();
	afx_msg void OnBnClickedAlignHeight();

	afx_msg void OnTvnKeydownWndTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedWndTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditWndTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSaveAs();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedSaveCoder();

	afx_msg void OnNMClickWndTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusWndTree(NMHDR *pNMHDR, LRESULT *pResult);

private:
	void			reloadStaticTexts();
	void			reloadDialogStaticText( const ItemNode& treeItemNode );
	void			WriteTextItem(ofstream& file, string& idx, string& txt);
	
	wstring	 m_strRootPath;
	wstring  m_strAucPath;
	int		 m_nAufVersion;
	bool	 m_bResLoaded;
	bool	 m_Mark_Resize;

	// XML Correlation
	wstring					m_strLayoutPath;
	CButton					m_AlignButton[Align_Count];
public:
	afx_msg void OnBnClickedIecontrol();
};
