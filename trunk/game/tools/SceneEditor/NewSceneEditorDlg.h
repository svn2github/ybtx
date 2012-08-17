// NewSceneEditorDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ElementTabPage.h"
#include "ModeChoiceDlg.h"
#include "MapView.h"
#include "AreaEditDlg.h"
#include "../../engine/sdk/inc/StringHelper.h"

class CColonyEditDlg;

// CNewSceneEditorDlg 对话框
class CNewSceneEditorDlg : public CDialog
{
// 构造
public:
	friend class CAreaEditDlg;


	CNewSceneEditorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NEWSCENEEDITOR_DIALOG };
	void ShowScalingCtrl(int isShow);
	void UpdateInfoBox();//更新消息框
	void UpdateShowBox();

	void ClearArea();

	COLORREF GetCurAreaColor();
	bool SetAreaColor(const CString& areaName, COLORREF color);
	bool AreaRename(const CString& oldName, const CString& newName);
	bool DelArea(const CString& areaName);
	int GetPenSize();
	bool SaveArea(string savePath);
    bool SaveTransArea(string savePath);
	void SaveAreaInfo(FILE* pFile);
	
	void SelectArea(const CString& name);

	CString GetMapPicturePath(const CString& sceneName);
	CString GetCurSceneName();

	void SetCurColonyEditDlg(CColonyEditDlg* pDlg);
	CColonyEditDlg* GetCurColonyEditDlg();

	bool LoadScene(const CString& mpathName);
	bool LoadArea(const CString& mpathName);
	void ConvertToUtf8(TCHAR* m_szSave);

	bool IsLeapCheck;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


private:
		CMenu				m_mainMenu;
		CMapView			m_mapView;
		CColonyEditDlg*		m_pCurColonyEditDlg;

		CComboBox			m_ComboAreaName;
		
		CSliderCtrl			m_SliderScaling;
		CTabCtrl			m_tabElement;
		CElementTabPage		m_tabPageElement[eTPT_COUNT]; //0:npc, 1:obj, 2:Trap, 3:群组

		CSliderCtrl			m_SliderPenSize;
		CSliderCtrl			m_SliderTransparence;

		CComboBox			m_ComboBarrierObj;
		CString				m_strCurSceneName;
		std::vector<CString> m_vecMusicNames;
		std::vector<CString> m_vecAreaTemp;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLoadMap();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLoadScene();
	afx_msg void OnLoadArea();
	afx_msg void OnNMCustomdrawViewScaling(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnResetView();

	void ShowElementTabPage(ETabPageType item);
	void InitMusic();
	CString GetBarrierObjName();
	

	void OnAddElement(CElement* pElement, ETabPageType type);
	void OnDelElement(CElement* pElement, ETabPageType type);
	void OnUpdateElement(CElement* pElement, ETabPageType type);
	void OnClearPage(ETabPageType type);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedShowNpc();
	afx_msg void OnBnClickedShowPath();
	afx_msg void OnBnClickedShowEyeshot();
	afx_msg void OnBnClickedShowObj();
	afx_msg void OnBnClickedShowTrap();
	afx_msg void OnBnClickedShowArrow();
	afx_msg void OnBnClickedSetCenterPos();

	afx_msg void OnSaveScene();
	afx_msg void OnSaveArea();
    afx_msg void OnSaveTransArea();
	afx_msg void OnBnClickedShowNpcFull();
	afx_msg void OnTcnSelchangeElementTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedShowGameView();
	afx_msg void OnBnClickedShowFollowMouse();
	afx_msg void OnBnClickedEditArea();
	afx_msg void OnBnClickedDoNotOver();

	afx_msg void OnBnClickedShowArea();
	afx_msg void OnNMCustomdrawTransparence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawPenSize(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedSetColor();
	afx_msg void OnCbnEditchangeAreaName();
	afx_msg void OnCbnSelchangeAreaName();
	afx_msg void OnBnClickedShowBlock();
	afx_msg void OnBnClickedIgnoreBlock();
	afx_msg void OnBnClickedFindInBarrier();
	afx_msg void OnBnClickedAreaMusicEdit();
	afx_msg void OnBnClickedMoverMap();
	afx_msg void OnBnClickedSetGridMiddle();
	afx_msg void OnSetOffsetPosition();
	afx_msg void OnReSaveAllSceneFile();
	afx_msg void OnReSaveAllAreaFile();
	afx_msg void OnReSaveAllDynamicFile();
	afx_msg void OnBnClickedRuler();
	afx_msg void OnBnClickedRotate();
};
