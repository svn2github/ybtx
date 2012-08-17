// AvatarView.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTypes.h"
#include "resource.h"
#include "AvatarView.h"
#include "CCharacter.h"
#include "CGraphic.h"
#include "GTRenderMgr.h"
#include "IEffectGroup.h"
#include "IEffectManager.h"
#include "CVector3.h"
#include "CRenderObject.h"
#include "CDataCache.h"
#include "ScriptMgr.h"
#include "CSkeletalsMgr.h"
#include "CEditModel.h"

// CAvatarView
#define	DEFAULT_ANI_PATH "character\\ani\\zhujue\\"

#define	INIT_LOADINF(type,path,name) m_LoadInf[type].m_Type = type; m_LoadInf[type].m_TypePath = path; m_LoadInf[type].m_Name = name;
IMPLEMENT_DYNCREATE(CAvatarView, CFormView)

CAvatarView::CAvatarView()
	: CFormView(CAvatarView::IDD)
	, m_pMainPlayer(NULL)
{
	//enum ECharType
	//{
	//	eRLM = 0,	//人类男
	//	eRLW = 1,	//人类女
	//	eSRM = 2,	//兽人男
	//	eSRW = 3,	//兽人女
	//	eJLM = 4,	//精灵男
	//	eJLW = 5,	//精灵女
	//	eARM = 6,	//矮人男
	//	eARV = 7,	//矮人女
	//	eOTHER = 8,	//其他
	//	eCount,
	//};
	INIT_LOADINF(eRLM,"rlm","人类男");
	INIT_LOADINF(eRLW,"rlw","人类女");
	INIT_LOADINF(eSRM,"srm","兽人男");
	INIT_LOADINF(eSRW,"srw","兽人女");
	INIT_LOADINF(eJLM,"jlm","精灵男");
	INIT_LOADINF(eJLW,"jlw","精灵女");
	INIT_LOADINF(eARM,"arm","矮人男");
	INIT_LOADINF(eARW,"arw","矮人女");
	INIT_LOADINF(eOTHER,"","其它");

	//Initialize();
}

CAvatarView::~CAvatarView()
{
}

void CAvatarView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYER_TYPE,	m_PlayerType);
	DDX_Control(pDX, IDC_PLAYER_ANI_GROUP,	m_PlayerAniGroup);
	DDX_Control(pDX, IDC_PLAYER_ANI_SPEED,	m_PlayerAniSpeed);
	DDX_Control(pDX, IDC_PLAYER_ANI,		m_PlayerAni);

	DDX_Control(pDX, IDC_PLAYER_HEAD,	m_PlayerHead);
	DDX_Control(pDX, IDC_PLAYER_FACE,	m_PlayerFace);
	DDX_Control(pDX, IDC_PLAYER_HAIR,	m_PlayerHair);
	DDX_Control(pDX, IDC_PLAYER_SHOULDER, m_PlayerShoulder);
	DDX_Control(pDX, IDC_PLAYER_BODY,	m_PlayerBody);
	DDX_Control(pDX, IDC_PLAYER_MANTLE, m_PlayerMantle);
	DDX_Control(pDX, IDC_PLAYER_CUFF,	m_PlayerCuff);
	DDX_Control(pDX, IDC_PLAYER_BOOT,	m_PlayerBoot);
	DDX_Control(pDX, IDC_MAIN_WEAPON,	m_MainWeapon);
	DDX_Control(pDX, IDC_SUB_WEAPON,	m_SubWeapon);

	DDX_Control(pDX, IDC_HEAD_ADV,			m_Head_Adv);
	DDX_Control(pDX, IDC_SHOULDER_ADV,		m_Shoulder_Adv);
	DDX_Control(pDX, IDC_BODY_ADV,			m_Body_Adv);
	DDX_Control(pDX, IDC_MANTLE_ADV,		m_Mantle_Adv);
	DDX_Control(pDX, IDC_CUFF_ADV,			m_Cuff_Adv);
	DDX_Control(pDX, IDC_BOOT_ADV,			m_Boot_Adv);
	DDX_Control(pDX, IDC_MAIN_WEAPON_ADV,	m_MainWeapon_Adv);
	DDX_Control(pDX, IDC_SUB_WEAPON_ADV,	m_SubWeapon_Adv);
	DDX_Control(pDX, IDC_EQUIP_GROUP, m_EquipGroup);
	DDX_Control(pDX, IDC_MODEDIT, m_modEdit);
}

