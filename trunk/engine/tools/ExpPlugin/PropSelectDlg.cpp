#include "stdafx.h"
#include "Exp.h"
#include "ExpPlugin.h"
#include "PropSelectDlg.h"
#include "Phyexp.h"
#include "Bipexp.h"
#include "ISkin.h"
#include ".\propselectdlg.h"
#include <sstream>
#include <algorithm>
// 临时修改，解决编译不过问题
//#include "PathBrowser\PathBrowser.h"
//#include "ProgressDialog\ProgressDialog.h"
#include "CEditorConfig.h"

#include "CPkgDirectory.h"
#include "PathLogDlg.h"
#include "CSkeInfoDlg.h"
// 临时修改，解决编译不过问题
//#include "ParaAdapterCtrl\CModRsTexPathCfgMgr.h"
#include "CPluginMeshBase.h"
#include "CPluginDataSources.h"


extern vector<BoxNode> g_vecNode;

BOOL	g_IsCpSke;
#define MAX_X		0x2000
#define MAX_Y		0x2010
#define MAX_Z       0x2020
#define MIN_X		0x2030
#define MIN_Y		0x2040
#define MIN_Z		0x2050

//////////////////////////////////////////////////////////
// 功能：检查是否骨骼
// 参数：INode* MaxNode     3DMAX节点
// 返回：略
//////////////////////////////////////////////////////////
bool IsSkeletal( INode* MaxNode )
{
	ObjectState os = MaxNode->EvalWorldState(0);
	if ( os.obj && os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID ) 
	{
		if( BONE_OBJ_CLASSID == os.obj->ClassID() )
			return true;

		Control *c = MaxNode->GetTMController();
		if( BIPSLAVE_CONTROL_CLASS_ID == c->ClassID() || 
			BIPBODY_CONTROL_CLASS_ID == c->ClassID() ||
			FOOTPRINT_CLASS_ID == c->ClassID() )
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////
// 功能：检查是否MESH
// 参数：INode* MaxNode     3DMAX节点
// 返回：略
//////////////////////////////////////////////////////////
bool IsNodeMesh( INode* MaxNode, Interface* pInterface )
{
	Object* obj = MaxNode->EvalWorldState( pInterface->GetAnimRange().Start() ).obj;

	if( obj )
		return (TRUE == obj->CanConvertToType( triObjectClassID ));
	else
		return false;
}

//////////////////////////////////////////////////////////
// 功能：得到节点的类型
// 参数：INode* MaxNode     3DMAX节点
// 返回：节点的类型
//////////////////////////////////////////////////////////
NODETYPE GetNodeType( INode* MaxNode, Interface* pInterface, BOOL IsSelect )
{
	assert( MaxNode );

	//检查是否骨骼节点
	if( IsSkeletal( MaxNode ) )
		return enumNT_SKELETAL;

	// 检查是不是MESH
	if( IsNodeMesh( MaxNode, pInterface ) && MaxNode->Renderable() && !MaxNode->IsHidden() )
	{
		// 确定当前MESH需要输出
		if( IsSelect && !MaxNode->Selected() )
			return enumNT_UNKNOW;

		// 通过在修改堆栈中查找PHYSIQUE修改器决定是否骨骼动画
		Object* obj = MaxNode->GetObjectRef();
		while( obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject* derObj = (IDerivedObject*)obj;
			for( int modStackIndex = 0; modStackIndex < derObj->NumModifiers(); modStackIndex++ )
			{
				Modifier* mod = derObj->GetModifier( modStackIndex );
				if( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
					return enumNT_SKELETALMESH;
				if( mod->ClassID() == SKIN_CLASSID  )
					return enumNT_SKELETALMESH;
				if( mod->ClassID() == BP_CLASS_ID_OSM  )
					return enumNT_SKELETALMESH;
			}
			obj = derObj->GetObjRef();
		}
		return enumNT_MESH;
	}

	return enumNT_UNKNOW;
}
NODECPTYPE GetNodeCpType(INode* MaxNode, Interface* pInterface, BOOL IsSelect )
{
	return enumNT_COMPRESS;
}
//////////////////////////////////////////////////////////
// 功能：历遍所有节点并且填充到相应的结构里面
// 参数：INode* MaxNode     当前3DMAX节点, 
//       ExportNode* Parent 上一级输出节点
// 返回：新创建的当前节点
//////////////////////////////////////////////////////////
UINT NodeEnum( INode* MaxNode, Interface* pInterface, BOOL IsSelect, vector<ListNode*>& List, CPiecePropDlg* PieceDlg, UINT& nSkeletal )
{	
	if( !MaxNode )
		return 0;

	string nodeName = MaxNode->GetName();
	string boxName ;
	int posABox = nodeName.rfind("&");
	if(posABox!=string::npos)
	{
		boxName = nodeName.substr(posABox+1,nodeName.size());
		if(boxName=="ABox")
		{
			BoxNode tboxnode;
			tboxnode.m_Node = MaxNode;
			tboxnode.m_BoxName = MaxNode->GetName();

			float maxX,maxY,maxZ,minX,minY,minZ;
			AppDataChunk *chunk;
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MAX_X);
			if (chunk)
			{
				float *f;
				f = (float*)chunk->data;
				maxX = *f;
			}
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MAX_Y);
			if(chunk)
			{
				float *f;
				f = (float*)chunk->data;
				maxY = *f;
			}
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MAX_Z);
			if(chunk)
			{
				float *f;
				f = (float*)chunk->data;
				maxZ = *f;
			}
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MIN_X);
			if(chunk)
			{
				float *f;
				f = (float*)chunk->data;
				minX = *f;
			}
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MIN_Y);
			if(chunk)
			{
				float *f;
				f = (float*)chunk->data;
				minY = *f;
			}
			chunk = MaxNode->GetAppDataChunk(triObjectClassID,DUMMY_CLASS_ID,MIN_Z);
			if(chunk)
			{
				float *f;
				f = (float*)chunk->data;
				minZ = *f;
			}

			tboxnode.m_vMaximum.x = maxX;
			tboxnode.m_vMaximum.y = maxY;
			tboxnode.m_vMaximum.z = maxZ;
			tboxnode.m_vMinimum.x = minX;
			tboxnode.m_vMinimum.y = minY;
			tboxnode.m_vMinimum.z = minZ;
			

			int	posId = nodeName.rfind("&",posABox-1);
			tboxnode.m_Id = atoi(nodeName.substr(posId+1,posABox-posId-1).c_str());
			int posBone = nodeName.rfind("&",posId-1);
			tboxnode.m_BoneName = nodeName.substr(posBone+1,posId-posBone-1);
			int posMesh = nodeName.rfind("&",posBone-1);
			tboxnode.m_MeshName = nodeName.substr(posMesh+1,posBone-posMesh-1);

			g_vecNode.push_back(tboxnode);
		}
	}


	//填充当前节点
	NODETYPE NodeType = GetNodeType( MaxNode, pInterface, IsSelect );

	if( NodeType == enumNT_SKELETAL )
		nSkeletal++;
	
	//将有效的网格归类
	if( NodeType == enumNT_MESH || NodeType == enumNT_SKELETALMESH || NodeType == enumNT_SKELETAL )
	{
		ListNode* Node = new ListNode( NodeType, MaxNode );
		List.push_back( Node );
		if( PieceDlg && ( NodeType == enumNT_MESH || NodeType == enumNT_SKELETALMESH ) )
            PieceDlg->m_ExpTypeSelect.InsertItem( LVIF_TEXT|LVIF_PARAM, PieceDlg->m_ExpTypeSelect.GetItemCount(), 
			MaxNode->GetName(), 0, 0, 0, (LPARAM)Node );
	}
	
	UINT NodeNum = 0;
	//继续考察子节点
	for ( int i=0; i<MaxNode->NumberOfChildren(); i++ )
	{
		int re = NodeEnum( MaxNode->GetChildNode(i), pInterface, IsSelect, List, PieceDlg, nSkeletal );
		if ( !re )
			return 0;
		NodeNum += re;
	}
	NodeNum ++;

	return NodeNum;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CPropSelectDlg dialog

