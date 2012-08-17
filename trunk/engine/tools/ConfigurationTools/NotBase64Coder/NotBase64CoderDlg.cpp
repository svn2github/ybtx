// NotBase64CoderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NotBase64Coder.h"
#include "NotBase64CoderDlg.h"

#include "CodeCvs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef	unsigned __int8		uint8;

char RadixTable[65] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
'0','1','2','3','4','5','6','7','8','9','+','-','='
};

using namespace std;
//using namespace sqr;

std::string mbcs_to_MemH64(const std::string& src)
{
	if (src == "")
		return src;
	int length = static_cast<int>(src.length());
	int moreCount = length%3;
	int resultSize = length;
	if (moreCount)
		resultSize = (resultSize+3-moreCount)*4/3 + moreCount - 3;
	else
		resultSize = resultSize*4/3;
	string strResult;
	strResult.resize(resultSize);
	for (int i=0;i<length-moreCount;)
	{
		uint8 one =  static_cast<uint8>((src[i]&0xfc)>>2);
		uint8 two =  static_cast<uint8>((src[i]&0x03)<<4) +  static_cast<uint8>((src[i+1]&0xf0)>>4);
		uint8 three =  static_cast<uint8>((src[i+1]&0x0f)<<2) +  static_cast<uint8>((src[i+2]&0xc0)>>6);
		uint8 four =  static_cast<uint8>(src[i+2]&0x3f);
		strResult[(i/3)*4] = RadixTable[one];
		strResult[(i/3)*4+1] = RadixTable[two];
		strResult[(i/3)*4+2] = RadixTable[three];
		strResult[(i/3)*4+3] = RadixTable[four];
		i += 3;
	}
	if(moreCount==2)
	{
		uint8 one =  static_cast<uint8>((src[length-2]&0xfc)>>2);
		uint8 two =  static_cast<uint8>((src[length-2]&0x03)<<4) +  static_cast<uint8>((src[length-1]&0xf0)>>4);
		uint8 three =  static_cast<uint8>((src[length-1]&0x0f)<<2) +  static_cast<uint8>((0&0xc0)>>6);
		strResult[resultSize-3] = RadixTable[one];
		strResult[resultSize-2] = RadixTable[two];
		strResult[resultSize-1] = RadixTable[three];
	}
	else if(moreCount ==1)
	{
		uint8 one =  static_cast<uint8>((src[length-1]&0xfc)>>2);
		uint8 two =  static_cast<uint8>((src[length-1]&0x03)<<4) +  static_cast<uint8>((0&0xf0)>>4);
		strResult[resultSize-2] = RadixTable[one];
		strResult[resultSize-1] = RadixTable[two];
	}
	return strResult;
}

int GetRadix64CharPos(char testChar)
{
	if (testChar>='A' && testChar <= 'Z')
	{
		return testChar-'A';
	}
	else if (testChar>='a' && testChar <= 'z')
	{
		return 26+testChar-'a';
	} 
	else if (testChar>='0' && testChar <= '9')
	{
		return 52+testChar-'0';
	}
	else if (testChar == '+')
	{
		return 62;
	}else if (testChar == '-')
	{
		return 63;
	}
	else
	{
		return 64;				//返回64标识错误，所查找的字符串并非由mbcs_to_MemH64函数所生成
	}
}