BEGIN_MESSAGE_MAP(CAvatarView, CFormView)
	ON_CBN_SELCHANGE(IDC_PLAYER_TYPE, &CAvatarView::OnCbnSelchangePlayerType)
	ON_CBN_SELCHANGE(IDC_PLAYER_ANI, &CAvatarView::OnCbnSelchangePlayerAni)
	ON_CBN_SELCHANGE(IDC_PLAYER_ANI_GROUP, &CAvatarView::OnCbnSelchangePlayerAniGroup)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PLAYER_ANI_SPEED, &CAvatarView::OnNMCustomdrawPlayerAniSpeed)

	ON_CBN_SELCHANGE(IDC_PLAYER_HEAD,		&CAvatarView::OnCbnSelchangePlayerHead)
	ON_CBN_SELCHANGE(IDC_PLAYER_FACE,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_PLAYER_HAIR,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_PLAYER_SHOULDER,	&CAvatarView::OnCbnSelchangePlayerShoulder)
	ON_CBN_SELCHANGE(IDC_PLAYER_BODY,		&CAvatarView::OnCbnSelchangePlayerBody)
	ON_CBN_SELCHANGE(IDC_PLAYER_MANTLE,		&CAvatarView::OnCbnSelchangePlayerMantle)
	ON_CBN_SELCHANGE(IDC_PLAYER_CUFF,		&CAvatarView::OnCbnSelchangePlayerCuff)
	ON_CBN_SELCHANGE(IDC_PLAYER_BOOT,		&CAvatarView::OnCbnSelchangePlayerBoot)
	ON_CBN_SELCHANGE(IDC_MAIN_WEAPON,		&CAvatarView::OnCbnSelchangePlayerMainWeapon)
	ON_CBN_SELCHANGE(IDC_SUB_WEAPON,		&CAvatarView::OnCbnSelchangePlayerSubWeapon)

	ON_CBN_SELCHANGE(IDC_HEAD_ADV,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_SHOULDER_ADV,	&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_BODY_ADV,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_MANTLE_ADV,	&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_CUFF_ADV,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_BOOT_ADV,		&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_MAIN_WEAPON_ADV,	&CAvatarView::UpdatePlayer)
	ON_CBN_SELCHANGE(IDC_SUB_WEAPON_ADV,	&CAvatarView::UpdatePlayer)

	//ON_CBN_SELCHANGE(IDC_HEAD_ADV,		&CAvatarView::OnCbnSelchangeAdvHead)
	//ON_CBN_SELCHANGE(IDC_SHOULDER_ADV,	&CAvatarView::OnCbnSelchangeAdvShoulder)
	//ON_CBN_SELCHANGE(IDC_BODY_ADV,		&CAvatarView::OnCbnSelchangeAdvBody)
	//ON_CBN_SELCHANGE(IDC_MANTLE_ADV,	&CAvatarView::OnCbnSelchangeAdvMantle)
	//ON_CBN_SELCHANGE(IDC_CUFF_ADV,		&CAvatarView::OnCbnSelchangeAdvCuff)
	//ON_CBN_SELCHANGE(IDC_BOOT_ADV,		&CAvatarView::OnCbnSelchangeAdvBoot)
	//ON_CBN_SELCHANGE(IDC_MAIN_WEAPON_ADV,	&CAvatarView::OnCbnSelchangeAdvMainWeapon)
	//ON_CBN_SELCHANGE(IDC_SUB_WEAPON_ADV,	&CAvatarView::OnCbnSelchangeAdvSubWeapon)
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_EQUIP_GROUP, &CAvatarView::OnCbnSelchangeEquipGroup)
END_MESSAGE_MAP()


