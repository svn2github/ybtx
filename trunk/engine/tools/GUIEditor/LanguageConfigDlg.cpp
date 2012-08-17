// FontConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "LanguageConfigDlg.h"
#include "CLanguageConfigEdit.h"

// CLanguageConfigDlg dialog

IMPLEMENT_DYNAMIC(CLanguageConfigDlg, CDialog)

CLanguageConfigDlg::CLanguageConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanguageConfigDlg::IDD, pParent)
{

}

CLanguageConfigDlg::~CLanguageConfigDlg()
{
}

void CLanguageConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_combLanguageList);
	DDX_Control(pDX, IDC_FONTLIST, m_combFontList);
	DDX_Control(pDX, IDC_LANGUAGE_NAME, m_editLanguage);
	DDX_Control(pDX, IDC_FONT_NAME, m_editFontName);
	DDX_Control(pDX, IDC_FONT_SIZE, m_editFontSize);
	DDX_Control(pDX, IDC_FONT_FILE, m_editFontFile);
}


BEGIN_MESSAGE_MAP(CLanguageConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_LANGUAGE, &CLanguageConfigDlg::OnBnClickedAddLanguage)
	ON_BN_CLICKED(IDC_ADDFONT, &CLanguageConfigDlg::OnBnClickedAddFont)
	ON_BN_CLICKED(ID_GENERATE_LANGUAGE_CONFIG, &CLanguageConfigDlg::OnBnClickedGenerateConfigFile)
	ON_BN_CLICKED(ID_CANCEL_CONFIG_LANGUAGE, &CLanguageConfigDlg::OnBnClickedCancelConfigLanguage)
	ON_CBN_SELCHANGE(IDC_LANGUAGE_LIST, &CLanguageConfigDlg::OnCbnSelChangeLanguageList)
END_MESSAGE_MAP()


// CLanguageConfigDlg message handlers

void CLanguageConfigDlg::OnBnClickedAddLanguage()
{
	CString	strLanguage;
	m_editLanguage.GetWindowText(strLanguage);
	if (strLanguage.GetLength() == 0)
	{
		MessageBox(L"请输入语言名称");
		return;
	}
	
	addLanguage(utf16_to_mbcs((LPCWSTR)strLanguage));

	m_editLanguage.SetWindowText(L"");
	m_combFontList.ResetContent();
}

void CLanguageConfigDlg::OnBnClickedAddFont()
{
	int32 nLanIndex = m_combLanguageList.GetCurSel();
	if ( nLanIndex == -1 )
	{
		MessageBox(L"请先选择一种语言。");
		return;
	}

	CString strFontFile;
	CString strFontName;
	CString strFontSize;
	m_editFontFile.GetWindowText(strFontFile);
	if ( strFontFile.GetLength() == 0 )
	{
		MessageBox(L"请输入字体文件名");
		return;
	}
	m_editFontName.GetWindowText(strFontName);
	if ( strFontName.GetLength() == 0 )
	{
		MessageBox(L"请输入字体名称");
		return;
	}
	m_editFontSize.GetWindowText(strFontSize);
	if ( strFontSize.GetLength() == 0 )
	{
		MessageBox(L"请输入字体大小");
		return;
	}

	addFont(nLanIndex, utf16_to_utf8((LPCWSTR)strFontFile), utf16_to_utf8((LPCWSTR)strFontName), utf16_to_utf8((LPCWSTR)strFontSize));

	m_editFontFile.SetWindowText(L"");
	m_editFontName.SetWindowText(L"");
	m_editFontSize.SetWindowText(L"");
}

void CLanguageConfigDlg::addLanguage( const string &szLanguage )
{
	if ( !sqr::CLanguageConfigEdit::GetEditInst()->AddLanguage(szLanguage) )
	{
		MessageBox( (utf8_to_utf16(szLanguage) + L"这个语言名称已经存在，请输入其他名称。").c_str() );
		return;
	}

	combAddLanguage(szLanguage);
}

void CLanguageConfigDlg::addFont(int32 nLanIndex, const string &szFontFile, 
								 const string &szFontName, const string &szFontSize)
{
	if ( !sqr::CLanguageConfigEdit::GetEditInst()->AddFont(nLanIndex, szFontFile, szFontName, szFontSize) )
	{
		MessageBox(L"添加字体失败，请修改字体参数再试。");
		return;
	}

	updateControl();
}

void CLanguageConfigDlg::combAddLanguage( const string &szLanguage )
{
	m_combLanguageList.SetCurSel(m_combLanguageList.AddString(utf8_to_utf16(szLanguage).c_str()));
}

void CLanguageConfigDlg::combAddFont(int32 id, const string &szFontName, const string &szFontFile)
{
	wstringstream ss;
	ss << id;
	m_combFontList.SetCurSel(m_combFontList.AddString((ss.str() + L" - " + utf8_to_utf16(szFontName) + L" - " + utf8_to_utf16(szFontName)).c_str()) );
}

void CLanguageConfigDlg::OnBnClickedGenerateConfigFile()
{
	if ( !sqr::CLanguageConfigEdit::GetEditInst()->GenerateConfigFile() )
	{
		MessageBox(L"还没有进行过字体配置,生成失败,请进行配置后再试。");
		return;
	}
	
	OnOK();
}

void CLanguageConfigDlg::OnBnClickedCancelConfigLanguage()
{
	OnCancel();
}

BOOL CLanguageConfigDlg::OnInitDialog()
{
	if ( !CDialog::OnInitDialog())
		return FALSE;

	// 若已经存在字体配置文件，则从字体配置文件读取配置并在对话框中添加对应信息
	if ( CLanguageConfig::GetInst()->LoadConfig() )
	{
		loadConfigFile();
	}

	return TRUE;
}

INT_PTR CLanguageConfigDlg::DoModal()
{
	INT_PTR re = CDialog::DoModal();
	
	return IDOK;
}

void CLanguageConfigDlg::loadConfigFile()
{
	for ( int32 nLanguageIndex = 0; nLanguageIndex < CLanguageConfig::GetInst()->GetLanCount(); ++nLanguageIndex )
	{
		string szLanguage = CLanguageConfig::GetInst()->GetLanName(nLanguageIndex);
		combAddLanguage(szLanguage);
	}

	updateControl();
}

void CLanguageConfigDlg::OnCbnSelChangeLanguageList()
{
	updateControl();
}

void CLanguageConfigDlg::updateControl()
{
	// 更新字体列表框
	int32 nLanIndex = m_combLanguageList.GetCurSel();

	m_combFontList.ResetContent();

	CLanguageConfig::GDIFontInfoVector  fontMap;
	CLanguageConfig::GetInst()->GetGDIFontInfoVector(nLanIndex, fontMap);
	CLanguageConfig::GDIFontInfoVector::iterator it = fontMap.begin();
	int32 index = 0;
	for ( ; it != fontMap.end(); ++it )
	{
		string szFont	  = utf16_to_mbcs(it->first);
		string szFontFile = it->second;
		combAddFont(index++, szFontFile, szFont);
	}
}	