std::string MemH64_to_mbcs(const std::string& src)
{
	if (src == "")
		return src;
	int length = static_cast<int>(src.length());
	int moreCount = length%4;
	int resultSize = length;
	if (moreCount)
	{
		resultSize = (resultSize+4-moreCount)*3/4 + moreCount - 4;
	}
	else
	{
		resultSize = resultSize*3/4;
	}
	string strResult;
	strResult.resize(resultSize);
	for (int i=0;i<length-moreCount;)
	{
		uint8 one   = GetRadix64CharPos(src[i]);
		uint8 two   = GetRadix64CharPos(src[i+1]);
		uint8 three = GetRadix64CharPos(src[i+2]);
		uint8 four   = GetRadix64CharPos(src[i+3]);
		strResult[i*3/4]		= static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
		strResult[i*3/4+1] = static_cast<uint8>((two&0x0f)<<4) + static_cast<uint8>((three&0x3c)>>2);
		strResult[i*3/4+2] = static_cast<uint8>((three&0x03)<<6) + static_cast<uint8>(four&0x3f);
		i += 4;
	}
	if(moreCount==2)
	{
		uint8 one = GetRadix64CharPos(src[length-2]);
		uint8 two = GetRadix64CharPos(src[length-1]);
		strResult[resultSize-1] = static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
	}
	else if(moreCount ==3)
	{
		uint8 one   = GetRadix64CharPos(src[length-3]);
		uint8 two   = GetRadix64CharPos(src[length-2]);
		uint8 three = GetRadix64CharPos(src[length-1]);
		strResult[resultSize-2] = static_cast<uint8>((one&0x3f)<<2) + static_cast<uint8>((two&0x30)>>4);
		strResult[resultSize-1] = static_cast<uint8>((two&0x0f)<<4) + static_cast<uint8>((three&0x3c)>>2);
	}
	return strResult;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNotBase64CoderDlg dialog




CNotBase64CoderDlg::CNotBase64CoderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNotBase64CoderDlg::IDD, pParent)
	, m_bConverting(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNotBase64CoderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNotBase64CoderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_TOMEM, &CNotBase64CoderDlg::OnEnChangeTomem)
	ON_BN_CLICKED(IDC_BTN_B2TCONV, &CNotBase64CoderDlg::OnBnClickedBtnB2tconv)
	ON_EN_CHANGE(IDC_DISP, &CNotBase64CoderDlg::OnEnChangeDisp)
	ON_EN_SETFOCUS(IDC_DISP, &CNotBase64CoderDlg::OnEnSetfocusDisp)
	ON_EN_SETFOCUS(IDC_TOMEM, &CNotBase64CoderDlg::OnEnSetfocusTomem)
END_MESSAGE_MAP()


// CNotBase64CoderDlg message handlers

BOOL CNotBase64CoderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNotBase64CoderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNotBase64CoderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNotBase64CoderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNotBase64CoderDlg::ConvTextToBase64()
{
	if(m_bConverting)
		return;
	m_bConverting = true;
	CString text;
	GetDlgItemText(IDC_TOMEM,text);
	string strMem = mbcs_to_MemH64(sqr::utf16_to_gbk(text.GetBuffer()));
	GetDlgItem(IDC_DISP)->SetWindowText(sqr::gbk_to_utf16(strMem.c_str()).c_str());
	//SetWindowText(IDC_TOMEM,strMem.c_str());
	//UpdateData(false);
	m_bConverting = false;
}

void CNotBase64CoderDlg::ConvBase64ToText()
{
	if(m_bConverting)
		return;
	m_bConverting = true;
	CString text;
	GetDlgItemText(IDC_DISP,text);
	string strMem = MemH64_to_mbcs(sqr::utf16_to_gbk(text.GetBuffer()));
	if(strMem == "" && text !="")
	{
		strMem = "error";
	}
	GetDlgItem(IDC_TOMEM)->SetWindowText(sqr::gbk_to_utf16(strMem.c_str()).c_str());
	//UpdateData(false);
	m_bConverting = false;
}

void CNotBase64CoderDlg::OnOK()
{
	ConvBase64ToText();
}

void CNotBase64CoderDlg::OnBnClickedBtnB2tconv()
{
	// TODO: Add your control notification handler code here
}

void CNotBase64CoderDlg::OnEnChangeTomem()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ConvTextToBase64();
}

void CNotBase64CoderDlg::OnEnChangeDisp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ConvBase64ToText();
}

void CNotBase64CoderDlg::OnEnSetfocusDisp()
{
	// TODO: Add your control notification handler code here
}

void CNotBase64CoderDlg::OnEnSetfocusTomem()
{
	// TODO: Add your control notification handler code here
}