// CAvatarView diagnostics

#ifdef _DEBUG
void CAvatarView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAvatarView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CAvatarView message handlers
void CAvatarView::Initialize(void)
{
	const ModelPART_VEC& tModelPartVec = gs_pCahce->GetPlayerModelPart();
	for( uint i = 0; i<eCharTypeCount; ++i )
	{
		ModelPART_VEC::const_iterator it,eit = tModelPartVec.end();
		SLoadInf& Inf = m_LoadInf[i];
		for( it = tModelPartVec.begin(); it!=eit; ++it )
		{
			ItemList* pItem = NULL;
			CPlayerModelPart* pPart = *it;

			//if( (pPart->GetDynamic() == "否" && i != eOTHER) 
			//	|| (pPart->GetDynamic() != "否" && i == eOTHER) )
			//	continue;

			if( pPart->GetPieceName() == "armet" )
				pItem = &Inf.m_Head;
			else if( pPart->GetPieceName() == "face" )
				pItem = &Inf.m_Face;
			else if( pPart->GetPieceName() == "hair" )
				pItem = &Inf.m_Hair;
			else if( pPart->GetPieceName() == "shoulder" )
				pItem = &Inf.m_Shoulder;
			else if( pPart->GetPieceName() == "body" )
				pItem = &Inf.m_Body;
			else if( pPart->GetPieceName() == "back" )
				pItem = &Inf.m_Mantle;
			else if( pPart->GetPieceName() == "arm" )
				pItem = &Inf.m_Cuff;
			else if( pPart->GetPieceName() == "shoe" )
				pItem = &Inf.m_Boot;
			else if( pPart->GetPieceName() == "weapon" )
				pItem = &Inf.m_MainWeapon;
			else if( pPart->GetPieceName() == "offweapon" )
				pItem = &Inf.m_SubWeapon;

			if( pItem!=NULL )
			{
				/*string FileName = pPart->GetModelFileName();
				size_t off = FileName.find(",");

				if( off != size_t(-1) )
					FileName.replace(off,1,Inf.m_TypePath);*/
				string FileName = pPart->GetModelFileName(Inf.m_Name);
				CPieceGroup* pGroup = NULL;
				if( 0 == CGraphic::GetInst()->CreatePieceGroup(FileName.c_str(),&pGroup) )
				{
					PieceItem Item;
					Item.m_Name = pPart->GetName();
					Item.m_PiecePath = FileName;
					Item.m_pPartInfo = pPart;
					pItem->push_back(Item);
					pGroup->Release();
				}
			}			
		}
	}
}	

void CAvatarView::UpdateComboBox( ItemList& il, CComboBox& cb )
{
	ItemList::iterator it,eit = il.end();
	cb.ResetContent();
	cb.AddString("无");
	cb.SetCurSel(0);
	for( it = il.begin(); it!=eit; ++it )
		cb.AddString((*it).m_Name.c_str());
	cb.SetCurSel(1);
}