IMPLEMENT_DYNAMIC(CPiecePropDlg, CDialog)
CPiecePropDlg::CPiecePropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPiecePropDlg::IDD, pParent)
	, m_WeightPerVertex(2)
	, m_FramePerSample(1)
	, m_SmoothAll(TRUE)
{
}

CPiecePropDlg::~CPiecePropDlg()
{
}

void CPiecePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WEIGHTPERVERTEX, m_WeightPerVertex);
	DDX_Text(pDX, IDC_FRAMEPERSAMPLE, m_FramePerSample);
	DDX_Control(pDX, IDC_EXPTYPESELECT, m_ExpTypeSelect);
	DDX_Check(pDX, IDC_SMOOTH_ALL, m_SmoothAll);
}


BEGIN_MESSAGE_MAP(CPiecePropDlg, CDialog)
	ON_BN_CLICKED(IDC_SETPATHBTN, OnBnSetPathBtn)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CPiecePropDlg::OnBnSetPathBtn()
{
// 临时修改，解决编译不过问题
	//m_ParaAdapterDlg.MyDoModal();
}

// CPropSelectDlg message handlers

BOOL CPiecePropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UINT nSkeletal = 0;
	//if(!g_vecNode.empty())
	//	g_vecNode.clear();
	m_ExpTypeSelect.InsertColumn( 0, "物件名字", LVCFMT_LEFT, 80 );
	m_ExpTypeSelect.InsertColumn( 1, "顶点动画", LVCFMT_LEFT, 60 );
	m_ExpTypeSelect.InsertColumn( 2, "骨骼动画", LVCFMT_LEFT, 60 );
	m_ExpTypeSelect.InsertColumn( 3, "无动画",   LVCFMT_LEFT, 50 );
	m_SaveParam.m_uiSP_NodeNum = NodeEnum( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, this, nSkeletal );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


