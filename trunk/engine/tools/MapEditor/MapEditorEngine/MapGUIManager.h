#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"

namespace sqr
{
	//////////////////////////////////////////////////////////////////////////
	// manager
	//////////////////////////////////////////////////////////////////////////
	class CBlyWnd;
	class CMiniMap;
	class CTitleTextureBox;
	class CRTTTextureBox;

	class CMapGUIManager : public Singleton<CMapGUIManager>
	{
	private:
		CBlyWnd*			m_pMainWnd;
		CBlyWnd*			m_pActiveWnd;

		CMiniMap*			m_pMiniMap;
		CTitleTextureBox*	m_pTitleTextureBox;

		enum EUI_POPUPWINDOW_STATE { EUPS_NONE,EUPS_BEGIN,EUPS_RUNNING,EUPS_END};

		struct SPopupWindowState
		{
			EUI_POPUPWINDOW_STATE m_ePopupWindowState;
			CBlyWnd * pTargetWnd;
			CBlyWnd * pOldActive;
		} m_PopupWindowState;

	public:

		CMapGUIManager();
		~CMapGUIManager();

		void CreateMapGUI();

		CBlyWnd * GetActiveWindow()
		{
			return m_pActiveWnd;
		}
		CBlyWnd * GetPopupWindow()
		{
			return m_PopupWindowState.pTargetWnd;
		}

		void InactiveWindow(CBlyWnd * pWnd);

		// SInputInfo * GetInputInfoBuffer() { return  &m_InputInfoBuffer;}

		void Render();
		void Update(const CMsg& msg);

		void BeginPopupWnd(CBlyWnd * pWnd);
		void EndPopupWnd();

		void ActiveWindow(CBlyWnd * pWnd);

		CBlyWnd* GetMainWnd() const;
		void ModifyStyle(CBlyWnd * pWnd,DWORD dwAdd,DWORD dwRemove);

		void UpdateFPSLimited();
		void UpdatePixel(DWORD dwMiniBlockIndex, DWORD dwGridIndexInMiniBlock, DWORD dwNewColor);
		void Finalize();
		void InitMiniMap();
		void UpdateMiniMapPosition();
		void HideSceneMinMap();

		void SetTitleTextureVisibility(bool b);
		void SetTitleTextureName(string & name);

		void RefreshMiniMap( int nMode);

		inline CMiniMap* GetMiniMap()
		{
			return m_pMiniMap;
		}
	};
}