void CAvatarView::OnCbnSelchangePlayerType()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	//更新动作包
	const uint NumPart1 = 3;
	const uint NumPart2 = 3;

	string Part1[NumPart1] = { "_js","_qs", "_fs" };
	string Part2[NumPart2] = { "_ss","_ds", "_ks" };

	m_PlayerAniGroup.ResetContent();
	m_PlayerAni.ResetContent();	
	for( uint i = 0; i<NumPart1; ++i )
		for( uint j = 0; j<NumPart2; ++j )
		{
			string tAniName = Inf.m_TypePath + Part1[i] + Part2[j];
			string tGroupPath =  DEFAULT_ANI_PATH + tAniName + ".ske";
			//CAnimationsRes* pAniRes = NULL;
			string sAgpfile = tGroupPath.substr(0,tGroupPath.size()-4)+".agp";
			CSkeletalFrame* pSke = NULL;
			CAnimationGroup* pAniGroup = NULL;

			if(0 == CGraphic::GetInst()->CreateAnimationGroup(sAgpfile.c_str(),&pAniGroup)&& 0 == CSkeletalsMgr::GetInst()->GetSkeInPool(pAniGroup->GetSkeletalFrameName(),&pSke))
			{
				m_PlayerAniGroup.AddString(tAniName.c_str());
				pSke = NULL;
				pAniGroup->Release();
			}
		}
	m_PlayerAniGroup.SetCurSel(0);

	//更新装备
	UpdateComboBox(Inf.m_Head,m_PlayerHead);
	UpdateComboBox(Inf.m_Face,m_PlayerFace);
	UpdateComboBox(Inf.m_Hair,m_PlayerHair);
	UpdateComboBox(Inf.m_Shoulder,m_PlayerShoulder);
	UpdateComboBox(Inf.m_Body,m_PlayerBody);
	UpdateComboBox(Inf.m_Mantle,m_PlayerMantle);
	UpdateComboBox(Inf.m_Cuff,m_PlayerCuff);
	UpdateComboBox(Inf.m_Boot,m_PlayerBoot);
	UpdateComboBox(Inf.m_MainWeapon,m_MainWeapon);
	UpdateComboBox(Inf.m_SubWeapon,m_SubWeapon);

	_UpdateAdv(Inf.m_Head,m_Head_Adv, m_PlayerHead.GetCurSel() );
	_UpdateAdv(Inf.m_Shoulder,m_Shoulder_Adv, m_PlayerShoulder.GetCurSel() );
	_UpdateAdv(Inf.m_Body,m_Body_Adv, m_PlayerBody.GetCurSel() );
	_UpdateAdv(Inf.m_Mantle,m_Mantle_Adv, m_PlayerMantle.GetCurSel() );
	_UpdateAdv(Inf.m_Cuff,m_Cuff_Adv, m_PlayerCuff.GetCurSel() );
	_UpdateAdv(Inf.m_Boot,m_Boot_Adv, m_PlayerBoot.GetCurSel() );
	_UpdateAdv(Inf.m_MainWeapon,m_MainWeapon_Adv, m_MainWeapon.GetCurSel() );
	_UpdateAdv(Inf.m_SubWeapon,m_SubWeapon_Adv, m_SubWeapon.GetCurSel() );
	
	CreateEquipGroup();
	
	OnCbnSelchangePlayerAniGroup();
	UpdatePlayer();
}

void  CAvatarView::_UpdatePlayer( ItemList& il, CComboBox& cb, vector<string>& Hide )
{
	if( cb.GetCurSel() > 0 )
	{
		int idb = cb.GetCurSel();
		PieceItem& Item = il[cb.GetCurSel() - 1];
		m_pMainPlayer->AddPiece(RFT_ARP,Item.m_PiecePath.c_str(),Item.m_pPartInfo->GetPieceName().c_str(),Item.m_pPartInfo->GetRenderStyle().c_str());

		string HideString;
		switch(m_PlayerType.GetCurSel())
		{
		case eRLM:	//人类男
			HideString = Item.m_pPartInfo->GetHumanMaleHidePiece();
			break;
		case eRLW:	//人类女
			HideString = Item.m_pPartInfo->GetHumanFeMaleHidePiece();
			break;
		case eSRM:	//兽人男
			HideString = Item.m_pPartInfo->GetOrcHidePiece();
			break;
		case eSRW:	//兽人女
			break;
		case eJLM:	//精灵男
			HideString = Item.m_pPartInfo->GetElfHidePiece();
			break;
		case eJLW:	//精灵女
			break;
		case eARM:	//矮人男
			HideString = Item.m_pPartInfo->GetDwarfHidePiece();
			break;
		case eARW:	//矮人女
			break;
		case eOTHER://其他
			break;
		default:
			break;
		}
		size_t off = 0;
		if( HideString.size() > 0 )
		{
			while( off != size_t(-1) )
			{
				size_t tOff;
				tOff = HideString.find(",",off);
				string temp;
				if( tOff == size_t(-1) )
					temp = HideString.substr(off ,HideString.size()-off);
				else
				{
					temp = HideString.substr(off,tOff-off);
					++tOff;
				}
				if( temp.size() > 0 )
					Hide.push_back(temp);
				off = tOff;
			}
		}
	}
}

