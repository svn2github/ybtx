//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_GRAPHICS_GUI__
#define __BLUELY_GRAPHICS_GUI__

#include "BluelyG.h"
#include "input.h"

namespace bg
{
namespace ui
{

// message type
enum EUI_INPUT_MSG_TYPE
{
	EUIMT_NONE,
	EUIMT_KB_KEYDOWN, 
	EUIMT_KB_KEYUP,   
	EUIMT_KB_CHAR,    
	EUIMT_MS_BTNDOWN, 
	EUIMT_MS_BTNUP,   
	EUIMT_MS_MOVE ,   
	EUIMT_MS_WHEEL    
};

// style
#define BWS_DEBUG       0x00000001 
#define BWS_VISIBLE     0x00000002 
#define BWS_DISABLE     0x00000004 
#define BWS_FILLBKG	    0x00000008 
#define BWS_POPUP	    0X00000010 
#define BWS_DRAG	    0x00000020 
#define BWS_SIZE	    0x00000040 // sizeable
#define BWS_TOPMOST	    0x00000080 
#define BWS_OUTLINE     0x00000100 
#define BWS_SHADOW      0x00000200 

// color
// #define BWC_BACK		0x80bbc5ff
#define BWC_BACK		0x80bbc5ff
#define BWC_TEXT		0xffffffff
#define BWC_LOW			0xb0000000
#define BWC_MIDDLE		0xb0808080
#define BWC_HIGH		0xb0ff0000
#define BWC_RED			0xb0ff0000
#define BWC_GREEN		0xb000ff00
#define BWC_BLUE		0xb00000ff
#define BWC_BLACK		0xb0000000
#define BWC_WHITE		0xb0ffffff

// attribute
#define BWA_FRAMESIZE   0x00000003 

// hit
#define BWH_OUTSIDE   0x00000000 
#define BWH_INSIDE    0x00000001 
#define BWH_INTERSECT 0x00000002 

enum EUI_POPUPWINDOW_STATE { EUPS_NONE,EUPS_BEGIN,EUPS_RUNNING,EUPS_END};

struct SWndInfo
{
	char szName[256];	// name									
	BOOL bActive;			
	BOOL bDrag;			
	WORD wSize;			
	DWORD dwStyle;		
	RECT rcRect;		

	D3DCOLOR colorBack;    				
	D3DCOLOR colorText;	

	D3DCOLOR colorLow;	
	D3DCOLOR colorMiddle;								
	D3DCOLOR colorHigh;						
};

class CUIManager;

class CWnd  
{
	friend CUIManager;
public:
	
	SWndInfo m_wndInfo;
	CWnd * m_pParent;										
	std::list<CWnd *> m_listChild;

protected:
	void AddChild(CWnd *pWnd);
	void RemoveChild(CWnd *pWnd);

public:
	void ShowWnd(BOOL bShow);								
	virtual	void MoveWnd(int iOX,int iOY);
	virtual void SizeWnd(int iCX,int iCY);
	BOOL IsActive() { return m_wndInfo.bActive;}
	DWORD GetStyle() { return m_wndInfo.dwStyle;}	
	void  SetStyle(DWORD dwStyle) { m_wndInfo.dwStyle = dwStyle;}
	int  PtInRect(const POINT *pPoint);
	virtual void HitTest(const POINT * pPoint,CWnd ** ppWnd);
	
	virtual void SetWndRect(RECT *pRect);
	virtual void GetWndRect(RECT *pRect);

	CWnd * GetParent() { return m_pParent;}

	
	virtual BOOL Create(const char * pszName,const RECT * pRect,DWORD dwStyle,CWnd * pParent);
	virtual void Destroy();

	virtual void Update(ip::CInputDevice::SEvent & event);
	virtual void Render();	

	void UpdateDefault();
	void UpdateChild(ip::CInputDevice::SEvent & event);

	void RenderDefault();
	void RenderChild();

	CWnd();
	virtual ~CWnd();

	void MoveWndTo(int iX, int iY);
	void CenterWndTo(int iX, int iY);

	void SetBkColor(D3DCOLOR dwColor) { m_wndInfo.colorBack = dwColor;}
	void SetTextColor(D3DCOLOR dwColor) { m_wndInfo.colorBack = dwColor;}	

	void ModifyStyle(DWORD dwAdd, DWORD dwRemove);
	
