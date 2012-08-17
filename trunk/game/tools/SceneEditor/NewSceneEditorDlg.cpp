// NewSceneEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "MyDefine.h"
#include "NewSceneEditor.h"
#include "NewSceneEditorDlg.h"
#include "SEPathMgr.h"
#include "IAudioModule.h"
#include "IAudioSystem.h"
#include "CPkgFile.h"
#include "DynamicFileSel.h"

#include "StateMgr.h"
#include "PlaceMgr.h"
#include "ChoiceMap.h"
#include "CodeCvs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern const wstring PATH_ALIAS_SCENE_EDITOR;

CMapView* g_pMapView = NULL;
CNewSceneEditorDlg* g_pMainDlg = NULL;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
EEideModeType g_eModeType = eMT_NORMAL;

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


// CNewSceneEditorDlg 对话框




CNewSceneEditorDlg::CNewSceneEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewSceneEditorDlg::IDD, pParent)
	, IsLeapCheck(false)
	, m_pCurColonyEditDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNewSceneEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAP_VIEW, m_mapView);
	DDX_Control(pDX, IDC_VIEW_SCALING, m_SliderScaling);
	DDX_Control(pDX, IDC_ELEMENT_TAB, m_tabElement);
	DDX_Control(pDX, IDC_PEN_SIZE, m_SliderPenSize);
	DDX_Control(pDX, IDC_TRANSPARENCE, m_SliderTransparence);
	DDX_Control(pDX, IDC_AREA_NAME, m_ComboAreaName);

	DDX_Control(pDX, IDC_BARRIER_OBJ, m_ComboBarrierObj);
}

BEGIN_MESSAGE_MAP(CNewSceneEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CNewSceneEditorDlg::OnBnClickedButton1)
	ON_COMMAND(ID_LOAD_MAP, &CNewSceneEditorDlg::OnLoadMap)
	ON_WM_TIMER()
	ON_COMMAND(ID_LOAD_SCENE, &CNewSceneEditorDlg::OnLoadScene)
	ON_COMMAND(ID_LOAD_AREA, &CNewSceneEditorDlg::OnLoadArea)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_VIEW_SCALING, &CNewSceneEditorDlg::OnNMCustomdrawViewScaling)
	ON_BN_CLICKED(IDC_RESET, &CNewSceneEditorDlg::OnBnResetView)
	ON_BN_CLICKED(IDC_SHOW_NPC, &CNewSceneEditorDlg::OnBnClickedShowNpc)
	ON_BN_CLICKED(IDC_SHOW_PATH, &CNewSceneEditorDlg::OnBnClickedShowPath)
	ON_BN_CLICKED(IDC_GRID_LINE, &CNewSceneEditorDlg::OnBnClickedShowEyeshot)
	ON_BN_CLICKED(IDC_SHOW_OBJ, &CNewSceneEditorDlg::OnBnClickedShowObj)
	ON_BN_CLICKED(IDC_SHOW_TRAP, &CNewSceneEditorDlg::OnBnClickedShowTrap)
	ON_BN_CLICKED(IDC_SHOW_ARROW, &CNewSceneEditorDlg::OnBnClickedShowArrow)
	ON_COMMAND(ID_SAVE_SCENE, &CNewSceneEditorDlg::OnSaveScene)
	ON_COMMAND(ID_SAVE_AREA, &CNewSceneEditorDlg::OnSaveArea)
	ON_COMMAND(ID_SAVE_TRANS_AREA, &CNewSceneEditorDlg::OnSaveTransArea)
	
	ON_BN_CLICKED(IDC_SHOW_NPC_FULL, &CNewSceneEditorDlg::OnBnClickedShowNpcFull)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ELEMENT_TAB, &CNewSceneEditorDlg::OnTcnSelchangeElementTab)
	ON_BN_CLICKED(IDC_SHOW_GAME_VIEW, &CNewSceneEditorDlg::OnBnClickedShowGameView)
	ON_BN_CLICKED(IDC_SHOW_FOLLOW_MOUSE, &CNewSceneEditorDlg::OnBnClickedShowFollowMouse)
	ON_BN_CLICKED(IDC_DRAW_AREA, &CNewSceneEditorDlg::OnBnClickedEditArea)
	ON_BN_CLICKED(IDC_FILL_AREA, &CNewSceneEditorDlg::OnBnClickedEditArea)
	ON_BN_CLICKED(IDC_ERASE_AREA, &CNewSceneEditorDlg::OnBnClickedEditArea)
	ON_BN_CLICKED(IDC_SHOW_AREA, &CNewSceneEditorDlg::OnBnClickedShowArea)
	ON_BN_CLICKED(IDC_DONOT_OVER, &CNewSceneEditorDlg::OnBnClickedDoNotOver)
	ON_BN_CLICKED(IDC_SET_CENTER_POS, &CNewSceneEditorDlg::OnBnClickedSetCenterPos)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TRANSPARENCE, &CNewSceneEditorDlg::OnNMCustomdrawTransparence)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PEN_SIZE, &CNewSceneEditorDlg::OnNMCustomdrawPenSize)
	ON_BN_CLICKED(IDC_SET_COLOR, &CNewSceneEditorDlg::OnBnClickedSetColor)
	ON_CBN_EDITCHANGE(IDC_AREA_NAME, &CNewSceneEditorDlg::OnCbnEditchangeAreaName)
	ON_CBN_SELCHANGE(IDC_AREA_NAME, &CNewSceneEditorDlg::OnCbnSelchangeAreaName)
	ON_BN_CLICKED(IDC_SHOW_BLOCK, &CNewSceneEditorDlg::OnBnClickedShowBlock)
	ON_BN_CLICKED(IDC_IGNORE_BLOCK, &CNewSceneEditorDlg::OnBnClickedIgnoreBlock)
	ON_BN_CLICKED(IDC_FIND_IN_BARRIER, &CNewSceneEditorDlg::OnBnClickedFindInBarrier)
	ON_BN_CLICKED(IDC_AREA_MUSIC_EDIT, &CNewSceneEditorDlg::OnBnClickedAreaMusicEdit)
	ON_BN_CLICKED(IDC_MOVE_MAP, &CNewSceneEditorDlg::OnBnClickedMoverMap)
	ON_BN_CLICKED(IDC_SET_GRID_MIDDLE, &CNewSceneEditorDlg::OnBnClickedSetGridMiddle)
	ON_BN_CLICKED(IDC_RULER, &CNewSceneEditorDlg::OnBnClickedRuler)
	ON_BN_CLICKED(IDC_ROTATE, &CNewSceneEditorDlg::OnBnClickedRotate)
	ON_COMMAND(ID_SET_OFFSET_POSITION, &CNewSceneEditorDlg::OnSetOffsetPosition)
	ON_COMMAND(ID_RESAVE_ALL_SCENE_FILE, &CNewSceneEditorDlg::OnReSaveAllSceneFile)
	ON_COMMAND(ID_RESAVE_ALL_AREA_FILE, &CNewSceneEditorDlg::OnReSaveAllAreaFile)
	ON_COMMAND(ID_RESAVE_ALL_DYNAMIC_FILE, &CNewSceneEditorDlg::OnReSaveAllDynamicFile)
END_MESSAGE_MAP()


// CNewSceneEditorDlg 消息处理程序