void CAvatarView::_UpdateAni(void)
{
	if(m_PlayerAniGroup.GetCurSel()==-1)
		return;
	CString tAniName;
	m_pMainPlayer->Delframework();
	m_PlayerAniGroup.GetLBText( m_PlayerAniGroup.GetCurSel(),tAniName);
	string tPath =  DEFAULT_ANI_PATH;
	tPath += tAniName.GetBuffer();
	tPath += ".ske";
	m_pMainPlayer->AddPiece( RFT_ARA, tPath.c_str(), NULL, NULL );
}

void  CAvatarView::UpdatePlayer()
{
	if( NULL == m_pMainPlayer )
		return;

	vector<string> HidePiece;
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	m_pMainPlayer->Release();
	_UpdateAni();

	_UpdatePlayer(Inf.m_Head,m_PlayerHead, HidePiece);
	_UpdatePlayer(Inf.m_Face,m_PlayerFace, HidePiece);
	_UpdatePlayer(Inf.m_Hair,m_PlayerHair, HidePiece);
	_UpdatePlayer(Inf.m_Shoulder,m_PlayerShoulder, HidePiece);
	_UpdatePlayer(Inf.m_Body,m_PlayerBody, HidePiece);
	_UpdatePlayer(Inf.m_Mantle,m_PlayerMantle, HidePiece);
	_UpdatePlayer(Inf.m_Cuff,m_PlayerCuff, HidePiece);
	_UpdatePlayer(Inf.m_Boot,m_PlayerBoot, HidePiece);
	_UpdatePlayer(Inf.m_MainWeapon,m_MainWeapon, HidePiece);
	_UpdatePlayer(Inf.m_SubWeapon,m_SubWeapon, HidePiece);

	OnCbnSelchangePlayerAni();

	_UpdateEffect(Inf.m_Head,		m_PlayerHead,		m_Head_Adv);
	_UpdateEffect(Inf.m_Shoulder,	m_PlayerShoulder,	m_Shoulder_Adv);
	_UpdateEffect(Inf.m_Body,		m_PlayerBody,		m_Body_Adv);
	_UpdateEffect(Inf.m_Mantle,		m_PlayerMantle,		m_Mantle_Adv);
	_UpdateEffect(Inf.m_Cuff,		m_PlayerCuff,		m_Cuff_Adv);
	_UpdateEffect(Inf.m_Boot,		m_PlayerBoot,		m_Boot_Adv);
	_UpdateEffect(Inf.m_MainWeapon,	m_MainWeapon,		m_MainWeapon_Adv);
	_UpdateEffect(Inf.m_SubWeapon,	m_SubWeapon,		m_SubWeapon_Adv);
	
	vector<string>::iterator it,eit = HidePiece.end();
	for( it = HidePiece.begin(); it!=eit; ++it )
		m_pMainPlayer->ShowPiece((*it).c_str(),false);
	DisplayModName();
}

void CAvatarView::_UpdateAdv( ItemList& il, CComboBox& cb,int Index)
{
	cb.ResetContent();
	cb.AddString("无");
	cb.SetCurSel(0);
	if( Index > 0 )
	{
		PieceItem& Item = il[Index - 1];
		string tPath;
		IEffectGroup* pEffectGroup = NULL;
		tPath = Item.m_pPartInfo->GetEffectFileName();
		if(tPath.size() == 0)
			return;

		string FileName = Item.m_pPartInfo->GetModelFileName();
		int Off = FileName.find_last_of("//");
		FileName = FileName.substr(Off+1,FileName.find_last_of(".") - Off);
		tPath += m_LoadInf[m_PlayerType.GetCurSel()].m_TypePath + "/" + FileName + "efx";

		IEffectManager::GetInst()->CreateEffectGroup(tPath.c_str(),&pEffectGroup);
		if( pEffectGroup != NULL )
		{
			for( uint32 i = 0; i< pEffectGroup->GetEffectNum(); ++i )
			{
				string temp = pEffectGroup->GetEffectName(i);
				if( temp.find(Item.m_pPartInfo->GetPieceName() + "\\") != size_t(-1) )
					cb.AddString(temp.c_str());
			}
			pEffectGroup->Release();
			cb.SetCurSel(1);
		}
	}	
	UpdatePlayer();
}

