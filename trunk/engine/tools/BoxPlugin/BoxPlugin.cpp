 /**********************************************************************
 *<
	FILE: BoxPlugin.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: librayhl

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "stdafx.h"
#include "BoxPlugin.h"
#include "Model.h"

#define BoxExp_CLASS_ID	Class_ID(0x75f8c50f, 0x5adb4507)

#define BOXSZ 20.0f

CModel		g_Model;



class BoxPlugin : public UtilityObj {
	public:
		
		HWND			hPanel;
		IUtil			*iu;
		Interface		*ip;

		SAVEPARAM               m_SaveParam;
		vector< ListNode* >		m_NodeType;

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);

		void FillModel(bool isFix);
		void ClearModel();

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
		
		void DeleteThis() { }		
		//Constructor/Destructor

		BoxPlugin();
		~BoxPlugin();		

};

static BoxPlugin theBoxExp;


class BoxExpClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theBoxExp; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return BoxExp_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("BoxPlugin"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
};

static BoxExpClassDesc BoxExpDesc;
ClassDesc2* GetBoxExpDesc() { return &BoxExpDesc; }

static INT_PTR CALLBACK BoxExpDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theBoxExp.Init(hWnd);
			break;

		case WM_DESTROY:
			theBoxExp.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_RESET:
				g_Model.Update();
				MessageBox(NULL,"完成","更新",MB_OK);
				break;
			case IDC_GENBOX:
				theBoxExp.ClearModel();
				theBoxExp.FillModel(TRUE);
				g_Model.BuildAABBBox();
				g_Model.ResumePose();
				//g_Model.SetBigBox();
				g_Model.SetAABBBox();

				// Create a dummy object & node
				
				// make a box object
				//GenBoxObject *ob = (GenBoxObject *)createInterface->CreateInstance(GEOMOBJECT_CLASS_ID,Class_ID(BOXOBJ_CLASS_ID,0));
				//ob->SetParams(BOXSZ,BOXSZ,BOXSZ,1,1,1,FALSE); 
				break;
			case IDC_DELBOX:
				g_Model.DelAllBox();
				break;
			case IDC_SELNODE:
				g_Model.UpdateBox();
				g_Model.SelAllNode();
				break;
			case IDC_ADDBOX:
				g_Model.GetSelectNodes();
				g_Model.ClearBoxMap();
				g_Model.AddBoxs();
				break;
			case IDC_OTHERBOX:
				theBoxExp.ClearModel();
				theBoxExp.FillModel(FALSE);
				g_Model.BuildAllBox();
				//g_Model.SetBigBox();
				g_Model.SetAABBBox();
				break;
			case IDC_FILLDATA:
				theBoxExp.ClearModel();
				g_Model.Update();
				theBoxExp.FillModel(FALSE);
				MessageBox(NULL,"填充数据完成","数据",MB_OK);
				break;
			}
			break;


		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theBoxExp.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return 0;
	}
	return 1;
}



//--- BoxPlugin -------------------------------------------------------
BoxPlugin::BoxPlugin()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

void BoxPlugin::FillModel(bool isFix)
{
	//填充数据
	UINT nSkeletal = 0;
	m_SaveParam.m_uiSP_NodeNum = g_Model.NodeEnum(ip->GetRootNode(),ip,FALSE,m_NodeType,nSkeletal);

	map< INode*, PIECETYPE > NodeType;
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		NodeType.insert( pair< INode*, PIECETYPE >( m_NodeType[i]->m_MaxNode, m_NodeType[i]->m_ExpType ) );

	m_SaveParam.m_uiSP_pInterface		= ip;
	m_SaveParam.m_uiSP_Select			= FALSE;
	m_SaveParam.m_uiSP_WeightPerVertex	= 2;
	m_SaveParam.m_uiSP_FramePerSample	= 1;
	m_SaveParam.m_uiSP_AnimationName	= "NotUsed";
	m_SaveParam.m_uiSP_pNodeType		= &NodeType;
	m_SaveParam.m_eSP_SaveType			= eST_Piece;
	m_SaveParam.m_bSP_SmoothAll			= false;

	//g_Model.BuildBigBox();

	if(isFix)
		g_Model.FixPose();
	g_Model.FillFrom3DMax(m_SaveParam);
}

void BoxPlugin::ClearModel()
{
	//清空数据
	for( int i = 0; i<(int)m_NodeType.size(); i++ )
		delete m_NodeType[i];
	m_NodeType.clear();

	g_Model.Free();
	m_SaveParam.Free();
}

BoxPlugin::~BoxPlugin()
{
}

void BoxPlugin::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;

	g_Model.Init(ip);

	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		BoxExpDlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void BoxPlugin::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void BoxPlugin::Init(HWND hWnd)
{
}

void BoxPlugin::Destroy(HWND hWnd)
{
}