INT_PTR CPiecePropDlg::DoModal( CString FileName, Interface* pInterface, BOOL Select ,BOOL IsExist)
{
	// TODO: Add your specialized code here and/or call the base class
	if(!pInterface||!FileName.GetLength())
		return 0;

// 临时修改，解决编译不过问题
	//new CModRsTexPathCfgMgr;

	TCHAR szFull[_MAX_PATH];
	GetModuleFileName(NULL,szFull,_MAX_PATH);
	string configPath = szFull;
	int pos = configPath.rfind("\\");
	configPath = configPath.substr(0,pos);
	configPath += "/EditorConfig.xml";

	if(!CEditorConfig::GetInst())
		CEditorConfig::GetInst()->InitEditorConfig(configPath.c_str());
	CDataSources::SetNull();
	new CPluginDataSources;

// 临时修改，解决编译不过问题
	//CModRsTexPathCfgMgr::Inst()->ReadXml();

	//CModRsTexPathCfgMgr::Inst()->ClearParaKey();
	string path = FileName.GetBuffer();
	if(path.rfind("\\")!=string::npos)
	{
		int pos = path.rfind("\\");
		path = path.substr(0,pos);
// 临时修改，解决编译不过问题
		//CModRsTexPathCfgMgr::Inst()->SetParaKey(path.c_str());
	}

	FileName.MakeLower();
	g_vecNode.clear();

	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();
	m_pInterface    = pInterface;
	m_Select        = Select;

	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_Model.SetGlobeIsExist(IsExist==TRUE);

	if( CDialog::DoModal() == IDOK )
	{
		if(IsExist)
			m_Model.GetOldModInfoFromFile((LPCTSTR)FileName);

		map< INode*, PIECETYPE > NodeType;
		for( int i = 0; i<(int)m_NodeType.size(); i++ )
			NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );
		
		m_SaveParam.m_uiSP_pInterface		= pInterface;
		m_SaveParam.m_uiSP_Select			= Select;
		m_SaveParam.m_uiSP_WeightPerVertex	= m_WeightPerVertex;
		m_SaveParam.m_uiSP_FramePerSample	= m_FramePerSample;
		m_SaveParam.m_uiSP_AnimationName	= "NotUsed";
		m_SaveParam.m_uiSP_pNodeType		= &NodeType;
		m_SaveParam.m_eSP_SaveType			= eST_Piece;
		m_SaveParam.m_bSP_SmoothAll			= m_SmoothAll;


		if(SUCCEEDED(m_Model.FillFrom3DMax( m_SaveParam ) ) )
		{	
			if(IsExist&&!m_Model.IsMatching())
			{
				::MessageBox(NULL,"两个模型的piece数量不相等","错误",MB_OK);
				return IMPEXP_CANCEL;
			}
			CPieceCompressPropDlg DlgCp;
			int	hr = (int)DlgCp.DoModal(&m_Model);
			if(SUCCEEDED(m_Model.Save((LPCTSTR)FileName)))
			{
				m_Model.ClearModData();
				// 临时修改，解决编译不过问题
				//if(CModRsTexPathCfgMgr::Inst()->m_ParaKey!="")
				{
					CPathLogDlg pathlogdlg;
					//for ( size_t i = 0; i < CModRsTexPathCfgMgr::Inst()->GetPathLogNum(); ++i )
					//{
					//	pathlogdlg.AddPath(CModRsTexPathCfgMgr::Inst()->GetPathLog(i));
					//}

					pathlogdlg.MyDoModal();
				}
                return IMPEXP_SUCCESS;
			}
		}
	}
	else
		return IMPEXP_CANCEL;

	return 0;
}

void CPiecePropDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	CDialog::OnOK();
}

void CPiecePropDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData();
	m_WeightPerVertex = m_WeightPerVertex + pNMUpDown->iDelta;
	if( m_WeightPerVertex < 0 )
        m_WeightPerVertex = 0;
	if( m_WeightPerVertex > 4 )
        m_WeightPerVertex = 4;
	UpdateData(FALSE);
}

void CPiecePropDlg::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData();
	m_FramePerSample = m_FramePerSample + pNMUpDown->iDelta;
	if( m_FramePerSample < 1 )
        m_FramePerSample = 1;
	if( m_FramePerSample > 99 )
        m_FramePerSample = 99;
	UpdateData(FALSE);
}
//


// CSkeletalProp dialog

IMPLEMENT_DYNAMIC(CSkeletalProp, CDialog)
CSkeletalProp::CSkeletalProp(CWnd* pParent /*=NULL*/)
	: CDialog(CSkeletalProp::IDD, pParent)
	, m_AniName(_T(""))
	, m_RootScale(FALSE)
	, m_IsExpAllFrame(TRUE)
	, m_IsCpSke(FALSE)
{
}

CSkeletalProp::~CSkeletalProp()
{
}

void CSkeletalProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANINAME, m_AniName);
	DDX_Check(pDX, IDC_CHECK1, m_RootScale);
	DDX_Check(pDX, IDC_CHECK3, m_IsExpAllFrame);
	DDX_Check(pDX, IDC_CHECK4, m_IsCpSke);
}


BEGIN_MESSAGE_MAP(CSkeletalProp, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSkeletalProp message handlers

BOOL CSkeletalProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	UINT nSkeletal = 0;
	g_vecNode.clear();
	m_SaveParam.m_uiSP_NodeNum = NodeEnum( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, NULL, nSkeletal );
	TCHAR Buf[256];
	sprintf( Buf, "输入动画名字( 骨骼数:%d )", nSkeletal );
	SetWindowText( Buf );

	::GetWindowText( m_pInterface->GetMAXHWnd(), Buf, 256 );
	for( int i = 255; i > 0; i-- ) 
		if( Buf[i] == '.' )
			break;
	Buf[i] = 0;
	m_AniName = Buf;
	GetDlgItem( IDC_ANINAME )->SetWindowText( m_AniName );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSkeletalProp::OnOK()
{
	UpdateData();
	if( m_AniName.IsEmpty() )
	{
		MessageBox( "请输入动画名字","提示");
		return;
	}

	CDialog::OnOK();
}

void	CSkeletalProp::ReadARA(CString FileName,Interface* pInterface, BOOL Select)
{
	map< INode*, PIECETYPE > NodeType;
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );

	m_SaveParam.m_uiSP_pInterface		= pInterface;
	m_SaveParam.m_uiSP_Select			= Select;
	m_SaveParam.m_uiSP_WeightPerVertex	= 2;
	m_SaveParam.m_uiSP_FramePerSample	= 1;
	m_SaveParam.m_uiSP_AnimationName	= m_AniName;

	m_SaveParam.m_uiSP_pNodeType		= &NodeType;
	m_SaveParam.m_eSP_SaveType			= eST_Skeletal;
	m_SaveParam.m_bSP_RootScale			= m_RootScale;
	m_Model.SetExpFrame(m_IsExpAllFrame);
	if(SUCCEEDED(m_Model.FillFrom3DMax( m_SaveParam ) ) )
	{
		string name = FileName.GetBuffer(FileName.GetLength());
		replace(name.begin(), name.end(), '\\', '/');
		m_Model.SetIndxSkeName(name);
	}

	string aniname = m_AniName.GetBuffer(m_AniName.GetLength());
	m_Model.FillFromFile( ((LPCTSTR)FileName ), aniname) ;
}

INT_PTR CSkeletalProp::FillOriAra(CString FileName, Interface* pInterface, BOOL Select )
{
	map< INode*, PIECETYPE > NodeType;
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );

	m_SaveParam.m_uiSP_pInterface		= pInterface;
	m_SaveParam.m_uiSP_Select			= Select;
	m_SaveParam.m_uiSP_WeightPerVertex	= 2;
	m_SaveParam.m_uiSP_FramePerSample	= 1;
	m_SaveParam.m_uiSP_AnimationName	= m_AniName;

	m_SaveParam.m_uiSP_pNodeType		= &NodeType;
	m_SaveParam.m_eSP_SaveType			= eST_Skeletal;
	m_SaveParam.m_bSP_RootScale			= m_RootScale;
	m_Model.SetExpFrame(m_IsExpAllFrame);
	if(SUCCEEDED(m_Model.FillFrom3DMax( m_SaveParam ) ) )
		return IMPEXP_SUCCESS;
	else
		return IMPEXP_CANCEL;
}