	const char * GetName() const { return m_wndInfo.szName;}
};

////////////////////////////////////////////////////////////////////////////
//// button 
////////////////////////////////////////////////////////////////////////////
//
//enum EUI_BUTTON_STATE { EUBS_NORMAL,EUBS_DOWN,EUBS_OVER };
//
//class CButton :	public CWnd
//{
//protected:
//	EUI_BUTTON_STATE m_eBtnState;
//	bool m_bClicked;
//	DWORD m_dwTextureHandle;
//public:
//	bool IsClicked();
//
//	virtual void Render();
//	virtual void Update();
//
//	void SetTextureHandle(DWORD dwTextureHandle) { assert(dwTextureHandle!=INVALID); m_dwTextureHandle = dwTextureHandle;}
//	DWORD GetTextureHandle() { return m_dwTextureHandle; }
//
//	CButton();
//	~CButton();
//};
//
//////////////////////////////////////////////////////////////////////////
// static 
//////////////////////////////////////////////////////////////////////////

class CStatic :	public CWnd
{
public:
	virtual void Render();
	virtual void Update();

	CStatic(void){};
	~CStatic(void){};
};

////////////////////////////////////////////////////////////////////////////
//// menu
////////////////////////////////////////////////////////////////////////////
//
//#define MENUBAR_MAX_ITEM_TEXT 32
//#define MENUBAR_MAX_ITEM	  32
//#define MENUBAR_MAX_MENU_TEXT 32
//#define MENUBAR_MAX_MENU	  32
//#define POPUPMENU_MAX_ITEM    32 
//#define POPUPMENU_MAX_TEXT    64
//
//enum EUI_MENUBAR_STATE { EUMS_NORMAL,EUMS_POPUP};
//
//#define MBS_DISABLE       0x0001
//#define MBS_CHECKED		  0x0002
//#define MBS_ITEM_SPACEBAR 0x8001
//
////class CMenuBar : public CWnd
////{
////	EUI_MENUBAR_STATE m_eMenuBarState;
////	DWORD m_dwHoverMenu;
////public:
////	class CMenu 
////	{
//		struct SItem
//		{
//			DWORD m_dwStyle;
//			char szText[MENUBAR_MAX_ITEM_TEXT];
//			RECT rc;
//		public:
//			SItem():m_dwStyle(0){ rc.left = rc.top = rc.right = rc.bottom = 0;}
//			void Enable() { m_dwStyle &= ~MBS_DISABLE;}
//		};
//
//		SItem m_ItemArray[MENUBAR_MAX_ITEM];
//		DWORD m_dwItemCnt;
//
//		BOOL  m_bClicked;
//
//		DWORD m_dwHoverItem;
//		DWORD m_dwSelItem;
//
//	public:
//
//		DWORD m_dwStyle;
//		char  m_szText[MENUBAR_MAX_MENU_TEXT];
//
//		RECT m_rcBtn;
//
//		int  m_iDropWidth,m_iDropHeight; // drop down size
//
//		CMenu():m_dwStyle(0),m_dwItemCnt(0),m_bClicked(0),m_dwHoverItem(0xffffffff),m_dwSelItem(0xffffffff) {m_rcBtn.left = 
//			m_rcBtn.top = m_rcBtn.right = m_rcBtn.bottom = m_iDropWidth = m_iDropHeight = 0;}
//
//		const char * GetItemText(DWORD dwItem);
//
//		DWORD GetItemCnt() const { return m_dwItemCnt;}
//
//		void  Enable()  { m_dwStyle &= ~MBS_DISABLE;}
//		void  Disable() { m_dwStyle |= MBS_DISABLE;}
//
//		void  EnableItem(DWORD dwItem);
//		void  DisableItem(DWORD dwItem);
//
//		void  SetCheckItem(DWORD dwItem,BOOL bCheck);
//		BOOL  IsChecked(DWORD dwItem);
//
//		DWORD AddItem(const char * pszText);
//
//		void Render(void);
//		void Update(void);
//
//		DWORD HitTestItem(POINT * pPoint);
//
//		void MoveMenu(int iOX,int iOY);
//		BOOL PtInRect(const POINT * pPoint);
//
//		BOOL IsClicked();
//		DWORD GetSelItem() const { return m_dwSelItem;}
//
//		const char * GetText() const { return m_szText;}
//
//		void ClearItem();
//	};
//
//private:
//	CMenu m_MenuArray[MENUBAR_MAX_MENU];
//	DWORD m_dwMenuCnt;
//
//	DWORD m_dwSelMenu;
//	DWORD m_dwSelMenuItem;
//	BOOL  m_bSelected;
//
//	DWORD HitTestMenu(POINT * pPoint);
//
//public:
//	CMenuBar(void);
//	~CMenuBar(void);
//
//	virtual void MoveWnd(int iOX,int iOY);
//	virtual void SizeWnd(int iCX,int iCY);
//	virtual void HitTest(const POINT * pPoint, CWnd ** ppWnd);
//
//	virtual void Render(void);
//	virtual void Update(void);
//
//	DWORD AddMenu(const char * szText);
//
//	BOOL  TestSelMenu(DWORD &dwMenu,DWORD &dwItem);
//
//	void OpenPopupMenu();
//	void ClosePopupMenu();
//
//	DWORD AddMenuItem(DWORD dwMenu, const char * pszText);
//
//	void DisableMenu(DWORD dwMenu);
//	void EnableMenu(DWORD dwMenu);
//
//	void DisableMenuItem(DWORD dwMenu,DWORD dwItem);
//	void EnableMenuItem(DWORD dwMenu,DWORD dwItem);
//
//	void SetCheckMenuItem(DWORD dwMenu,DWORD dwItem,BOOL bCheck);
//
//	void ClearMenuItem(DWORD dwMenu);
//	
//
//	CMenu * GetMenu(DWORD dwMenu);
//};
//
//////////////////////////////////////////////////////////////////////////////
////// pie menu
////////////////////////////////////////////////////////////////////////////
//
//class CPieMenu : public CWnd
//{
//	enum EUI_PIEMENU_STATE { EUPS_NORMAL, EUPS_HOVER};
//
//	class CItem : public CWnd
//	{
//		EUI_PIEMENU_STATE m_eState;	
//	
//	public:
//		CItem() 
//		{ 
//			m_eState = EUPS_NORMAL;
//		}
//		virtual void Render();
//		virtual void Update();
//
//		BOOL LineIntersect(POINT ptLineA[2],POINT ptLineB[2]);
//		BOOL LineRectIntersect(POINT ptLine[2]);
//		BOOL CheckHover(void);
//	};
//
//	std::vector<CItem *> m_vItemArray;
//
//	DWORD m_dwRadius;
//
//public:
//	CPieMenu(void) { m_dwRadius = 64; };
//	virtual ~CPieMenu() {};
//
//	BOOL Create(const char * pszName, const RECT * pRECT, DWORD dwStyle, CWnd * pParent);
//	virtual void Destroy();
//
//	virtual void Render();
//	virtual void Update(ip::CInputDevice::SEvent & event);	
//
//	DWORD AddItem(const char * pszName,const RECT * pRect);
//
//	BOOL TestSelMenu(DWORD &dwItem);						
//	void BeginPopUp();										
//	void EndPopUp();											
//	void SetRadius(DWORD dwRadius);							
//};

//////////////////////////////////////////////////////////////////////////
// checkbox
//////////////////////////////////////////////////////////////////////////

class CCheckBox : public CWnd
{
	DWORD m_dwTextureHandle;
	bool  m_bChecked;
	bool m_bClicked;
	static int ms_iIDMother;
	static int ms_iHotID;
	int m_iID;
	bool m_bHover;
public:
	CCheckBox();
	~CCheckBox();

