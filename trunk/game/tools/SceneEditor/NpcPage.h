#pragma once
#include "afxcmn.h"
#include "Resource.h"
#include "PlaceElement.h"


// CNpcPage dialog

enum ENpcPageType
{
	eNPT_NORMAL,
	eNPT_BOSS,
	eNPT_SERVANT,

	eNPT_COUNT
};

class CNpcPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcPage)

public:
	CNpcPage();
	virtual ~CNpcPage();

// Dialog Data
	enum { IDD = IDD_NPC_PAGE };


	bool Init();
	bool loadData();
	void UpdateSelNpc(int iIndex);
	PPlaceNpc GetSelectNpc();
	CListCtrl* GetListCtrl(ENpcPageType type);
	void ChangeType(ENpcPageType type, int iColonyId);

	void CreateSelPath(const CString& pathName);
private:
	void UpdateAIComboBox(CString sAIType);
	void UpdateMoveComboBox(CString sAIType);
	void SetAi(CString sAIType);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()

	
private:
	PlaceNpc		m_SelNpc;
	CListCtrl		m_listCtrlNormalInfo;
	CListCtrl		m_listCtrlBossInfo;
	CListCtrl		m_listCtrlServantInfo;

	CListCtrl*		m_pCurListCtrl;
	
	CString			m_strName;
	
	ENpcPageType	m_eCurType;
	bool			m_bIsLoading;
	
	int	m_iMoveRange;
	int m_iDirection;
	int m_iColonyId;
	CString m_strFindNpcName;
	int m_iMinDelayTime;
	int m_iMaxDelayTime;

	CComboBox m_ComboMoveState;//行走方式
	CComboBox m_ComboAISet;
	CComboBox m_ComboMoveType;//移动方式
	//CComboBox m_ComboColony;
	CComboBox m_ComboIdleAct;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDefaultai();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeAiset();
	afx_msg void OnClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedInsert();
	afx_msg void OnBnClickedIgnoreClog();
};