INT_PTR CSkeletalProp::SaveSke(CString FileName)
{
	if(SUCCEEDED(m_Model.Save((LPCTSTR)FileName)))
		return IMPEXP_SUCCESS;
	else
		return IMPEXP_CANCEL;
}

INT_PTR CSkeletalProp::DoModal( CString FileName, Interface* pInterface, BOOL Select ,BOOL IsExist)
{
	// TODO: Add your specialized code here and/or call the base class
	if(!pInterface||!FileName.GetLength())
		return 0;

	FileName.MakeLower();
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();
	m_pInterface    = pInterface;
	m_Select        = Select;

	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( CDialog::DoModal() == IDOK )
	{
		g_IsCpSke = m_IsCpSke;
		if(FillOriAra(FileName,pInterface,Select))
		{
			return SaveSke(FileName);
		}
		else
			IMPEXP_CANCEL;

		//if(IsExist==false)//如果ara文件不存在则直接用以前方式保存
		//	return SaveOriAra(FileName,pInterface,Select);
		//else//如果ara文件存在则进行下面判断
		//{
		//	string strfile = FileName.GetBuffer(FileName.GetLength());
		//	strfile = strfile.substr(0,strfile.size()-4)+".agp";
		//	ReadARA(strfile.c_str(),pInterface,Select);
		//	
		//	if(m_Model.IsFit(strfile.c_str()))
		//	{
		//		if(SUCCEEDED(m_Model.SaveFromFile(strfile)))//读取的ara文件写入到新的ara文件,并且增加max里面将导出的anf路径,并且导出anf文件
		//			return IMPEXP_SUCCESS;
		//	}
		//	else
		//		return IMPEXP_CANCEL;
		//}
	}
	else
		return IMPEXP_CANCEL;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAniGroupProp, CDialog)
CAniGroupProp::CAniGroupProp(CWnd* pParent /*=NULL*/)
: CDialog(CAniGroupProp::IDD, pParent)
, m_AniName(_T(""))
, m_RootScale(FALSE)
, m_IsExpAllFrame(TRUE)
, m_IsCpSke(FALSE)
{
}

CAniGroupProp::~CAniGroupProp()
{
}

void CAniGroupProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANIFILENAME, m_AniName);
	DDX_Check(pDX, IDC_CHECK_IS_EXPALL, m_IsExpAllFrame);
	DDX_Check(pDX, IDC_CHECK_IS_CP, m_IsCpSke);
}