BOOL CNewSceneEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_pMapView = &m_mapView;
	g_pMainDlg = this;
	//m_mainMenu.LoadMenu(IDR_MAIN_MENU);
	//ASSERT(m_mainMenu);
	//SetMenu(&m_mainMenu);

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

	CPlaceMgr::Inst().Init(this);
	m_mapView.Init();
	SetTimer(PAINT_EVENT_ID, PAINT_ELAPSE, NULL);
	m_SliderScaling.SetRange(0,100);
	m_SliderScaling.SetPos(50);
	ShowScalingCtrl(false);

	m_SliderPenSize.SetRange(1,15);
	m_SliderPenSize.SetPos(3);

	m_SliderTransparence.SetRange(0,255);
	m_SliderTransparence.SetPos(128);
	
	CRect rc;
	m_tabElement.GetClientRect(&rc);
	rc.top += 35;
	rc.bottom -= 2;

	m_tabElement.InsertItem(eTPT_NPC, "NPC");
	m_tabElement.InsertItem(eTPT_OBJ, "OBJ");
	m_tabElement.InsertItem(eTPT_TRAP, "TRAP");
	m_tabElement.InsertItem(eTPT_COLONY, "群组");
	for (int i = 0; i < eTPT_COUNT ; i++)
	{
		m_tabPageElement[i].Create(IDD_ELEMENT_TAB, &m_tabElement);
		m_tabPageElement[i].MoveWindow(&rc);
		m_tabPageElement[i].SetType(ETabPageType(i));
	}

	ShowElementTabPage(eTPT_NPC);
	InitMusic();
	switch(g_eModeType)
	{
	case eMT_NORMAL:
		SetWindowText("普通场景编辑");
		break;
	case eMT_AREA_FB:
		{
			SetWindowText("区域副本编辑");
			COriginalData::BarrierObj_Iter iter = g_pOriginalData->m_vecBarrierObj.begin();
			for (; iter != g_pOriginalData->m_vecBarrierObj.end(); ++iter)
			{
				m_ComboBarrierObj.AddString(*iter);
			}
			m_ComboBarrierObj.SetCurSel(0);
			m_ComboBarrierObj.EnableWindow(true);
			GetDlgItem(IDC_FILL_AREA)->EnableWindow(false);
			GetDlgItem(IDC_DONOT_OVER)->EnableWindow(false);
			GetDlgItem(IDC_SET_COLOR)->EnableWindow(false);
			m_ComboAreaName.EnableWindow(false);
			SetAreaColor("障碍", BARRIER_OBJ_IMAGE_COLOR);
			m_ComboAreaName.SetCurSel(0);
			break;
		}
	case eMT_DYNAMIC:
		SetWindowText("动态文件编辑");
		GetDlgItem(IDC_SET_CENTER_POS)->ShowWindow(SW_SHOW);
		g_pCache->ReLoadDynamicFile();
		g_pCache->LoadDynamicNames();
	    break;
	case eMT_PATH_EDIT:
		SetWindowText("动态路径编辑");
		GetDlgItem(IDC_SET_CENTER_POS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON1)->SetWindowText("摆放路径");
	    break;
	default:
	    break;
	}
	if (g_eModeType != eMT_NORMAL)
	{
		CMenu* pMenu = GetMenu();
		pMenu->EnableMenuItem(ID_LOAD_AREA, MF_DISABLED|MF_GRAYED);
		pMenu->EnableMenuItem(ID_SAVE_AREA, MF_DISABLED|MF_GRAYED);
	}
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNewSceneEditorDlg::ShowElementTabPage(ETabPageType type)
{
	for (int i = 0; i < eTPT_COUNT; i ++)
	{
		if(i == type)
		{
			m_tabPageElement[i].ShowWindow(SW_SHOW);
		}
		else
		{
			m_tabPageElement[i].ShowWindow(SW_HIDE);
		}
	}
}

void CNewSceneEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CNewSceneEditorDlg::OnPaint()
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
HCURSOR CNewSceneEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNewSceneEditorDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (g_eModeType != eMT_PATH_EDIT)
	{
		if(g_eModeType == eMT_DYNAMIC && g_pCache->GetCurEditDynamicName() == "")
		{
			MessageBox("请先在加载文件 中选择或新建一个 要编辑的动态文件");
			return;
		}
		if (!CPlaceMgr::Inst().IsShow())
		{
			CPlaceMgr::Inst().NormalShow();
		}
		
	}
	else
	{
		CDynamicFileSel dlg;
		if (dlg.DoModal() == IDOK)
		{
			CString selName = dlg.GetSelName();
			PPlaceNpc pNpc = g_pCache->FindNpc(selName);
			if (pNpc)
			{
				m_mapView.MoveMapToCenter(pNpc->fPos);
				m_mapView.RemoveSelectElement(eET_ALL);
				m_mapView.AddSelectElement(pNpc);
			}
			else if(selName != "")
			{
				CPlaceMgr::Inst().SetPathName(selName);
				CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
			}
		}
	}
}

void CNewSceneEditorDlg::OnLoadMap()
{
	CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_ING);
	//CString strPath = CSEPathMgr::Inst().GetMapPath();
	g_pMapView->PauseRender();
	CChoiceMap choiceMapDlg;
	if ( IDOK == choiceMapDlg.DoModal())
	{
		CString strSceneName = choiceMapDlg.GetSceneName();
		CString strPath = g_pOriginalData->GetMapFile(strSceneName);
		if (strPath  == "")
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
			return;
		}
		
		CString mapPath ;

		mapPath = GetMapPicturePath(strSceneName);
		if (mapPath == "")
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
			MessageBox("未找到可用的地图图片.\n 请将jpg图片放在designer/tools/NewSceneEditor/Map 下 \n命名与 scene_common表一致,使用同一个地图的不同场景只要一个有图片就行.");
			return;
		}
		
		//strPath  = CSEPathMgr::Inst().GetArtistPath()  + '/' + strPath;
		CString errorMessage;
		if(m_mapView.LoadMap(mapPath, strPath, errorMessage))
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_OK);
			m_strCurSceneName = strSceneName;
			g_pCache->ClearAll();
			m_mapView.UpdateAll();
			//CSEPathMgr::Inst().SaveMapPath(mapPath);
			ShowScalingCtrl(SW_SHOW);
			m_mapView.SetAreaImageAlpha(m_SliderTransparence.GetPos());
			if (g_eModeType == eMT_PATH_EDIT)
			{
				g_pCache->LoadDynamicPath(strSceneName);
			}
			if (choiceMapDlg.IsAutoOpenFile())
			{
				if (g_pOriginalData->GetSceneFile(strSceneName) != "")
				{
					CString path = CSEPathMgr::Inst().GetDesignerPath() + '/' +	g_pOriginalData->GetSceneFile(strSceneName) + ".lua";
					LoadScene(path);
				}
				else
				{
					MessageBox("无摆怪文件");
				}
			}
			if (choiceMapDlg.IsAutoOpenAreaFile())
			{
				if(g_pOriginalData->GetAreaFile(strSceneName) != "")
				{
					CString path = CSEPathMgr::Inst().GetDesignerPath() + '/' +	g_pOriginalData->GetAreaFile(strSceneName) + ".lua";
					LoadArea(path);
				}
				else
				{
					MessageBox("无区域文件");
				}
			}
		}
		else
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
			MessageBox(errorMessage);
		}		
	}
	else
	{
		CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
		return;
	}
}

CString CNewSceneEditorDlg::GetMapPicturePath(const CString& sceneName)
{
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	CString picturePath ;
	FILE* pFile = NULL;
	CString mapFileName = g_pOriginalData->GetMapFile(sceneName);
	
	COriginalData::SceneCfg_Map::iterator iter = g_pOriginalData->m_mapSceneCfg.begin();
	for (; iter != g_pOriginalData->m_mapSceneCfg.end(); ++iter)
	{
		CString tempSceneName = iter->first;
		if (mapFileName == g_pOriginalData->GetMapFile(tempSceneName))
		{
			picturePath = "Map/" + tempSceneName + ".jpg";
			pFile = fopen(picturePath.GetBuffer(), "r");
			if(NULL != pFile)
			{
				fclose(pFile);
				return  picturePath;
			}
		}
	}
	return "";
}

void CNewSceneEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case PAINT_EVENT_ID:
		m_mapView.RePaint();
		UpdateInfoBox();
		break;
	default:
	    break;
	}
	CString areaName;
	m_ComboAreaName.GetWindowText(areaName);
	CDC* pDC = GetDlgItem(IDC_AREA_COLOR_SHOW)->GetDC();
	CRect rect;
	GetDlgItem(IDC_AREA_COLOR_SHOW)->GetClientRect(rect);
	COLORREF color = g_pCache->GetAreaColor(areaName);
	if(color != -1)
	{
		pDC->FillSolidRect(rect.left, rect.top, rect.right -rect.left, rect.bottom -rect.top,color);
	}
	else
	{
		GetDlgItem(IDC_AREA_COLOR_SHOW)->SetWindowText("未设置颜色");
	}
	ReleaseDC(pDC);

	//为了实现 鼠标出了m_tabElement 窗口后把焦点设置在主窗口上
	static bool isInElementTabWnd = true;
	CPoint pos;
	GetCursorPos(&pos);
	m_tabElement.ScreenToClient(&pos);
	m_tabElement.GetClientRect(&rect);
	if(pos.x >= rect.left && pos.x <= rect.right && pos.y >= rect.top && pos.y <= rect.bottom)
	{
		isInElementTabWnd = true;
	}
	else if(isInElementTabWnd)
	{
		SetFocus();
		isInElementTabWnd = false;
	}


	

	CDialog::OnTimer(nIDEvent);
}






void CNewSceneEditorDlg::OnLoadScene()
{
	if(!m_mapView.IsMapOk())
	{
		MessageBox("先加载地图!");
		return;
	}
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return;
	}
	CStateMgr::Inst().DoEvent(eEVENT_LOAD_SCENE_ING);
	
	if(g_eModeType == eMT_DYNAMIC)
	{
		g_pMapView->PauseRender();
		CDynamicFileSel dlg;
		if (dlg.DoModal() == IDOK)
		{
			CString selName = dlg.GetSelName();
			if (selName != "" && g_pCache->LoadDynamicData(selName))
			{
				CStateMgr::Inst().DoEvent(eEVENT_LOAD_SCENE_OK);
				m_mapView.SetUpdateMark(eUES_ALL);
			}
		}
	}
	else
	{
		CString strPath = CSEPathMgr::Inst().GetScenePath();
		CFileDialog Dlg(TRUE, NULL, strPath.GetBuffer(), 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.lua)|*.lua||");
		CString strSceneFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('/') - 1);
		char fileName[CHAT_BUFFER_SIZE];
		strcpy(fileName, strSceneFileName);
		Dlg.m_ofn.lpstrFile = fileName;
		g_pMapView->PauseRender();
		if(Dlg.DoModal() == IDOK)
		{
			CString pathName = Dlg.GetPathName();
			if (pathName.Right(4) == ".lua")
			{
				if(LoadScene(pathName))
				{
					return;
				}
			}
			MessageBox("场景文件加载失败!");
		}
	}
}

bool CNewSceneEditorDlg::LoadScene(const CString& pathName)
{
	if(g_pCache->LoadSceneData(pathName))
	{
		CStateMgr::Inst().DoEvent(eEVENT_LOAD_SCENE_OK);
		CSEPathMgr::Inst().SaveScenePath(pathName);
		m_mapView.SetUpdateMark(eUES_ALL);
		return true;
	}
	return false;
}

bool CNewSceneEditorDlg::LoadArea(const CString& pathName)
{
	if(g_pCache->LoadAreaData(pathName))
	{
		CSEPathMgr::Inst().SaveAreaPath(pathName);
		return true;
	}
	return false;
}



void CNewSceneEditorDlg::OnLoadArea()
{
	if(!m_mapView.IsMapOk())
	{
		MessageBox("先加载地图!");
		return;
	}
	CString strPath = CSEPathMgr::Inst().GetAreaPath();

	CFileDialog Dlg(TRUE, NULL, strPath.GetBuffer(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.lua)|*.lua||");
	CString strAreaFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('/') - 1);
	char fileName[CHAT_BUFFER_SIZE];
	strcpy(fileName, strAreaFileName);
	Dlg.m_ofn.lpstrFile = fileName;
	g_pMapView->PauseRender();
	if(Dlg.DoModal() == IDOK)
	{
		CString areaPath = Dlg.GetPathName();
		if (areaPath.Right(4) == ".lua")
		{
			if(LoadArea(areaPath))
			{
				return;
			}
		}
		MessageBox("场景文件加载失败!");
	}
}




void CNewSceneEditorDlg::OnNMCustomdrawViewScaling(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true);
	float fhalf = (float)((m_SliderScaling.GetRangeMax() - m_SliderScaling.GetRangeMin()) /2.0);
	int nScaling = m_SliderScaling.GetPos();
	float fScaling = 1.0;
	if (nScaling >= fhalf)//放大
	{
		fScaling = float(1 + ((nScaling - fhalf)/ fhalf)  * (MAX_SCALING -1));
	}
	else//缩小
	{
		fScaling = float(MIN_SCALING + nScaling * (1 - MIN_SCALING) / fhalf);
	}
	if (fScaling < MIN_SCALING)
	{
		fScaling = (float)MIN_SCALING;
	}
	else if(fScaling > MAX_SCALING)
	{
		fScaling = (float)MAX_SCALING;
	}
	m_mapView.SetScaling(fScaling);
	*pResult = 0;
}

void CNewSceneEditorDlg::OnBnResetView()
{
	m_SliderScaling.SetPos((m_SliderScaling.GetRangeMax() - m_SliderScaling.GetRangeMin()) /2);
}


void CNewSceneEditorDlg::ShowScalingCtrl(int nCmdShow )
{
	m_SliderScaling.ShowWindow(nCmdShow);
	GetDlgItem(IDC_ZOOM1)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_ZOOM2)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_RESET)->ShowWindow(nCmdShow);
}

