// MakeTxtXmlMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MakeTxtXmlMFC.h"
#include "MakeTxtXmlMFCDlg.h"
#include "FILE.h"
#include <string>
#include "COriFileOperate.h"
#include "tinyxml/tinyxml.h"
#include "CSplitInfo.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMakeTxtXmlMFCDlg 对话框




CMakeTxtXmlMFCDlg::CMakeTxtXmlMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeTxtXmlMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeTxtXmlMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeTxtXmlMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMakeTxtXmlMFCDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMakeTxtXmlMFCDlg 消息处理程序

BOOL CMakeTxtXmlMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	CSplitInfo::GetInst()->Init(0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMakeTxtXmlMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMakeTxtXmlMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CMakeTxtXmlMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMakeTxtXmlMFCDlg::OnBnClickedButton1()
{
	string CfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	CFILE cfile_t(true,0,0,OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_HIDEREADONLY,0,0);
	const int MIN_FILE_NUMBER = 20;
	cfile_t.m_ofn.lpstrFile = new TCHAR[_MAX_PATH*MIN_FILE_NUMBER];
	memset(cfile_t.m_ofn.lpstrFile,0,_MAX_PATH*MIN_FILE_NUMBER);
	cfile_t.m_ofn.nMaxFile = _MAX_PATH*MIN_FILE_NUMBER;

	if(IDOK==cfile_t.DoModal())
	{
		POSITION pos = cfile_t.GetStartPosition();
		while(NULL!=pos)
		{
			string strFilePath=cfile_t.GetNextPathName(pos);
			if(strFilePath!="")
			{
				if(strFilePath.find(CfgExtend.c_str())!=string::npos)
				{
					vector<bool> vec;
					GetColType(strFilePath,vec);
					string strXml = strFilePath.substr(0,strFilePath.size()-4);
					strXml.append(".xml");
					ForceDeleteFile(strXml);
					WriteToXml(strXml,vec,strFilePath);
					ostringstream ss;
					ss<<strFilePath<<"转换完毕，xml文件和源"<<CfgExtend<<"文件位于同一目录";
					MessageBox(ss.str().c_str());
				}
				else
				{
					ostringstream ss;
					ss<<strFilePath<<"文件类型错误，请重新选择，应该是"<<CfgExtend<<"文件";
					MessageBox(ss.str().c_str());
				}	
			}
		}
	}
	delete[] cfile_t.m_ofn.lpstrFile;
	// TODO: Add your control notification handler code here
}


void GetColType(string& strFileName,vector<bool>& vecType)
{
	COriFileOperate oriFile(strFileName);
	if (!CSplitInfo::GetInst()->GetBeKeepBlankLine(strFileName))
	{
		oriFile.CleanBlanRow();
		oriFile.CleanNoteRow();
	}
	size_t nRowNum = oriFile.GetRowCount();
	size_t nColNum = 0;
	nColNum = oriFile.GetColCountByRow(0);
	vecType.clear();
	for (size_t i=0;i<nColNum;++i)
	{
		vecType.push_back(true);
	}
	for (size_t i = 1; i < nRowNum; i++)
	{
		for (size_t j = 0; j < nColNum; j++)
		{
			if (!oriFile.BeNumber(i,j))
			{
				vecType[j] = false;
			}
		}
	}
}

void WriteToXml(string& strXml, vector<bool>& vecType,string& FileName)
{
	COriFileOperate oriFile(FileName);
	size_t nVecSize = vecType.size();
	if (strXml.find("chinese_simple") != -1)
	{
		for (size_t i=1;i<nVecSize;++i)
		{
			vecType[i] = false;
		}
	}
	char* szNumOrStrInfo = new char[nVecSize+1];
	for (size_t i = 0; i < nVecSize; i++)
	{
		if (vecType[i])
		{
			szNumOrStrInfo[i] = 'n';
		}
		else
		{
			szNumOrStrInfo[i] = 's';
		}
	}
	szNumOrStrInfo[nVecSize] = '\0';
	TiXmlDocument* pXmlDoc = new TiXmlDocument;
	pXmlDoc->InsertEndChild(TiXmlElement("config"));
	TiXmlNode* pXmlNode = pXmlDoc->FirstChild("config");
	pXmlNode->InsertEndChild(TiXmlElement("TableType"));
	TiXmlElement* pXmlElem = pXmlNode->FirstChildElement("TableType");
	pXmlElem->InsertEndChild(TiXmlText("S"));
	string str = szNumOrStrInfo;
	string str1 = oriFile.GetDataByRowCol(0,0);
	transform(str1.begin(),str1.end(),str1.begin(),toupper);
	for(uint32 i=1;i<=str.size();i++)
	{
		TiXmlElement* cxn = new TiXmlElement("Col");
		pXmlNode->LinkEndChild(cxn);
		if(str1!="NOTE:")
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(0,i-1).c_str());
		}
		else
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(1,i-1).c_str());
		}
		string ss;
		stringstream temp;
		temp<<szNumOrStrInfo[i-1];
		temp>>ss;
		cxn->SetAttribute("Type",ss);
	}

	pXmlDoc->SaveFile(strXml);

	delete pXmlDoc;
	pXmlDoc = NULL;
	delete[] szNumOrStrInfo;
	szNumOrStrInfo = NULL;
}
void ForceDeleteFile(const string& strFile)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile.c_str(),&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return ;
	string strFullPath = strFile;
	strFullPath.resize(strFullPath.length()+1);
	SHFILEOPSTRUCT op;
	memset(&op,0,sizeof(op));
	op.pFrom = strFullPath.c_str();
	op.pTo = NULL;
	op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	op.wFunc = FO_DELETE;
	op.hwnd = NULL;
	op.hNameMappings = NULL;
	if (SHFileOperation(&op))
	{
		string strMessage = strFile + "需要更新内容，但现存文件不能删除，请确认是否有其他程序在使用中";
		MessageBox(0,strMessage.c_str(),"错误",NULL);
		exit(0);
	}
}