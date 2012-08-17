// SoftClothDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Exp.h"
#include "ExpPlugin.h"
#include "SoftClothDlg.h"
#include ".\softclothdlg.h"
#include <string>
#include <iostream>
#include <sstream>

// CSoftClothDlg dialog

IMPLEMENT_DYNAMIC(CSoftClothDlg, CDialog)
CSoftClothDlg::CSoftClothDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftClothDlg::IDD, pParent)
{
}

CSoftClothDlg::~CSoftClothDlg()
{
}

void CSoftClothDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSoftClothDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSoftClothDlg message handlers

void CSoftClothDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    std::stringstream ss;    TCHAR t[255];
    int ti; float tf;

    GetDlgItemText( IDC_CLOTHPROP_WINDPASS, t, 255 );    ss.clear(); ss.str( t );
    ss >> tf; Exp->TimePass = tf;

    GetDlgItemText( IDC_CLOTHPROP_WINDMIN, t, 255 );    ss.clear(); ss.str( t );
    ss >> ti; Exp->WindMin = ti;

    GetDlgItemText( IDC_CLOTHPROP_WINDMAX, t, 255 );    ss.clear(); ss.str( t );
    ss >> ti; Exp->WindMax = ti;

	GetDlgItemText( IDC_CLOTHPROP_WINDMAXH, t, 255 );    ss.clear(); ss.str( t );
	ss >> ti; Exp->WindMaxH = ti;

    GetDlgItemText( IDC_CLOTHPROP_WIDTH, t, 255 );    ss.clear(); ss.str( t );
    ss >> ti; Exp->Width = ti;

    GetDlgItemText( IDC_CLOTHPROP_HEIGHT, t, 255 );    ss.clear();ss.str( t );
    ss >> ti; Exp->Height = ti;

    GetDlgItemText( IDC_CLOTHPROP_GRAVITY, t, 255 );    ss.clear();ss.str( t );
    ss >> tf; Exp->Gravity = tf;

    GetDlgItemText( IDC_CLOTHPROP_GASSFORCE, t, 255 );  ss.clear();ss.str( t );
    ss >> tf; Exp->GassForce = tf;

    GetDlgItemText( IDC_CLOTHPROP_COEFFICIENTK, t, 255 );   ss.clear(); ss.str( t );
    ss >> tf; Exp->CoefficientK = tf;

    GetDlgItemText( IDC_CLOTHPROP_LONGRATIO, t, 255 );   ss.clear(); ss.str( t );
    ss >> tf; Exp->LongRatio = tf;

    GetDlgItemText( IDC_CLOTHPROP_SHORTRATIO, t, 255 );  ss.clear();  ss.str( t );
    ss >> tf; Exp->ShortRatio = tf;

    GetDlgItemText( IDC_CLOTHPROP_MASSA, t, 255 );   ss.clear(); ss.str( t );
    ss >> tf; Exp->MassA = tf * 100.0f;

    GetDlgItemText( IDC_CLOTHPROP_MASSB, t, 255 );   ss.clear(); ss.str( t );
    ss >> tf; Exp->MassB = tf * 100.0f;

    GetDlgItemText( IDC_CLOTHPROP_MASSC, t, 255 );   ss.clear(); ss.str( t );
    ss >> tf; Exp->MassC = tf * 100.0f;

	Exp->HengXiang = ((CButton*)GetDlgItem( IDC_CHECK1 ))->GetCheck();
    OnOK();
}

INT_PTR CSoftClothDlg::DoModal( SoftClothExpProp * Exp)
{
    // TODO: Add your specialized code here and/or call the base class
    this->Exp = Exp;
    return CDialog::DoModal();
}

void CSoftClothDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    SetWindowText( (string("Èí²¼ ") + Exp->MaxNode->GetName()).c_str() );

    SetDlgItemText( IDC_CLOTHPROP_WIDTH,        "10" );
    SetDlgItemText( IDC_CLOTHPROP_HEIGHT,       "10" );
    SetDlgItemText( IDC_CLOTHPROP_GRAVITY,      "100.0" );
    SetDlgItemText( IDC_CLOTHPROP_GASSFORCE,    "5.0" );
    SetDlgItemText( IDC_CLOTHPROP_COEFFICIENTK, "800.0" );
    SetDlgItemText( IDC_CLOTHPROP_LONGRATIO,    "1.02" );
    SetDlgItemText( IDC_CLOTHPROP_SHORTRATIO,   "0.6" );
    SetDlgItemText( IDC_CLOTHPROP_MASSA,        "8.0" );
    SetDlgItemText( IDC_CLOTHPROP_MASSB,        "10.0" );
    SetDlgItemText( IDC_CLOTHPROP_MASSC,        "13.5" );
    SetDlgItemText( IDC_CLOTHPROP_WINDMIN,      "300" );
    SetDlgItemText( IDC_CLOTHPROP_WINDMAX,      "1200" );
	SetDlgItemText( IDC_CLOTHPROP_WINDMAXH,      "100" );
    SetDlgItemText( IDC_CLOTHPROP_WINDPASS,     "0.15" );


    // TODO: Add your message handler code here
}