BEGIN_MESSAGE_MAP(CAniGroupProp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSkeletalProp message handlers

BOOL CAniGroupProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	UINT nSkeletal = 0;
	g_vecNode.clear();
	m_SaveParam.m_uiSP_NodeNum = NodeEnum( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, NULL, nSkeletal );
	TCHAR Buf[256];
	sprintf( Buf, "输入动画名字( 骨骼数:%d )", nSkeletal );
	SetWindowText( Buf );

	::GetWindowText( m_pInterface->GetMAXHWnd(), Buf, 256 );
	for( int i = 255; i > 0; i-- ) 
		if( Buf[i] == '.' )
			break;
	Buf[i] = 0;
	m_AniName = Buf;
	GetDlgItem( IDC_ANINAME )->SetWindowText( m_AniName );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAniGroupProp::OnOK()
{
	UpdateData();
	if( m_AniName.IsEmpty() )
	{
		MessageBox( "请输入动画名字","提示");
		return;
	}

	CDialog::OnOK();
}

BOOL CAniGroupProp::ReadSke(CString FileName)
{
	return m_Model.LoadSkeFromFile( ((LPCTSTR)FileName ))==true;
}

BOOL CAniGroupProp::ReadAgp(CString FileName)
{
	m_Model.FillAgpFromFile((LPCTSTR)FileName) ;
	//得到indxname装载ske
	string indxname = m_Model.GetIndxNameFromAgp();
	string skename = FileName.GetBuffer();
	string skepath;

	skepath = m_ResPath;
	skename = skepath + indxname;
	
	if(ReadSke(skename.c_str())==-1)
	{
		MessageBox(skename.c_str(),"载入骨架资源不成功",MB_OK);
		return FALSE;
	}

	//如果自动载入ske没成功就手动指定ske
	//if(!loadSuc)
	//{
	//	if(!PointSke())
	//		return FALSE;
	//}

	m_Model.SetIndxSkeName(indxname);
	//匹配ske
	if(m_Model.IsFit(skename.c_str()))
		return TRUE;
	else
		return FALSE;
}

INT_PTR CAniGroupProp::SaveAgp(CString FileName)
{
	m_Model.SynAniname();
	if(SUCCEEDED(m_Model.SaveAgp((LPCTSTR)FileName)))
		return IMPEXP_SUCCESS;
	else
		return IMPEXP_CANCEL;
}

INT_PTR CAniGroupProp::FillData(CString FileName, Interface* pInterface, BOOL Select )
{
	map< INode*, PIECETYPE > NodeType;
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );

	m_SaveParam.m_uiSP_pInterface		= pInterface;
	m_SaveParam.m_uiSP_Select			= Select;
	m_SaveParam.m_uiSP_WeightPerVertex	= 2;
	m_SaveParam.m_uiSP_FramePerSample	= 1;
	m_SaveParam.m_uiSP_AnimationName	= m_AniName;

	m_SaveParam.m_uiSP_pNodeType		= &NodeType;
	m_SaveParam.m_eSP_SaveType			= eST_Skeletal;
	m_SaveParam.m_bSP_RootScale			= m_RootScale;
	m_Model.SetExpFrame(m_IsExpAllFrame);
	if(SUCCEEDED(m_Model.FillFrom3DMax( m_SaveParam ) ) )
		return IMPEXP_SUCCESS;
	else
		return IMPEXP_CANCEL;
}

BOOL CAniGroupProp::PointSke()
{
	static char Filter[] = " Ske Files (*.ske)|*.ske||";

	CFileDialog Dlg( TRUE, NULL, NULL,  OFN_HIDEREADONLY|OFN_NOCHANGEDIR, Filter );
	if( Dlg.DoModal() == IDCANCEL )
		return FALSE;

	CString FileName = Dlg.GetPathName();

	if( FileName.IsEmpty() )
		return FALSE; 

	ReadSke(FileName);
	if(m_Model.IsFit(FileName))
	{
		string indxName = FileName.GetBuffer();
		m_Model.SetIndxSkeName(indxName);
		return TRUE;
	}
	else
		return FALSE;
}

INT_PTR CAniGroupProp::DoModal( CString FileName, Interface* pInterface, BOOL Select ,BOOL IsExist)
{
	// TODO: Add your specialized code here and/or call the base class
	if(!pInterface||!FileName.GetLength())
		return 0;

	FileName.MakeLower();
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();
	m_pInterface    = pInterface;
	m_Select        = Select;

	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( CDialog::DoModal() == IDOK )
	{
		g_IsCpSke = m_IsCpSke;

		if(!HasResPath())
			return IMPEXP_CANCEL;

		FillData(FileName,pInterface,Select);

		if(IsExist==false)//如果agp文件不存在则新建一个agp,并且自动搜索资源目录下的匹配ske文件
		{
			//同时生成agp和ske,ske生成到res\ske里面,agp自己指定目录
			BACKTYPE tp = IsSaveSke(FileName);
			if(tp ==BACKTYPE_TRUE)//如果允许保存ske文件,则保存ske文件以及agp包
			{
				CSkeInfoDlg SkeDlg;
				SkeDlg.DoSkeModal(m_AniName);
				string skename = m_ResPath;
				if(SkeDlg.m_SkeName.Find(".ske")==-1)
					skename += SkeDlg.m_SkeName+".ske";
				if(SaveSke(skename.c_str())!=IMPEXP_SUCCESS)
				{
					MessageBox(skename.c_str(),"保存不成功",MB_OK);
					return IMPEXP_CANCEL;
				}
				m_Model.SetIndxSkeName(skename);
				if(SaveAgp(FileName)!=IMPEXP_SUCCESS)
				{
					MessageBox(FileName,"保存不成功",MB_OK);
					return IMPEXP_CANCEL;
				}
				return IMPEXP_SUCCESS;
			}
			else if(tp==BACKTYPE_FALSE)//找到匹配的ske文件则直接保存agp包,并且把ske名字给agp的索引
				return SaveAgp(FileName);
			else if(tp==BACKTYPE_CANCEL)
				return IMPEXP_CANCEL;
			return IMPEXP_CANCEL;
		}
		else//如果agp文件存在则进行下面判断
		{
			if(ReadAgp(FileName))
				return SaveAgp(FileName);
			else
				return IMPEXP_CANCEL;
		}
	}
	else
		return IMPEXP_CANCEL;

	return 0;
}

