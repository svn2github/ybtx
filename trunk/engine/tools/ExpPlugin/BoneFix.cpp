// BoneFix.cpp : implementation file
//
#include "stdafx.h"
#include "Exp.h"
#include "ExpPlugin.h"
#include "BoneFix.h"
#include ".\bonefix.h"
#include <sstream>
#include <string>

// CBoneFix dialog

IMPLEMENT_DYNAMIC(CBoneFix, CDialog)
CBoneFix::CBoneFix(CWnd* pParent /*=NULL*/)
	: CDialog(CBoneFix::IDD, pParent)
{
}

CBoneFix::~CBoneFix()
{
}

void CBoneFix::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BONE_RADIO1, m_BoneRadio1);
	DDX_Control(pDX, IDC_BONE_RADIO2, m_BoneRadio2);
	DDX_Control(pDX, IDC_BONE_CHECK3, m_BoneCheck);
}


BEGIN_MESSAGE_MAP(CBoneFix, CDialog)
	ON_BN_CLICKED(IDC_BONE_RADIO1, OnBnClickedBoneRadio1)
	ON_BN_CLICKED(IDC_BONE_RADIO2, OnBnClickedBoneRadio2)
	ON_BN_CLICKED(IDC_BONE_CHECK3, OnBnClickedBoneCheck3)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CBoneFix message handlers

void CBoneFix::OnBnClickedBoneRadio1()
{
	// TODO: Add your control notification handler code here
	m_BoneRadio2.SetCheck( BST_UNCHECKED );
}

void CBoneFix::OnBnClickedBoneRadio2()
{
	// TODO: Add your control notification handler code here
	m_BoneRadio1.SetCheck( BST_UNCHECKED );
}

void CBoneFix::OnBnClickedBoneCheck3()
{
	// TODO: Add your control notification handler code here
	if( m_BoneCheck )
		HaveBoneFix = true;
	else
		HaveBoneFix = false;
}


void CBoneFix::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	BoneFixMap.clear();
	if ( HaveBoneFix )
	{
		std::string BoneParentNameL, BoneParentNameR;
		std::string BoneL, BoneR;
		std::stringstream ss;    TCHAR t[512];

		GetDlgItemText( IDC_EDIT_BONE_PARENT, t, 255 ); ss.clear(); ss.str( t );
		BoneParentNameL = ss.str().c_str();
		GetDlgItemText( IDC_EDIT_BONE_PARENT2, t, 255 ); ss.clear(); ss.str( t );
		BoneParentNameR = ss.str().c_str();

		if( this->m_BoneRadio1.GetCheck() == BST_CHECKED ) 
		{
			GetDlgItemText( IDC_EDIT_BONE_MALE, t, 255 ); ss.clear(); ss.str( t );
			BoneL = ss.str().c_str();
			GetDlgItemText( IDC_EDIT_BONE_MALE2, t, 255 ); ss.clear(); ss.str( t );
			BoneR = ss.str().c_str();
		} else
		{
			GetDlgItemText( IDC_EDIT_BONE_FEMALE, t, 255 ); ss.clear(); ss.str( t );
			BoneL = ss.str().c_str();
			GetDlgItemText( IDC_EDIT_BONE_FEMALE2, t, 255 ); ss.clear(); ss.str( t );
			BoneR = ss.str().c_str();
		}
		std::string temp;
		for( int i = 0; i < BoneL.size(); ++i)
		{
			if( BoneL[i] == '[')
			{
				temp = "";
			} else if( BoneL[i] == ']' )
			{
				BoneFixMap[temp] = BoneParentNameL;
			} else
			{
				temp += BoneL[i];
			}
		}
		temp = "";
		for( int i = 0; i < BoneR.size(); ++i)
		{
			if( BoneR[i] == '[')
			{
				temp = "";
			} else if( BoneR[i] == ']' )
			{
				BoneFixMap[temp] = BoneParentNameR;
			} else
			{
				temp += BoneR[i];
			}
		}
	}

	OnOK();
}

BOOL CBoneFix::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	HaveBoneFix = false;
	SetDlgItemText( IDC_EDIT_BONE_MALE,	   "[Bone08][Bone09][Bone10][Bone11][Bone12]" ); 
	SetDlgItemText( IDC_EDIT_BONE_MALE2,   "[Bone01][Bone02][Bone03][Bone04][Bone05][Bone06][Bone07]" ); 
	SetDlgItemText( IDC_EDIT_BONE_FEMALE,  "" );
	SetDlgItemText( IDC_EDIT_BONE_FEMALE2, "" );
	SetDlgItemText( IDC_EDIT_BONE_PARENT,  "Bip01 L Hand" );
	SetDlgItemText( IDC_EDIT_BONE_PARENT2, "Bip01 R Hand" );
	this->m_BoneRadio1.SetCheck( BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