BOOL CNewSceneEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_KEYUP:
		switch(pMsg->wParam)
		{
		case VK_SHIFT:
			CStateMgr::Inst().DoEvent(eEVENT_SHIFT_UP);
			break;
		case VK_CONTROL:
			CStateMgr::Inst().DoEvent(eEVENT_CTRL_UP);
			break;
		case VK_TAB:
			CStateMgr::Inst().DoEvent(eEVENT_TAB_UP);
			break;
		}
		break;
	case WM_KEYDOWN:
	switch(pMsg->wParam)
		{
		case VK_SHIFT:
			CStateMgr::Inst().DoEvent(eEVENT_SHIFT_DOWN);
			break;
		case VK_CONTROL:
			CStateMgr::Inst().DoEvent(eEVENT_CTRL_DOWN);
			break;
		case VK_TAB:
			CStateMgr::Inst().DoEvent(eEVENT_TAB_DOWN);
			break;
		case VK_ESCAPE:
			CStateMgr::Inst().DoEvent(eEVENT_EIXT);
			return 0;
		case VK_DELETE:
			m_mapView.DeleteAllSelectElement();
			break;
		case 'C':
			if (CStateMgr::IsAssistState(eAS_CTRL))
			{
				m_mapView.UserCopyElement();
			}
			break;
		case 'V':
			if (CStateMgr::IsAssistState(eAS_CTRL))
			{
				m_mapView.PasteElement();
			}
			break;
		case 'X':
			if (CStateMgr::IsAssistState(eAS_CTRL))
			{
				m_mapView.CutElement();
			}
			break;
		case 'R':
			m_mapView.RotateElement();
			break;
		case 'Q':
			m_mapView.Measure();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		CStateMgr::Inst().DoEvent(eEVENT_L_DOWN);
		break;
	case WM_RBUTTONDOWN:
		CStateMgr::Inst().DoEvent(eEVENT_R_DOWN);
		break;
	case WM_LBUTTONUP:
		CStateMgr::Inst().DoEvent(eEVENT_L_UP);
		m_mapView.UpdateFocus();
		break;
	case WM_RBUTTONUP:
		CStateMgr::Inst().DoEvent(eEVENT_R_UP);
		break;
	case WM_MOUSEWHEEL:
		if(m_mapView.IsCursorInView() && !CPlaceMgr::Inst().IsShow())
		{
			if(m_mapView.IsShowSate(eSS_AREA))
			{
				m_SliderPenSize.SetFocus();
			}
			else 
			{
				m_SliderScaling.SetFocus();
			}
		}
	case WM_LBUTTONDBLCLK:
		CStateMgr::Inst().DoEvent(eEVENT_DB_L_DOWN);
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CNewSceneEditorDlg::UpdateInfoBox()
{
	FPos gridPos;
	if(m_mapView.GetCurCursorGridPos(gridPos))
	{
		CString s;
		s.Format("%.2f",gridPos.x);
		GetDlgItem(IDC_GRID_X)->SetWindowText(s);
		s.Format("%.2f",gridPos.y);
		GetDlgItem(IDC_GRID_Y)->SetWindowText(s);
	}

	CElement* pFocus = m_mapView.GetMyFocus();
	if (pFocus != NULL )
	{
		CString name;
		switch(pFocus->eType)
		{
		case eET_NPC:
			name = static_cast<PPlaceNpc>(pFocus)->name;
			break;
		case eET_OBJ:
			name = static_cast<PPlaceObj>(pFocus)->name;
			break;
		case eET_TRAP:
			name = static_cast<PPlaceTrap>(pFocus)->name;
		    break;
		case eET_PATH_LINE:
			name = static_cast<CPathLine*>(pFocus)->pBackKeyPos->pOwner->name;
			name += "<一段路径>";
			break;
		case eET_PATH_KEY_POS:
			name = static_cast<PPathKeyPos>(pFocus)->pOwner->name;
			name += "<巡逻点>";
			break;
		}
		GetDlgItem(IDC_NAME)->SetWindowText(name);
		CString pos;
		pos.Format("%.2f", pFocus->fPos.x);
		GetDlgItem(IDC_GRID_X2)->SetWindowText(pos);
		pos.Format("%.2f", pFocus->fPos.y);
		GetDlgItem(IDC_GRID_Y2)->SetWindowText(pos);
	}
	else
	{
		GetDlgItem(IDC_NAME)->SetWindowText("");
		GetDlgItem(IDC_GRID_X2)->SetWindowText("");
		GetDlgItem(IDC_GRID_Y2)->SetWindowText("");
	}
}

void CNewSceneEditorDlg::UpdateShowBox()
{
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_NPC))->SetCheck(m_mapView.IsShowSate(eSS_NPC) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_ARROW))->SetCheck(m_mapView.IsShowSate(eSS_ARROW) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_PATH))->SetCheck(m_mapView.IsShowSate(eSS_PATH) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_GRID_LINE))->SetCheck(m_mapView.IsShowSate(eSS_GRID_LINE) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_OBJ))->SetCheck(m_mapView.IsShowSate(eSS_OBJ) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_TRAP))->SetCheck(m_mapView.IsShowSate(eSS_TRAP) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_NPC_FULL))->SetCheck(m_mapView.IsShowSate(eSS_NPC_FULL) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_GAME_VIEW))->SetCheck(m_mapView.IsShowSate(eSS_GAME_VIEW) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_FOLLOW_MOUSE))->SetCheck(m_mapView.IsShowSate(eSS_GAME_VIEW_FOLLOW) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_AREA))->SetCheck(m_mapView.IsShowSate(eSS_AREA) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_SHOW_BLOCK))->SetCheck(m_mapView.IsShowSate(eSS_BLOCK) ? BST_CHECKED : BST_UNCHECKED);
	static_cast<CButton*>(GetDlgItem(IDC_IGNORE_BLOCK))->SetCheck(m_mapView.IsShowSate(eSS_IGNORE_BLOCK) ? BST_CHECKED : BST_UNCHECKED);
	if (m_mapView.IsShowSate(eSS_NPC))
	{
		GetDlgItem(IDC_SHOW_PATH)->EnableWindow(true);
		GetDlgItem(IDC_GRID_LINE)->EnableWindow(true);
		//GetDlgItem(IDC_SHOW_ARROW)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_SHOW_PATH)->EnableWindow(false);
		GetDlgItem(IDC_GRID_LINE)->EnableWindow(false);
		//GetDlgItem(IDC_SHOW_ARROW)->EnableWindow(false);
	}
	//if (m_mapView.IsShowSate(eSS_AREA))
	//{
	//	GetDlgItem(IDC_SHOW_BLOCK)->EnableWindow(false);
	//}
	//else
	//{
	//	GetDlgItem(IDC_SHOW_BLOCK)->EnableWindow(true);
	//}
}

void CNewSceneEditorDlg::OnBnClickedShowNpc()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_NPC))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_NPC,true);
		GetDlgItem(IDC_SHOW_PATH)->EnableWindow(true);
		GetDlgItem(IDC_GRID_LINE)->EnableWindow(true);
		//GetDlgItem(IDC_SHOW_ARROW)->EnableWindow(true);
		
	}
	else
	{
		m_mapView.SetShowState(eSS_NPC,false);
		GetDlgItem(IDC_SHOW_PATH)->EnableWindow(false);
		GetDlgItem(IDC_GRID_LINE)->EnableWindow(false);
		//GetDlgItem(IDC_SHOW_ARROW)->EnableWindow(false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowPath()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_PATH))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_PATH,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_PATH,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowEyeshot()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_GRID_LINE))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_GRID_LINE,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_GRID_LINE,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowObj()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_OBJ))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_OBJ,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_OBJ, false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowTrap()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_TRAP))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_TRAP,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_TRAP,false);
	}
}


void CNewSceneEditorDlg::OnBnClickedShowArrow()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_ARROW))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_ARROW,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_ARROW,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowNpcFull()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_NPC_FULL))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_mapView.SetShowState(eSS_NPC_FULL, true);
	}
	else
	{
		m_mapView.SetShowState(eSS_NPC_FULL,false);
	}
}

void CNewSceneEditorDlg::ConvertToUtf8(TCHAR* m_szSave)
{
	//std::string tString = gbk_to_utf8(m_szSave);
	//int length = tString.length();
	//for(int i = 0; i < length; i++)
	//{
	//	m_szSave[i] = tString[i];
	//}
	//m_szSave[length] = '\0';
}

void CNewSceneEditorDlg::OnSaveScene()
{
	if(!m_mapView.IsMapOk())
	{
		MessageBox("先加载地图!");
		return;
	}
	if (g_eModeType == eMT_DYNAMIC)
	{
		if (g_pCache->SaveDynamicFile())
		{
			MessageBox("保存成功.");
		}
		else
		{
			MessageBox("保存失败.");
		}
		return;
	}
	else if (g_eModeType == eMT_PATH_EDIT)
	{
		if (g_pCache->SaveDynamicPath())
		{
			MessageBox("保存成功.");
		}
		else
		{
			MessageBox("保存失败.");
		}
		return;
	}
	if(g_eModeType == eMT_AREA_FB && GetBarrierObjName() == "")
	{
		MessageBox("请选择障碍obj类型");
		m_ComboBarrierObj.SetFocus();
		return;
	}
	CStateMgr::Inst().DoEvent(eEVENT_LOAD_SCENE_ING);
	CString strPath = CSEPathMgr::Inst().GetScenePath();
	
	CFileDialog Dlg(false, NULL, strPath.GetBuffer(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.lua)|*.lua||");
	CString strSceneFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('/') - 1);
	char fileName[CHAT_BUFFER_SIZE];
	strcpy(fileName, strSceneFileName);
	Dlg.m_ofn.lpstrFile = fileName;
	while (true)
	{
		g_pMapView->PauseRender();
		if(Dlg.DoModal() == IDOK)
		{
			CString mapPath = Dlg.GetPathName();
			if(mapPath.Find(".") == -1)
			{
				mapPath += ".lua";
			}
			else
			{
				if (mapPath.Right(4) != ".lua")
				{
					MessageBox("请以 .lua 为后缀");
					continue;
				}
			}
			if(!g_pCache->SaveSceneFile(mapPath.GetBuffer()))
			{
				MessageBox("保存失败.");
			}
			else
			{
				MessageBox("保存成功.");
				CSEPathMgr::Inst().SaveScenePath(mapPath);
			}
			return;
		}
		else
		{
			return;
		}
	}
}