INT_PTR CAniGroupProp::SaveSke(CString FileName)
{
	if(SUCCEEDED(m_Model.Save((LPCTSTR)FileName)))
		return IMPEXP_SUCCESS;
	else
		return IMPEXP_CANCEL;
}
BOOL CAniGroupProp::HasResPath()
{
	TCHAR szFull[_MAX_PATH];
	GetModuleFileName(NULL,szFull,_MAX_PATH);
	string configPath = szFull;
	int pos = configPath.rfind("\\");
	configPath = configPath.substr(0,pos);
	configPath += "/EditorConfig.xml";
	if(!CEditorConfig::GetInst())
		CEditorConfig::GetInst()->InitEditorConfig(configPath.c_str());

// 临时修改，解决编译不过问题
	//CPathBrowser	m_PathBrowser;
	//m_PathBrowser.Initialize();
	string tempRootPath;
	//m_PathBrowser.GetFirstPath( tempRootPath );
	//if(tempRootPath=="")
	//{
	//	m_PathBrowser.DoModal();
	//	m_PathBrowser.GetFirstPath( tempRootPath );
	//}

	if(tempRootPath=="")
	{
		MessageBox("需要指定资源根目录","提示",MB_OK);
		return FALSE;
	}
	else
	{
		m_ResPath = tempRootPath+"\\ske\\";
		return TRUE;
	}
}

BACKTYPE CAniGroupProp::IsSaveSke(CString FileName)
{
	if(!m_Model.CheckTrouser())
		return BACKTYPE_CANCEL;

	// 临时修改，解决编译不过问题
	//CProgressDialog progressDlg;
	//progressDlg.Create(IDD_PROGRESS, this);
	//progressDlg.ShowWindow(SW_SHOW);
	//IProgressCallback* pProgress = &progressDlg;
	//pProgress->SetDescription("骨架匹配中,请等待...");
	//pProgress->SetProgress(0.0f);

	sqr::CPkgDir pkg;
	pkg.Open(L"", "ske","ske");
	int size = pkg.GetFileNum();
	for(int i=0;i<size;i++)
	{
		string filename;
		pkg.GetFileName(i,filename);
		filename = m_ResPath+filename;
		m_Model.LoadSkeFromFile(filename);
		if(m_Model.SkeMatching())
		{
			CString strinfo;
			strinfo.Format("提示:此动作包的指向骨架文件%s",filename.c_str());
			MessageBox(strinfo,FileName,MB_OK);
			m_Model.SetIndxSkeName(filename);
			//pProgress->SetProgress(1.0f);
			return BACKTYPE_FALSE;
		}
		//pProgress->SetProgress((float)i/size);

	}
	//pProgress->SetProgress(1.0f);

	return BACKTYPE_TRUE;
}
//// CSkeletalProp dialog
//
//IMPLEMENT_DYNAMIC(CSoftBodyProp, CDialog)
//CSoftBodyProp::CSoftBodyProp(CWnd* pParent /*=NULL*/)
//: CDialog(CSoftBodyProp::IDD, pParent)
//{
//}
//
//CSoftBodyProp::~CSoftBodyProp()
//{
//}
//
//void CSoftBodyProp::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}
//
//
//BEGIN_MESSAGE_MAP(CSoftBodyProp, CDialog)
//END_MESSAGE_MAP()
//
//
//// CSoftBodyProp message handlers
//
//BOOL CSoftBodyProp::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	UINT nSkeletal = 0;
//	m_SaveParam.m_uiSP_NodeNum = NodeEnum( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, NULL, nSkeletal );
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// EXCEPTION: OCX Property Pages should return FALSE
//}
//
//void CSoftBodyProp::OnOK()
//{
//	UpdateData();
//
//	CDialog::OnOK();
//}
//
//INT_PTR CSoftBodyProp::DoModal( CString FileName, Interface* pInterface, BOOL Select )
//{
//	// TODO: Add your specialized code here and/or call the base class
//	if(!pInterface||!FileName.GetLength())
//		return 0;
//	FileName.MakeLower();
//
//	for( int i = 0; i<(int)m_NodeType.size(); i++ )
//		delete m_NodeType[i];
//	m_NodeType.clear();
//	m_pInterface    = pInterface;
//	m_Select        = Select;
//
//	UINT nSkeletal = 0;
//	m_SaveParam.m_uiSP_NodeNum = NodeEnum( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, NULL, nSkeletal );
//
//	if( /*CDialog::DoModal() ==*/ IDOK )
//	{
//		map< INode*, PIECETYPE > NodeType;
//		for( int i = 0; i<(int)m_NodeType.size(); i++ )
//			NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );
//
//		m_SaveParam.m_uiSP_pInterface	   = pInterface;
//		m_SaveParam.m_uiSP_Select		   = Select;
//		m_SaveParam.m_uiSP_WeightPerVertex = 2;
//		m_SaveParam.m_uiSP_FramePerSample  = 1;
//		m_SaveParam.m_uiSP_AnimationName   = "";
//		m_SaveParam.m_uiSP_pNodeType       = &NodeType;
//		m_SaveParam.m_eSP_SaveType         = eST_SoftBody;
//		m_SaveParam.m_bSP_SmoothAll		   = true;
//
//		if(SUCCEEDED(m_Model.FillFrom3DMax( m_SaveParam ) ) )
//		{
//			if(SUCCEEDED(m_Model.Save((LPCTSTR)FileName)))
//				return IMPEXP_SUCCESS;
//		}
//	}
//	else
//		return IMPEXP_CANCEL;
//
//	return 0;
//}

void CPiecePropDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}

void CSkeletalProp::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}

void CAniGroupProp::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}




// CPieceCompressPropDlg dialog

IMPLEMENT_DYNAMIC(CPieceCompressPropDlg, CDialog)

CPieceCompressPropDlg::CPieceCompressPropDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPieceCompressPropDlg::IDD, pParent)
{
	m_Model = NULL;

}

CPieceCompressPropDlg::~CPieceCompressPropDlg()
{
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();
}

void CPieceCompressPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLIST, m_ExpCpSelect);
}


BEGIN_MESSAGE_MAP(CPieceCompressPropDlg, CDialog)
END_MESSAGE_MAP()

BOOL CPieceCompressPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UINT nSkeletal = 0;
	m_ExpCpSelect.InsertColumn( 0, "物件名字", LVCFMT_LEFT, 80 );
	m_ExpCpSelect.InsertColumn( 1, "压缩", LVCFMT_LEFT, 60 );
	m_ExpCpSelect.InsertColumn( 2, "不压缩", LVCFMT_LEFT, 60 );
	NODECPTYPE NodeType;
	CPieceClass::iterator iter =  m_Model->GetCPieceGroup().m_Piece[0].begin();
	while(iter != m_Model->GetCPieceGroup().m_Piece[0].end())
	{
		if(((CPluginPiece*)(*iter))->GetMash()->GetIsCompress())
			NodeType = enumNT_COMPRESS;
		else
			NodeType = enumNT_UNCOMPRESS;
		ListNodeCp* Node = new ListNodeCp( NodeType, ((CPluginPiece*)(*iter))->GetMash() );
		m_NodeType.push_back( Node );
		m_ExpCpSelect.InsertItem(LVIF_TEXT|LVIF_PARAM, m_ExpCpSelect.GetItemCount(), ((CPluginPiece*)(*iter))->GetPieceName().c_str(), 0, 0, 0, (LPARAM)Node);
		iter++;
	}

	//m_SaveParam.m_uiSP_NodeNum = NodeEnumCp( m_pInterface->GetRootNode(), m_pInterface, m_Select, m_NodeType, this, nSkeletal );

	return TRUE;  // return TRUE unless you set the focus to a control
}
INT_PTR CPieceCompressPropDlg::DoModal( ::CPluginModel* tModel)
{
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_Model = tModel;

	if( CDialog::DoModal() == IDOK )//确定的话就同步到新的压缩状态,否则恢复到老的压缩状态
	{
		CPieceClass::iterator iter =  m_Model->GetCPieceGroup().m_Piece[0].begin();
		while(iter != m_Model->GetCPieceGroup().m_Piece[0].end())
		{
			((CPluginPiece*)(*iter))->GetMash()->asynNewCompressState();
			iter++;
		}
		return IMPEXP_SUCCESS;
	}
	else
	{
		CPieceClass::iterator iter =  m_Model->GetCPieceGroup().m_Piece[0].begin();
		while(iter != m_Model->GetCPieceGroup().m_Piece[0].end())
		{
			((CPluginPiece*)(*iter))->GetMash()->asynOldCompressStete();
			iter++;
		}
		return IMPEXP_CANCEL;
	}
}
// CPieceCompressPropDlg message handlers
