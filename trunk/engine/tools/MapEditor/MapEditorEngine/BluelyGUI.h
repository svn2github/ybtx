#pragma once
#include "CEventOwner.h"
#include "MEGraphic.h"

namespace sqr
{
	class ITexture;
	class CRenderTexture;

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
	#define BWC_LOW			0x60444444
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

	struct SWndInfo
	{
		TCHAR szName[256];	// name
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

	class CBlyWnd
	{
	public:

		SWndInfo m_wndInfo;
		CBlyWnd * m_pParent;
		list<CBlyWnd *> m_listChild;

		void AddChild(CBlyWnd *pWnd);
		void RemoveChild(CBlyWnd *pWnd);

	public:
		void ShowWnd(BOOL bShow);
		virtual	void MoveWnd(int iOX,int iOY);
		virtual void SizeWnd(int iCX,int iCY);
		BOOL IsActive();
		DWORD GetStyle();
		void  SetStyle(DWORD dwStyle);
		int  PtInRect(const POINT *pPoint);
		virtual void HitTest(const POINT * pPoint,CBlyWnd ** ppWnd);

		virtual void SetWndRect(RECT *pRect);
		virtual void GetWndRect(RECT *pRect);

		CBlyWnd * GetParent()
		{
			return m_pParent;
		}


		virtual BOOL Create(const TCHAR * pszName,const RECT * pRect,DWORD dwStyle,CBlyWnd * pParent);
		virtual void Destroy();

		virtual void Update(const CMsg& msg);
		virtual void Render();

		void UpdateDefault();
		void UpdateChild(const CMsg& msg);

		void RenderDefault();
		void RenderChild();

		CBlyWnd();
		virtual ~CBlyWnd();

		void MoveWndTo(int iX, int iY);
		void CenterWndTo(int iX, int iY);

		void SetBkColor(D3DCOLOR dwColor);
		void SetTextColor(D3DCOLOR dwColor);

		void ModifyStyle(DWORD dwAdd, DWORD dwRemove);

		const TCHAR * GetName() const
		{
			return m_wndInfo.szName;
		}

		void HitOffToCenter(int & x, int & y, const POINT * pPoint);
	};

	//用于模型纹理预览
	class CRTTTextureBox : public CBlyWnd
	{
	public:
		CRTTTextureBox();
		~CRTTTextureBox();

		virtual BOOL Create(const TCHAR * pszName,const RECT * pRect,DWORD dwStyle,CBlyWnd *pParent);
		virtual void Render();
		void DrawRTTTexture();
	private:
		CRenderTexture *m_pITexture;
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// checkbox
	//////////////////////////////////////////////////////////////////////////

	class CCheckBox : public CBlyWnd
	{
		vector<int> m_vecTextureHandles;
		vector<string> m_vecTextureNames;
		ITexture *m_pTex;
		bool  m_bChecked;
		bool m_bClicked;
		static int ms_iIDMother;
		static int ms_iHotID;
		int m_iID;
		bool m_bHover;
	public:
		CCheckBox();
		~CCheckBox();

		static int GetHotID()
		{
			return ms_iHotID;
		}
		static void SetHotID(int i)
		{
			ms_iHotID = i;
		}

		bool IsClicked();
		void SetChecked(bool bFlag = true);
		void SetTextureName(vector<string> & vTextureNames);
		vector<string> & GetTextureName();

		virtual void Render();
		virtual void Update(const CMsg& msg);
	};

	//////////////////////////////////////////////////////////////////////////
	//用于模型纹理预览
	class CTitleTextureBox : public CBlyWnd
	{
		string m_strTextureName;
	public:
		CTitleTextureBox() : m_strTextureName("")
		{
			;
		}
		~CTitleTextureBox()
		{
			;
		}

		void SetTextureName(string & name);
		string GetTextureName();
		virtual void Render();
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// MiniMap
	//////////////////////////////////////////////////////////////////////////

	class CMiniMap : public CBlyWnd
	{
	public:
		enum
		{
			COLOR_ADD_BLOCK = 0xffff0000,
			COLOR_ERASE_BLOCK = BWC_BACK,
			COLOR_ADD_BLOCK_LOGIC = BWC_BACK
		};

	private:
		struct SMiniBlock
		{
			SMiniBlock()
			{
				::memset(clrBranch,0x00,sizeof(D3DCOLOR)*MINIMAP_BLOCK_GRID_SIZE*MINIMAP_BLOCK_GRID_SIZE);
				nDirtyCnt = 0;
				rcSrc.left = rcSrc.top = 0;
				rcSrc.right = rcSrc.bottom = MINIMAP_BLOCK_GRID_SIZE;
				dwSrcPitch = MINIMAP_BLOCK_GRID_SIZE*sizeof(DWORD);
			}

			D3DCOLOR clrBranch[MINIMAP_BLOCK_GRID_SIZE*MINIMAP_BLOCK_GRID_SIZE];
			int nDirtyCnt;
			RECT rcDest;
			RECT rcSrc;
			DWORD dwSrcPitch;
		};

		SMiniBlock m_clrDB[MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_SIZE];
		DWORD m_dwBlockIndexActive;
		list<SMiniBlock*> m_lstDirties;
		RECT m_rcPVS;
		POINT m_ptCenter;

		ITexture *m_pITexture;
		DWORD *m_pBitmap;

		void RegisterAsDirty(DWORD dwMiniBlockIndex);

	public:
		CMiniMap() : m_dwBlockIndexActive(INVALID), m_pITexture(NULL), m_pBitmap(NULL) {}
		~CMiniMap()
		{
			if (m_pBitmap)
			{
				delete []m_pBitmap;
				m_pBitmap = NULL;
			}
		}

		void UpdatePixel(DWORD dwMiniBlockIndex, DWORD dwGridIndexInMiniBlock, DWORD dwNewColor);
		void RefreshMiniMap( int nMode);
		virtual void Render();
		void Finalize();
		virtual void Update(const CMsg& msg);
		BOOL Create(const TCHAR *pszName,const RECT *pRect,DWORD dwStyle,CBlyWnd *pParent);
		void NavigateTo(DWORD x, DWORD y);
		void Init();
	};
}