void CNewSceneEditorDlg::OnSaveArea()
{
	if(!m_mapView.IsMapOk())
	{
		MessageBox("先加载地图!");
		return;
	}
	CString strPath = CSEPathMgr::Inst().GetAreaPath();

	CFileDialog Dlg(false, NULL, strPath.GetBuffer(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.lua)|*.lua||");
	CString strAreaFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('/') - 1);
	char fileName[CHAT_BUFFER_SIZE];
	strcpy(fileName, strAreaFileName);
	Dlg.m_ofn.lpstrFile = fileName;
	while (true)
	{
		g_pMapView->PauseRender();
		if(Dlg.DoModal() == IDOK)
		{
			CString mapPath = Dlg.GetPathName();
			if(mapPath.Find(".") == -1)
			{
				mapPath += ".lua";
			}
			else
			{
				if (mapPath.Right(4) != ".lua")
				{
					MessageBox("请以 .lua 为后缀");
					continue;
				}
			}
			if(!SaveArea(mapPath.GetBuffer()))
			{
				MessageBox("保存失败.");
			}
			else
			{
				MessageBox("保存成功.");
				CSEPathMgr::Inst().SaveAreaPath(mapPath);
			}
			return;
		}
		else
		{
			return;
		}
	}
}

void CNewSceneEditorDlg::OnSaveTransArea()
{
	if(!m_mapView.IsMapOk())
	{
		MessageBox("先加载地图!");
		return;
	}
	CString strPath = CSEPathMgr::Inst().GetAreaPath();

	CFileDialog Dlg(false, NULL, strPath.GetBuffer(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.lua)|*.lua||");
	CString strAreaFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('/') - 1);
	char fileName[CHAT_BUFFER_SIZE];
	strcpy(fileName, strAreaFileName);
	Dlg.m_ofn.lpstrFile = fileName;
	while (true)
	{
		g_pMapView->PauseRender();
		if(Dlg.DoModal() == IDOK)
		{
			CString mapPath = Dlg.GetPathName();
			if(mapPath.Find(".") == -1)
			{
				mapPath += ".lua";
			}
			else
			{
				if (mapPath.Right(4) != ".lua")
				{
					MessageBox("请以 .lua 为后缀");
					continue;
				}
			}
			if(!SaveTransArea(mapPath.GetBuffer()))
			{
				MessageBox("保存失败.");
			}
			else
			{
				MessageBox("保存成功.");
				CSEPathMgr::Inst().SaveAreaPath(mapPath);
			}
			return;
		}
		else
		{
			return;
		}
	}
}

void CNewSceneEditorDlg::OnTcnSelchangeElementTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowElementTabPage(ETabPageType(m_tabElement.GetCurSel()));
	*pResult = 0;
}

void CNewSceneEditorDlg::OnBnClickedShowGameView()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_GAME_VIEW))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.SetShowState(eSS_GAME_VIEW,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_GAME_VIEW,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowBlock()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_BLOCK))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.SetShowState(eSS_BLOCK,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_BLOCK,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedIgnoreBlock()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_IGNORE_BLOCK))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.SetShowState(eSS_IGNORE_BLOCK,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_IGNORE_BLOCK,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowFollowMouse()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_FOLLOW_MOUSE))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.SetShowState(eSS_GAME_VIEW_FOLLOW,true);
	}
	else
	{
		m_mapView.SetShowState(eSS_GAME_VIEW_FOLLOW,false);
	}
}

void CNewSceneEditorDlg::OnBnClickedShowArea()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_SHOW_AREA))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.SetShowState(eSS_AREA,true);
		//GetDlgItem(IDC_SHOW_BLOCK)->EnableWindow(false);
		CStateMgr::Inst().DoEvent(eEVENT_BEGIN_EDIT_AREA);
	}
	else
	{
		m_mapView.SetShowState(eSS_AREA,false);
		//GetDlgItem(IDC_SHOW_BLOCK)->EnableWindow(true);
		CStateMgr::Inst().DoEvent(eEVENT_EXIT_EDIT_AREA);
	}
	OnBnClickedEditArea();
	OnBnClickedDoNotOver();
}

void CNewSceneEditorDlg::OnBnClickedEditArea()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_DRAW_AREA))->GetState();
	if ((state & 0x0003) == 1)
	{	
		CStateMgr::Inst().DoEvent(eEVENT_DRAW_AREA);
		if (g_eModeType == eMT_AREA_FB)
		{
			GetDlgItem(IDC_PEN_SIZE)->EnableWindow(false);
			m_SliderPenSize.SetPos(1);
		}
		return;
	}
	state  = static_cast<CButton*> (GetDlgItem(IDC_FILL_AREA))->GetState();
	if ((state & 0x0003) == 1)
	{	
		CStateMgr::Inst().DoEvent(eEVENT_FILL_AREA);
		return;
	}
	state  = static_cast<CButton*> (GetDlgItem(IDC_ERASE_AREA))->GetState();
	if ((state & 0x0003) == 1)
	{	
		CStateMgr::Inst().DoEvent(eEVENT_ERASE_AREA);
		if (g_eModeType == eMT_AREA_FB)
		{
			GetDlgItem(IDC_PEN_SIZE)->EnableWindow(true);
			m_SliderPenSize.SetPos(3);
		}
		return;
	}
}

void CNewSceneEditorDlg::OnBnClickedDoNotOver()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_DONOT_OVER))->GetState();
	if ((state & 0x0003) == 1)
	{	
		m_mapView.IsOverArea = false;
	}
	else
	{
		m_mapView.IsOverArea = true;
	}
}




void CNewSceneEditorDlg::OnNMCustomdrawTransparence(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_mapView.SetAreaImageAlpha(m_SliderTransparence.GetPos());
	CString s;
	s.Format("%.2f%%", m_SliderTransparence.GetPos()/2.55);
	GetDlgItem(IDC_TRANSPARENCE_SHOW)->SetWindowText(s);
	*pResult = 0;
}

void CNewSceneEditorDlg::OnNMCustomdrawPenSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CString s;
	s.Format("%d X %d",GetPenSize(), GetPenSize());
	GetDlgItem(IDC_PEN_SIZE_SHOW)->SetWindowText(s);
	*pResult = 0;
}