void CAvatarView::_UpdateEffect( ItemList& il, CComboBox& cb, CComboBox& cbe )
{
	if( cb.GetCurSel() > 0 && cbe.GetCurSel()> 0 )
	{
		SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
		PieceItem& Item = il[cb.GetCurSel() - 1];
		string tPath = Item.m_pPartInfo->GetEffectFileName();
		string FileName = Item.m_pPartInfo->GetModelFileName();
		int Off = FileName.find_last_of("//");
		FileName = FileName.substr(Off+1,FileName.find_last_of(".") - Off);
		tPath += m_LoadInf[m_PlayerType.GetCurSel()].m_TypePath + "/" + FileName + "efx";

		CString tString;
		cbe.GetLBText(cbe.GetCurSel(), tString);
		m_pMainPlayer->AddEffect(tPath.c_str(),tString.GetBuffer(),-1,NULL);
	}
}

void CAvatarView::OnCbnSelchangePlayerHead()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Head,m_Head_Adv, m_PlayerHead.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerShoulder()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Shoulder,m_Shoulder_Adv, m_PlayerShoulder.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerBody()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Body,m_Body_Adv, m_PlayerBody.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerMantle()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Mantle,m_Mantle_Adv, m_PlayerMantle.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerCuff()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Cuff,m_Cuff_Adv, m_PlayerCuff.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerBoot()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_Boot,m_Boot_Adv, m_PlayerBoot.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerMainWeapon()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_MainWeapon,m_MainWeapon_Adv, m_MainWeapon.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerSubWeapon()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	_UpdateAdv(Inf.m_SubWeapon,m_SubWeapon_Adv, m_SubWeapon.GetCurSel() );
	UpdatePlayer();
}

void CAvatarView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	
	if(bShow)
	{
		m_PlayerAniSpeed.SetRange( 0 ,  8, true );
		m_PlayerAniSpeed.SetPos(4);
		m_PlayerType.ResetContent();
		for(UINT i = 0; i<eCharTypeCount; ++i )
			m_PlayerType.AddString(m_LoadInf[i].m_Name.c_str());
		m_PlayerType.SetCurSel(0);
		
		if( CGTRenderMgr::HasInst() )
		{
			CGTRenderMgr::GetInst()->DestroyObj();
			if(m_pMainPlayer == NULL)
				m_pMainPlayer = CGTRenderMgr::GetInst()->m_MiniScene.CreateObject();
		}
		OnCbnSelchangePlayerType();
	}
	else
	{
		if( CGTRenderMgr::HasInst() && m_pMainPlayer!=NULL)
		{
			CGTRenderMgr::GetInst()->m_MiniScene.DestroyObject(m_pMainPlayer);
			m_pMainPlayer = NULL;
		}
	}
}
int CAvatarView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Initialize();
	return 0;
}

//------------------------------动作相关----------------------

void CAvatarView::OnCbnSelchangePlayerAniGroup()
{
	if(m_PlayerAniGroup.GetCurSel() <0 || m_pMainPlayer == NULL)
		return;
	CString tAniName;
	m_PlayerAniGroup.GetLBText( m_PlayerAniGroup.GetCurSel(),tAniName);
	string tPath =  DEFAULT_ANI_PATH;
	tPath += tAniName.GetBuffer();
	tPath += ".ske";
	//CAnimationsRes* pAniRes = NULL;
	CSkeletalFrame* pSke = NULL;
	CAnimationGroup* pAniGroup = NULL;

	string sAgpfile = tPath.substr(0,tPath.size()-4)+".agp";
	m_PlayerAni.ResetContent();	
	if( 0==CGraphic::GetInst()->CreateAnimationGroup(sAgpfile.c_str(),&pAniGroup)&&0 == CSkeletalsMgr::GetInst()->GetSkeInPool(pAniGroup->GetSkeletalFrameName(),&pSke) )
	{
		for(int i =0; i< pAniGroup->GetAnimationNum(); ++i )
		{
			const TCHAR* pChar = pAniGroup->GetAnimationName(i);
			m_PlayerAni.AddString( pChar );
		}
		pSke = NULL;
		pAniGroup->Release();
	}
	m_PlayerAni.SetCurSel(0);
	UpdatePlayer();
}

