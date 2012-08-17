#pragma once
#include "afxwin.h"

//这些配置可以考虑放到Lua中


// CAvatarView form view
class CPlayerModelPart;
namespace sqr
{
	class CRenderObject;
}

namespace sqr_tools
{
	class CEditModel;
}

class CAvatarView : public CFormView
{
	DECLARE_DYNCREATE(CAvatarView)
public:
	enum ECharType
	{
		eRLM = 0,	//人类男
		eRLW = 1,	//人类女
		eSRM = 2,	//兽人男
		eSRW = 3,	//兽人女
		eJLM = 4,	//精灵男
		eJLW = 5,	//精灵女
		eARM = 6,	//矮人男
		eARW = 7,	//矮人女
		eOTHER = 8,	//其他
		eCharTypeCount,
	};

	struct PieceItem
	{
		string				m_Name;
		string				m_PiecePath;
		CPlayerModelPart*	m_pPartInfo;
	};

	typedef vector<PieceItem>	ItemList;
	struct SLoadInf
	{
		ECharType	m_Type;
		string		m_TypePath;
		string		m_Name;

		ItemList	m_Head;
		ItemList	m_Face;
		ItemList	m_Hair;
		ItemList	m_Shoulder;
		ItemList	m_Body;
		ItemList	m_Mantle;
		ItemList	m_Cuff;
		ItemList	m_Boot;
		ItemList	m_MainWeapon;
		ItemList	m_SubWeapon;
	};
	typedef map<CComboBox*,int> MapItemGroup;
	typedef vector<MapItemGroup> VecAllGroup;
	typedef map<string,VecAllGroup> MapGroupEquip;
public:
	CAvatarView();           // protected constructor used by dynamic creation
	virtual ~CAvatarView();

public:
	enum { IDD = IDD_AVATAR_TOOL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void PostNcDestroy(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangePlayerType();

	afx_msg void OnCbnSelchangePlayerHead();
	afx_msg void OnCbnSelchangePlayerShoulder();
	afx_msg void OnCbnSelchangePlayerBody();
	afx_msg void OnCbnSelchangePlayerMantle();
	afx_msg void OnCbnSelchangePlayerCuff();
	afx_msg void OnCbnSelchangePlayerBoot();
	afx_msg void OnCbnSelchangePlayerMainWeapon();
	afx_msg void OnCbnSelchangePlayerSubWeapon();

	afx_msg void OnNMCustomdrawPlayerAniSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangePlayerAni();
	afx_msg void OnCbnSelchangePlayerAniGroup();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//
	//afx_msg void OnCbnSelchangeAdvHead();
	//afx_msg void OnCbnSelchangeAdvShoulder();
	//afx_msg void OnCbnSelchangeAdvBody();
	//afx_msg void OnCbnSelchangeAdvMantle();
	//afx_msg void OnCbnSelchangeAdvCuff();
	//afx_msg void OnCbnSelchangeAdvBoot();
	//afx_msg void OnCbnSelchangeAdvMainWeapon();
	//afx_msg void OnCbnSelchangeAdvSubWeapon();
protected:
	SLoadInf	m_LoadInf[eCharTypeCount];
	void		Initialize(void);
	void		UpdateComboBox( ItemList& il, CComboBox& cb );
	void		_UpdatePlayer( ItemList& il, CComboBox& cb, vector<string>& Hide );
	void		_UpdateEffect( ItemList& il, CComboBox& cb, CComboBox& cbe );
	void		_UpdateAdv( ItemList& il, CComboBox& cb,int Index );
	void		_UpdateAni(void);
	void		UpdatePlayer();
	void		CreateEquipGroup();
	void		UpdateEquipGroup(string& strName);
	CComboBox	m_PlayerType;
	CComboBox	m_PlayerAniGroup;
	CComboBox	m_PlayerAni;
	CSliderCtrl	m_PlayerAniSpeed;

	CComboBox	m_PlayerHead;
	CComboBox	m_PlayerFace;
	CComboBox	m_PlayerHair;
	CComboBox	m_PlayerShoulder;
	CComboBox	m_PlayerBody;
	CComboBox	m_PlayerMantle;
	CComboBox	m_PlayerCuff;
	CComboBox	m_PlayerBoot;
	CComboBox	m_MainWeapon;
	CComboBox	m_SubWeapon;

	CComboBox	m_Head_Adv;
	CComboBox	m_Shoulder_Adv;
	CComboBox	m_Body_Adv;
	CComboBox	m_Mantle_Adv;
	CComboBox	m_Cuff_Adv;
	CComboBox	m_Boot_Adv;
	CComboBox	m_MainWeapon_Adv;
	CComboBox	m_SubWeapon_Adv;
	CComboBox m_EquipGroup;

	CEditModel* m_pMainPlayer;

	MapGroupEquip m_MapGroupEquip;

//预览功能
protected:
	//CEditorView m_PreView;
	//CMiniScene	m_PreScene;
private:
	void DisplayModName();
public:
	afx_msg void OnCbnSelchangeEquipGroup();
	afx_msg void OnCbnSelchangeGroupAdv();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CEdit m_modEdit;
};