void CNewSceneEditorDlg::OnBnClickedSetColor()
{
	CString name;
	m_ComboAreaName.GetWindowText(name);
	if(name == "")
	{
		MessageBox("请填区域名字 或 选择已有的区域名字!!", "!*_*!:", MB_OK);
		return;
	}
	CColorDialog  setColorDlg;
	COLORREF color;
	g_pMapView->PauseRender();
	if(IDOK == setColorDlg.DoModal())
	{
		color = setColorDlg.GetColor();
		if(g_pCache->GetAreaData(name))
		{
			CString text = name + " 已经设置过颜色,确定要修改吗?";
			if(IDOK == MessageBox(text, "",MB_OKCANCEL))
			{
				COLORREF oldColor = g_pCache->GetAreaColor(name);
				if( !(SetAreaColor(name, color)))
				{
					return;
				}
				else
				{
					m_mapView.ReplaceColor(oldColor, color);
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			if(!SetAreaColor(name, color))
			{
				return;
			}
		}
	}
}

void CNewSceneEditorDlg::ClearArea()
{
	m_ComboAreaName.ResetContent();
	g_pCache->ClearAreaData();
	g_pMapView->ClearArea();
}


void CNewSceneEditorDlg::SelectArea(const CString& name)
{
	int index  = m_ComboAreaName.FindString(-1, name);
	m_ComboAreaName.SetCurSel(index);
	OnCbnSelchangeAreaName();
}

COLORREF CNewSceneEditorDlg::GetCurAreaColor()
{
	CString name;
	m_ComboAreaName.GetWindowText(name);
	if(name == "")
	{
		MessageBox("请填区域名字 或 选择已有的区域名字!!", "!*_*!:", MB_OK);
		return -1;
	}
	COLORREF color = g_pCache->GetAreaColor(name) ;
	if (color == -1)
	{
		MessageBox("请设置区域颜色", "!*_*!:", MB_OK);
		return -1;
	}
	return color ;
}



//设置成功返回true 失败返回false(颜色已经被使用过)
bool CNewSceneEditorDlg::SetAreaColor(const CString& areaName, COLORREF color)
{
	if (g_pCache->IsAreaColorUsed(color))
	{
		MessageBox("设置的颜色已经被使用过,请重新设置", MB_OK);
		return false;
	}
	if (areaName != "主路" && areaName.Find("主路") >= 0)
	{
		MessageBox("主路 通用的特殊功能区域, 其他区域名不能含 '主路' 二字", MB_OK);
		return false;
	}
	
	if (!g_pCache->GetAreaData(areaName))
	{
		m_ComboAreaName.AddString(areaName);
		g_pCache->AddArea(areaName);
	}
	g_pCache->SetAreaColor(areaName, color);
	return true;
}

bool CNewSceneEditorDlg::AreaRename(const CString& oldName, const CString& newName)
{
	if(oldName == newName || newName == "")
	{
		return false;
	}
	if (g_pCache->GetAreaData(newName))
	{
		//重名
		return false;
	}

	if (g_pCache->ModifyAreaName(oldName, newName))
	{
		m_ComboAreaName.DeleteString(m_ComboAreaName.FindString(-1, oldName));
		m_ComboAreaName.AddString(newName);
		return true;
	}
	else
	{
		return false;
	}
}

bool CNewSceneEditorDlg::DelArea(const CString& areaName)
{	
	
	if (!g_pCache->GetAreaData(areaName))
	{
		return false;
	}
	int color = g_pCache->GetAreaColor(areaName);
	g_pCache->DelArea(areaName);
	m_ComboAreaName.DeleteString(m_ComboAreaName.FindString(-1, areaName));
	m_mapView.DelColor(color);
	return true;
}

int CNewSceneEditorDlg::GetPenSize()
{
	return m_SliderPenSize.GetPos() * 2 -1;
}

void CNewSceneEditorDlg::SaveAreaInfo(FILE* pFile)
{

	char szSave[CHAT_BUFFER_SIZE];
	const char szAreaNames[] = "\n\nlocal areaNames ={";
	const char szAreaName[] = "\n[%d] = \"%s\",";
	const char szAreaNamesEnd[] = "\n}";
	const char szAreaRegion[] = "local region = {";
	const char szAreaRegionEnd[] = "\n}";
	const char szAreaLine[] = "\n[%d] = {";
	const char szAreaSegment[] = " %d,%d,%d,";
	const char szAreaLineEnd[] = " },";
	
	int x,y;
	int iStart, iEnd;
	int color;
	int width = m_mapView.GetMapGridWidth();
	int height = m_mapView.GetMapGridHeight();
	CString name;
	bool isWriteLineBegin;
	
	int areaCount = 0;
	m_vecAreaTemp.clear();  //用来记录使用过的area
	int areaIndex;

	strcpy(szSave, szAreaRegion);
	ConvertToUtf8(szSave);
	fwrite(szSave, strlen(szSave), 1, pFile);
	for (x = 0; x < width; x++)
	{
		isWriteLineBegin = false;
		y = 0;
		while ( y < height)
		{
			color = m_mapView.GetAreaColor(x,y);
			if(color == -1)
			{
				y ++;
				continue;
			}
			iStart = y;
			for (iEnd = iStart + 1; iEnd < height; iEnd ++)
			{
				if (color != m_mapView.GetAreaColor(x,iEnd))
				{
					break;
				}
			}

			if ( !isWriteLineBegin)
			{
				isWriteLineBegin = true;
				sprintf(szSave, szAreaLine, x);
				ConvertToUtf8(szSave);
				fwrite(szSave, strlen(szSave), 1, pFile);
			}

			name = g_pCache->GetAreaName(color);
			for (areaIndex = m_vecAreaTemp.size() -1; areaIndex >= 0 ; areaIndex--)
			{
				if(m_vecAreaTemp[areaIndex] == name)
				{
					break;
				}
			}
			if (areaIndex == -1) //新的区域
			{
				areaIndex = m_vecAreaTemp.size();
				m_vecAreaTemp.push_back(name);
			}
			
			sprintf(szSave, szAreaSegment, iStart, iEnd -1, areaIndex + 1);
			ConvertToUtf8(szSave);
			fwrite(szSave, strlen(szSave), 1, pFile);
			
			y = iEnd;
		}
		if (isWriteLineBegin)
		{
			strcpy(szSave, szAreaLineEnd);
			ConvertToUtf8(szSave);
			fwrite(szSave, strlen(szSave), 1, pFile);
		}
	}
	strcpy(szSave, szAreaRegionEnd);
	ConvertToUtf8(szSave);
	fwrite(szSave, strlen(szSave), 1, pFile);

	strcpy(szSave, szAreaNames);
	ConvertToUtf8(szSave);
	fwrite(szSave,strlen(szSave), 1, pFile );
	for (uint i = 0; i < m_vecAreaTemp.size(); i ++)
	{
		sprintf(szSave, szAreaName , i+1, m_vecAreaTemp[i]);
		ConvertToUtf8(szSave);
		fwrite(szSave, strlen(szSave), 1, pFile);
	}
	strcpy(szSave, szAreaNamesEnd);
	ConvertToUtf8(szSave);
	fwrite(szSave, strlen(szSave), 1, pFile);
}

bool CNewSceneEditorDlg::SaveTransArea(string savePath)
{

    string file = savePath;
	FILE* pFile  = fopen(file.c_str(), "wb");
	if (pFile == NULL)
	{
		return false ;
	}
	char szSave[CHAT_BUFFER_SIZE];

    SaveAreaInfo(pFile);

    const char szEnd[] = "\n\nInitSceneAreaTransportSetting(\"%s\",areaNames,region)\n\n";
    sprintf(szSave, szEnd, m_strCurSceneName.GetBuffer());
    ConvertToUtf8(szSave);
	fwrite(szSave, strlen(szSave), 1, pFile);
	fclose(pFile);
	return true;
}

bool CNewSceneEditorDlg::SaveArea(string savePath)
{
     
	string file = savePath;
	FILE* pFile  = fopen(file.c_str(), "wb");
	if (pFile == NULL)
	{
		return false ;
	}
    size_t pos = savePath.rfind('\\');
	if (pos == -1)
	{
		pos = savePath.rfind('/');
	}

	string luaName = savePath.erase(0,pos+1);
	pos = luaName.rfind(".lua");
	string lName = luaName.erase(pos,pos+4); 

	char szSave[CHAT_BUFFER_SIZE];
    
	SaveAreaInfo(pFile);

	const char szEnd[] = "\n\ng_AreaMgr:InitSceneAreaSetting(\"scene/%s\", areaNames, region)\n\n";
	sprintf(szSave, szEnd, lName.c_str());
	ConvertToUtf8(szSave);
	fwrite(szSave, strlen(szSave), 1, pFile);

	const char szMusicBegin[] = "local AreaMusic = {";
	const char szMusicData[] =  "\n[\"%s\"] = \"%s\",";
	const char szMusicEnd[] = "\n}\n\ng_AreaMgr:InitSceneMusicSetting(\"scene/%s\", AreaMusic)";

	bool isSaveBegin = false;
	CString music;
	for (uint i = 0; i < m_vecAreaTemp.size(); i ++)
	{
		music = g_pCache->GetAreaMusic(m_vecAreaTemp[i]);
		if (music != "无")
		{
			if (!isSaveBegin)
			{
				strcpy(szSave, szMusicBegin);
				ConvertToUtf8(szSave);
				fwrite(szSave, strlen(szSave), 1, pFile);
				isSaveBegin = true;
			}
			sprintf(szSave, szMusicData,  m_vecAreaTemp[i],  music);
			ConvertToUtf8(szSave);
			fwrite(szSave, strlen(szSave), 1, pFile);
		}
	}
	
	if (isSaveBegin)
	{
		sprintf(szSave, szMusicEnd, lName.c_str());
		ConvertToUtf8(szSave);
		fwrite(szSave, strlen(szSave), 1, pFile);
	}

	//保存区域副本属性
	const char szFbStateBegin[] = "\n\nlocal AreaFb = {";
	const char szFbState[] =  "\n[\"%s\"] = %d,";
	const char szFbStateEnd[] =  "\n}\ng_AreaMgr:InitAreaState(\"scene/%s\", AreaFb)";
	
	bool isFb;
	isSaveBegin = false;
	for (uint i = 0; i < m_vecAreaTemp.size(); i ++)
	{
		isFb = g_pCache->GetAreaFbState(m_vecAreaTemp[i]);
		if (isFb)
		{
			if (!isSaveBegin)
			{
				strcpy(szSave, szFbStateBegin);
				ConvertToUtf8(szSave);
				fwrite(szSave, strlen(szSave), 1, pFile);
				isSaveBegin = true;
			}
			sprintf(szSave, szFbState,  m_vecAreaTemp[i],  g_pCache->GetAreaMaxPlayerNum(m_vecAreaTemp[i]));
			ConvertToUtf8(szSave);
			fwrite(szSave, strlen(szSave), 1, pFile);
		}
	}
	
	if (isSaveBegin)
	{
		sprintf(szSave, szFbStateEnd, lName.c_str());
		ConvertToUtf8(szSave);
		fwrite(szSave, strlen(szSave), 1, pFile);
	}
	

	const char szRatingPlayerBegin[] = "\n\nlocal RatingPlayerTbl = {";
	const char szRatingPlayer[] =  "\n[\"%s\"] = %d,";
	const char szRatingPlayerEnd[] =  "\n}\ng_AreaMgr:InitRatingPlayerTbl(\"scene/%s\", RatingPlayerTbl)";
	
	int ratingPlayerNum;
	isSaveBegin = false;
	for (uint i = 0; i < m_vecAreaTemp.size(); i ++)
	{
		ratingPlayerNum = g_pCache->GetAreaRatingPlayerNum(m_vecAreaTemp[i]);
		if (ratingPlayerNum > 0)
		{
			if (!isSaveBegin)
			{
				strcpy(szSave, szRatingPlayerBegin);
				ConvertToUtf8(szSave);
				fwrite(szSave, strlen(szSave), 1, pFile);
				isSaveBegin = true;
			}
			sprintf(szSave, szRatingPlayer,  m_vecAreaTemp[i],  ratingPlayerNum);
			ConvertToUtf8(szSave);
			fwrite(szSave, strlen(szSave), 1, pFile);
		}
	}

	if (isSaveBegin)
	{
		sprintf(szSave, szRatingPlayerEnd, lName.c_str());
		ConvertToUtf8(szSave);
		fwrite(szSave, strlen(szSave), 1, pFile);
	}



	fclose(pFile);
	return true;
}


void CNewSceneEditorDlg::InitMusic()
{
	char curPath[CHAT_BUFFER_SIZE];
	::GetCurrentDirectory(CHAT_BUFFER_SIZE,curPath);

	CString str = CSEPathMgr::Inst().GetArtistPath();
	CPkgFile::AddLoadPath(PATH_ALIAS_SCENE_EDITOR.c_str(), gbk_to_utf16((LPCSTR)str).c_str());
	::SetCurrentDirectory(CSEPathMgr::Inst().GetArtistPath().GetBuffer());

	if (!GetAudioModule()->GetAudioSystem()->Open())
	{
		return;
	}
	if(!GetAudioModule()->GetAudioSystem()->LoadSoundBankFromXml("sound/config.xml"))
	{
		MessageBox("无法打开 sound/config.xml");
		return;
	}
	GetAudioModule()->CreateAudioEmitter();
	IAudioSystem* audioSystem = GetAudioModule()->GetAudioSystem();
	if (NULL != audioSystem)
	{
		string str = "", strCueName = "";
		str = "sound/music";
		index_t SoundBankIndex = audioSystem->GetSoundBankIndexByName(str.c_str());
		size_t nCuesCnt = audioSystem->GetNumCues(SoundBankIndex);
		for ( size_t cueIndex = 0; cueIndex < nCuesCnt; ++ cueIndex )
		{
			strCueName = audioSystem->GetCueName(SoundBankIndex, cueIndex);
			m_vecMusicNames.push_back(strCueName.c_str());
		}
	}
	::SetCurrentDirectory(curPath);
}
void CNewSceneEditorDlg::OnCbnEditchangeAreaName()
{
	CString areaName;
	m_ComboAreaName.GetWindowText(areaName);
}

void CNewSceneEditorDlg::OnCbnSelchangeAreaName()
{
	CString areaName;
	int index = m_ComboAreaName.GetCurSel();
	if(index < 0)
	{
		return;
	}
	m_ComboAreaName.GetLBText(index, areaName);
}


CString CNewSceneEditorDlg::GetBarrierObjName()
{
	CString BarrierObjName;
	m_ComboBarrierObj.GetWindowText(BarrierObjName);
	return BarrierObjName;
}
void CNewSceneEditorDlg::OnBnClickedFindInBarrier()
{
	m_mapView.ClearSelectElement();
	for (CCache::NpcBC_Iter iter = g_pCache->m_bcNpc.begin(); iter != g_pCache->m_bcNpc.end(); ++iter)
	{
		if (m_mapView.IsBlock((*iter)->fPos))
		{
			m_mapView.SetScaling(MAX_SCALING);
			m_SliderScaling.SetPos(m_SliderScaling.GetRangeMax());
			m_mapView.MoveMapToCenter((*iter)->fPos);
			m_mapView.AddSelectElement(*iter);
			return;
		}
	}
	for (CCache::ObjBC_Iter iter = g_pCache->m_bcObj.begin(); iter != g_pCache->m_bcObj.end(); ++iter)
	{
		if (m_mapView.IsBlock((*iter)->fPos))
		{
			m_mapView.SetScaling(MAX_SCALING);
			m_SliderScaling.SetPos(m_SliderScaling.GetRangeMax());
			m_mapView.MoveMapToCenter((*iter)->fPos);
			m_mapView.AddSelectElement(*iter);
			return;
		}
	}
	for (CCache::TrapBC_Iter iter = g_pCache->m_bcTrap.begin(); iter != g_pCache->m_bcTrap.end(); ++iter)
	{
		if (m_mapView.IsBlock((*iter)->fPos))
		{
			m_mapView.SetScaling(MAX_SCALING);
			m_SliderScaling.SetPos(m_SliderScaling.GetRangeMax());
			m_mapView.MoveMapToCenter((*iter)->fPos);
			m_mapView.AddSelectElement(*iter);
			return;
		}
	}
	for (CCache::PathKeyPosBC_Iter iter = g_pCache->m_bcPathKeyPos.begin(); iter != g_pCache->m_bcPathKeyPos.end(); ++iter)
	{
		if (m_mapView.IsBlock((*iter)->fPos))
		{
			m_mapView.SetScaling(MAX_SCALING);
			m_SliderScaling.SetPos(m_SliderScaling.GetRangeMax());
			m_mapView.MoveMapToCenter((*iter)->fPos);
			m_mapView.AddSelectElement(*iter);
			return;
		}
	}
	MessageBox("没有在障碍内发现物体 ^.^");
}

CString CNewSceneEditorDlg::GetCurSceneName()
{
	return m_strCurSceneName;
}

void CNewSceneEditorDlg::OnClose()
{
	if (MessageBox("是否关闭摆怪编辑器. 请注意保存数据.","", MB_OKCANCEL) == IDOK)
		OnOK(); 
}

void CNewSceneEditorDlg::OnBnClickedAreaMusicEdit()
{
	g_pMapView->PauseRender();
	CAreaEditDlg areaEidDlg;
	areaEidDlg.DoModal();
}

void CNewSceneEditorDlg::OnBnClickedMoverMap()
{
	if(!m_mapView.IsMapOk())
	{
		return;
	}
	CString s;
	FPos gridFPos(0,0);
	GetDlgItem(IDC_GRID_X)->GetWindowText(s);
	if(s != "")
	{
		gridFPos.x = (float)atof(s.GetBuffer());
		if (gridFPos.x < 0)
		{
			gridFPos.x = 0;
		}
		else if(gridFPos.x> m_mapView.GetMapGridWidth())
		{
			gridFPos.x = (float)m_mapView.GetMapGridWidth();
		}
	}
	GetDlgItem(IDC_GRID_Y)->GetWindowText(s);
	if(s != "")
	{
		gridFPos.y = (float)atof(s.GetBuffer());
		if (gridFPos.y < 0)
		{
			gridFPos.y = 0;
		}
		else if(gridFPos.y> m_mapView.GetMapGridHeight())
		{
			gridFPos.y = (float)m_mapView.GetMapGridHeight();
		}
	}
	m_mapView.MoveMapToCenter(gridFPos);
	Pos viewPos;
	m_mapView.GridPosToViewPos(viewPos, gridFPos);
	CPoint pt(viewPos.x, viewPos.y);
	m_mapView.ClientToScreen(&pt);
	SetCursorPos(pt.x, pt.y);
}


void CNewSceneEditorDlg::OnAddElement(CElement* pElement, ETabPageType type)
{
	m_tabPageElement[type].AddElement(pElement);
}

void CNewSceneEditorDlg::OnDelElement(CElement* pElement, ETabPageType type)
{
	m_tabPageElement[type].DelElement(pElement);
}

void CNewSceneEditorDlg::OnUpdateElement(CElement* pElement, ETabPageType type)
{
	m_tabPageElement[type].UpdateElement(pElement);
}

void CNewSceneEditorDlg::OnClearPage(ETabPageType type)
{
	m_tabPageElement[type].ClearElement();
}

void CNewSceneEditorDlg::OnBnClickedSetCenterPos()
{
	if (g_eModeType == eMT_DYNAMIC || g_eModeType == eMT_PATH_EDIT)
	{
		g_pCache->SetCenterPos();
	}
}
void CNewSceneEditorDlg::OnBnClickedSetGridMiddle()
{
	// TODO: Add your control notification handler code here
	m_mapView.MoveAllSelectElementToGridMiddle();
}

void CNewSceneEditorDlg::OnSetOffsetPosition()
{
	g_pCache->SetOffsetPos();
	// TODO: Add your command handler code here
}


void CNewSceneEditorDlg::OnReSaveAllSceneFile()
{
	if (g_eModeType == eMT_NORMAL)
	{	
		int iErrorNum = 0;
		int iSucceedNum = 0;
		int iLeapNum = 0;
		IsLeapCheck = true;
		g_pMapView->PauseRender();
		CString strSceneName;
		CString path;
		COriginalData::SceneCfg_Map::iterator iter= g_pOriginalData->m_mapSceneCfg.begin();
		for (; iter != g_pOriginalData->m_mapSceneCfg.end(); ++iter)
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_ING);
			strSceneName = iter->first;
			cout << strSceneName << "\t\t\t"; 

			if (g_pOriginalData->GetSceneFile(strSceneName) == "")
			{
				cout << "跳过加载           -- 没有摆怪文件" << endl;
				++iLeapNum;
				continue;
			}
			
			m_strCurSceneName = strSceneName;
			g_pCache->ClearAll();
			path = CSEPathMgr::Inst().GetDesignerPath() + '/' +	g_pOriginalData->GetSceneFile(strSceneName) + ".lua";
			LoadScene(path);

			if(!g_pCache->SaveSceneFile(path.GetBuffer()))
			{
				cout << "保存失败!" << endl;
				++iErrorNum;
			}
			else
			{
				cout << "保存成功" << endl;
				++iSucceedNum;
			}
		}
		g_pCache->ClearAll();
		CString strMsg;
		strMsg.Format("共 %d 张地图\n成功保存 %d 张\n跳过 %d 张\n失败 %d 张", g_pOriginalData->m_mapSceneCfg.size(),iSucceedNum, iLeapNum,iErrorNum);
		cout << endl << "----------------------------------------------------------" <<endl;
		cout << strMsg << endl;
		MessageBox(strMsg);

		IsLeapCheck = false;
	}
	else
	{
		MessageBox("需要在 普通编辑模式 下使用");
	}
}