void CAvatarView::OnCbnSelchangePlayerAni()
{
	if(m_PlayerAni>=0 && m_PlayerAni.GetCurSel()!=-1)
	{
		CString tAniName;
		m_PlayerAni.GetLBText( m_PlayerAni.GetCurSel(),tAniName);
		m_pMainPlayer->DoAni(tAniName.GetBuffer(),-1,true,0,0,-1,float(m_PlayerAniSpeed.GetPos())/4.0f);
	}
}

void CAvatarView::OnNMCustomdrawPlayerAniSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	OnCbnSelchangePlayerAni();
	*pResult = 0;
}

void	CAvatarView::	CreateEquipGroup()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	ItemList il = Inf.m_Head;
	vector<string> vecGroup;
	for (ItemList::iterator iter = il.begin();iter != il.end();++iter)
	{
		string strEquipHead = (*iter).m_Name;
		int32 iLength = strEquipHead.length();
		string strGroup = strEquipHead.substr(0,iLength-4);
		vector<ItemList*> vecItemList;
		vecItemList.push_back(&Inf.m_Shoulder);
		vecItemList.push_back(&Inf.m_Body);
		vecItemList.push_back(&Inf.m_Mantle);
		vecItemList.push_back(&Inf.m_Cuff);
		vecItemList.push_back(&Inf.m_Boot);
		for (vector<ItemList*>::iterator iterVec = vecItemList.begin();iterVec != vecItemList.end();++iterVec)
		{
			bool bResult = false;
			for (ItemList::iterator iterIT = (*iterVec)->begin();iterIT != (*iterVec)->end();++iterIT)
			{
				string strTemp = (*iterIT).m_Name;
				int32 iLengthTemp = strTemp.length();
				if(iLengthTemp>4)
				{
					string strNameTemp = strTemp.substr(0,iLengthTemp-4);
					if (strNameTemp == strGroup)
					{
						vecGroup.push_back(strGroup);
						bResult = true;
						break;
					}
				}
			}
			if(bResult)
				break;
		}
	}
	m_EquipGroup.ResetContent();
	m_EquipGroup.AddString("无");
	m_EquipGroup.SetCurSel(0);
	for (vector<string>::iterator iter = vecGroup.begin();iter != vecGroup.end();++iter)
	{
		m_EquipGroup.AddString((*iter).c_str());
	}

}

void CAvatarView::OnCbnSelchangeEquipGroup()
{
	// TODO: Add your control notification handler code here
	CString strGroup;
	m_EquipGroup.GetLBText(m_EquipGroup.GetCurSel(),strGroup);
	string strName(strGroup.GetBuffer());
	UpdateEquipGroup(strName);
}