	static int GetHotID() { return ms_iHotID; }
	static void SetHotID(int i) { ms_iHotID = i; }

	bool IsClicked();
	void SetChecked(bool bFlag = true);
	void SetTextureHandle(DWORD dwTextureHandle);
	// void SetTextureRect(RECT & rc) { m_rc = rc;}
	// RECT GetTextureRect() { return m_rc;}
	DWORD GetTextureHandle() { return m_dwTextureHandle; }

	virtual void Render();
	virtual void Update(ip::CInputDevice::SEvent & event);
};

//////////////////////////////////////////////////////////////////////////
// manager
//////////////////////////////////////////////////////////////////////////

class CUIManager
{
public:
	static CUIManager * GetInstance() { if ( !one ) one = new CUIManager; return one;} 

private:
	CUIManager();
	virtual ~CUIManager() { SAFE_DELETE(one);};
	static CUIManager * one;

	CWnd * m_pMainWnd;
	CWnd * m_pActiveWnd;

	struct SPopupWindowState
	{
		EUI_POPUPWINDOW_STATE m_ePopupWindowState;
		CWnd * pTargetWnd;
		CWnd * pOldActive;
	} m_PopupWindowState;

	bool m_bEdit;

public:

	CWnd * GetActiveWindow() { return m_pActiveWnd;}
	CWnd * GetPopupWindow()  { return m_PopupWindowState.pTargetWnd;}

	void InactiveWindow(CWnd * pWnd);

	CWnd * GetMainWnd() const { return m_pMainWnd;}

//	SInputInfo * GetInputInfoBuffer() { return  &m_InputInfoBuffer;}

	void Render();																
	void Update(ip::CInputDevice::SEvent & event);																

	void BeginPopupWnd(CWnd * pWnd);												
	void EndPopupWnd();															
	
	void ActiveWindow(CWnd * pWnd);												

	BOOL Create();																		
	void Destroy(void);

	void SetMainWnd(CWnd *pWnd);												
	void ModifyStyle(CWnd * pWnd,DWORD dwAdd,DWORD dwRemove);
};

} 
}

#endif