void CNewSceneEditorDlg::OnReSaveAllAreaFile()
{
	if (g_eModeType == eMT_NORMAL)
	{	
		int iErrorNum = 0;
		int iSucceedNum = 0;
		int iLeapNum = 0;
		IsLeapCheck = true;
		g_pMapView->PauseRender();
		CString strSceneName;
		CString errorMessage;
		CString strArtistPath;
		CString strMapPath;
		CString strAreaPath;
		COriginalData::SceneCfg_Map::iterator iter= g_pOriginalData->m_mapSceneCfg.begin();
		for (; iter != g_pOriginalData->m_mapSceneCfg.end(); ++iter)
		{
			CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_ING);
			strSceneName = iter->first;
			cout << strSceneName << "\t\t\t"; 
			if (g_pOriginalData->GetAreaFile(strSceneName) == "")
			{
				CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
				cout << "跳过加载           -- 没有区域文件" << endl;
				++iLeapNum;
				continue;
			}
			strArtistPath = g_pOriginalData->GetMapFile(strSceneName);
			if (strArtistPath  == "")
			{
				CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
				cout << "跳过加载           -- 没有资源文件" << endl;
				++iLeapNum;
				continue;
			}
			strArtistPath  = CSEPathMgr::Inst().GetArtistPath()  + '/' + strArtistPath;
			strMapPath = GetMapPicturePath(strSceneName);
			if (strMapPath == "")
			{
				CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
				cout << "跳过加载           -- 没有图片" << endl;
				++iLeapNum;
				continue;
			}
			
			
			if(m_mapView.LoadMap(strMapPath, strArtistPath, errorMessage))
			{
				strAreaPath = CSEPathMgr::Inst().GetDesignerPath() + '/' +	g_pOriginalData->GetAreaFile(strSceneName) + ".lua";
				LoadArea(strAreaPath);
			}
			else
			{
				CStateMgr::Inst().DoEvent(eEVENT_LOAD_MAP_FAILD);
				cout << "加载地图失败  " << errorMessage.GetBuffer() << endl;
				++iErrorNum;
				continue;
			}

			if(!SaveArea(strAreaPath.GetBuffer()))
			{
				cout << "保存失败!" << endl;
				++iErrorNum;
			}
			else
			{
				cout << "保存成功" << endl;
				++iSucceedNum;
			}
		}
		g_pCache->ClearAll();
		CString strMsg;
		strMsg.Format("共 %d 张地图\n成功保存 %d 张\n跳过 %d 张\n失败 %d 张", g_pOriginalData->m_mapSceneCfg.size(),iSucceedNum, iLeapNum,iErrorNum);
		cout << endl << "----------------------------------------------------------" <<endl;
		cout << strMsg << endl;
		MessageBox(strMsg);

		IsLeapCheck = false;
	}
	else
	{
		MessageBox("需要在 普通编辑模式 下使用");
	}
}