void	CAvatarView::	UpdateEquipGroup(string& strName)
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	vector<ItemList*> vecItemList;
	vecItemList.push_back(&Inf.m_Head);
	vecItemList.push_back(&Inf.m_Shoulder);
	vecItemList.push_back(&Inf.m_Body);
	vecItemList.push_back(&Inf.m_Mantle);
	vecItemList.push_back(&Inf.m_Cuff);
	vecItemList.push_back(&Inf.m_Boot);
	vector<CComboBox*> vecComboList;
	vecComboList.push_back(&m_PlayerHead);
	vecComboList.push_back(&m_PlayerShoulder);
	vecComboList.push_back(&m_PlayerBody);
	vecComboList.push_back(&m_PlayerMantle);
	vecComboList.push_back(&m_PlayerCuff);
	vecComboList.push_back(&m_PlayerBoot);
	vector<CComboBox*>::iterator iterCombo = vecComboList.begin();
	for (vector<ItemList*>::iterator iterVec = vecItemList.begin();iterVec != vecItemList.end();++iterVec,++iterCombo)
	{
		int i=0;
		bool bResult = false;
		for (ItemList::iterator iterIT = (*iterVec)->begin();iterIT != (*iterVec)->end();++iterIT,++i)
		{
			string strTemp = (*iterIT).m_Name;
			int32 iLengthTemp = strTemp.length();
			if(iLengthTemp>4)
			{
				string strNameTemp = strTemp.substr(0,iLengthTemp-4);
				if (strNameTemp == strName)
				{
					(*iterCombo)->SetCurSel(i+1);
					bResult = true;
					break;
				}
			}
		}
		if(!bResult)
			(*iterCombo)->SetCurSel(0);
	}
	OnCbnSelchangePlayerHead();
	OnCbnSelchangePlayerShoulder();
	OnCbnSelchangePlayerBody();
	OnCbnSelchangePlayerMantle();
	OnCbnSelchangePlayerCuff();
	OnCbnSelchangePlayerBoot();
}

void CAvatarView::DisplayModName()
{
	SLoadInf& Inf = m_LoadInf[m_PlayerType.GetCurSel()];
	string strBlank = "无";
	string headName = "头盔:" + (m_PlayerHead.GetCurSel() ? Inf.m_Head[m_PlayerHead.GetCurSel()-1].m_PiecePath : "");
	string faceName =	  "相貌:" + (m_PlayerFace.GetCurSel() ?  Inf.m_Face[m_PlayerFace.GetCurSel()-1].m_PiecePath: "");
	string hairName =	  "头发:" + (m_PlayerHair.GetCurSel()  ? Inf.m_Hair[m_PlayerHair.GetCurSel()-1].m_PiecePath: "");
	string shoulderName = "护肩:" + (m_PlayerShoulder.GetCurSel() ? Inf.m_Shoulder[m_PlayerShoulder.GetCurSel()-1].m_PiecePath :"");
	string bodyName = "身体:" + (m_PlayerBody.GetCurSel() ? Inf.m_Body[m_PlayerBody.GetCurSel()-1].m_PiecePath : "");
	string mantleName = "披风:" + (m_PlayerMantle.GetCurSel() ? Inf.m_Mantle[m_PlayerMantle.GetCurSel()-1].m_PiecePath : "");
	string cuffName = "护腕:" + (m_PlayerCuff.GetCurSel() ? Inf.m_Cuff[m_PlayerCuff.GetCurSel()-1].m_PiecePath : "");
	string bootName = "靴子:" + (m_PlayerBoot.GetCurSel() ? Inf.m_Boot[m_PlayerBoot.GetCurSel()-1].m_PiecePath : "");
	string weaponName = "主手:" + (m_MainWeapon.GetCurSel() ? Inf.m_MainWeapon[m_MainWeapon.GetCurSel()-1].m_PiecePath : "");
	string sWeaponName = "副手:" + (m_SubWeapon.GetCurSel() ? Inf.m_SubWeapon[m_SubWeapon.GetCurSel()-1].m_PiecePath : "");
	string strResult = headName + "\r\n" + faceName + "\r\n" + hairName + "\r\n"+ shoulderName + "\r\n"
				+ bodyName + "\r\n" + mantleName + "\r\n" + cuffName + "\r\n" + bootName + "\r\n" + weaponName + "\r\n"
				+ sWeaponName;
	GetDlgItem(IDC_MODEDIT)->SetWindowText(strResult.c_str());
	UpdateData(true);
}

void CAvatarView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	RECT rect;
	if (m_modEdit.m_hWnd)
	{	
		m_modEdit.GetWindowRect(&rect);
		int heigth = cy/6>100 ? cy:70;
		m_modEdit.SetWindowPos(NULL,0,0,rect.right-rect.left,heigth,SWP_NOZORDER|SWP_NOMOVE );
	}
}