void CNewSceneEditorDlg::OnReSaveAllDynamicFile()
{
	if (g_eModeType == eMT_DYNAMIC)
	{
		int iErrorNum = 0;
		int iSucceedNum = 0;
		IsLeapCheck = true;
		g_pMapView->PauseRender();
		map<CString, CString>::iterator iter = g_pCache->m_mapDynamicFile.begin();
		for (; iter != g_pCache->m_mapDynamicFile.end(); ++iter)
		{
			g_pCache->LoadDynamicData(iter->first);
			if(g_pCache->SaveDynamicFile())
			{
				++iSucceedNum;
			}
			else
			{
				++iErrorNum;
			}
		}
		g_pCache->ClearAll();
		CString strMsg;
		strMsg.Format("成功保存 %d 个文件\n失败 %d 个文件",iSucceedNum, iErrorNum);
		MessageBox(strMsg);
		
		IsLeapCheck = false;
	}
	else
	{
		MessageBox("需要在 动态文件编辑模式 下使用");
	}
}

void CNewSceneEditorDlg::SetCurColonyEditDlg(CColonyEditDlg* pDlg)
{
	m_pCurColonyEditDlg = pDlg;
}

CColonyEditDlg* CNewSceneEditorDlg::GetCurColonyEditDlg()
{
	return m_pCurColonyEditDlg;
}

void CNewSceneEditorDlg::OnBnClickedRuler()
{
	m_mapView.Measure();
}
void CNewSceneEditorDlg::OnBnClickedRotate()
{
	m_mapView.RotateElement();
}