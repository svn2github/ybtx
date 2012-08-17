
//--------------------------------------------------------------------------------
// 文件：ExpPlugin.cpp
// 日期：2004.3.22
// 功能：从3DMAX场景获得模型数据，并按要求输出
//------------------------------------------------------------------------------------
// 日期：2007.1.19
// 功能：增加 披风 的导出 :)
//------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "model.h"
// 临时修改，解决编译不过问题
//#include "ParaAdapterCtrl\CModRsTexPathCfgMgr.h"
#include "stdmat.h"
#include <algorithm>
#include <fstream>
#include <set>
#include <map>
#include "Phyexp.h"
#include "Bipexp.h"
#include "decomp.h"
#include "ISkin.h"
#include "iiksys.h"
#include <iostream>
#include <string>
#include <vector>
#include "BoneFix.h"
#include "SoftClothDlg.h"
#include "PropSelectDlg.h"
#include "cfourcc.h"
#include <Windows.h>
#include "GraphicBase.h"
#include "CEditPieceGroup.h"
#include "CPluginMeshBase.h"
#include "CPluginStaticMesh.h"
#include "CPluginVertexAniMesh.h"
#include "CPluginSkeletalMesh.h"
#include "CModCoder.h"

#define   MINIMUMWEIGHT    0.0001f
//#define	  SingleUV
#define	  DoubleUV
#define GETVERSION(VER)	0x0000ffff&VER
#define GETCP(VER)  (0x00010000&VER)>>16
#define GETBOX(VER)	 (0x00100000&VER)>>20
#define ENABLECP(VER)		0x00010000|VER	
#define ENABLEBOX(VER)		0x00100000|VER
extern int  FaceIndex[2][3];
SAVEPARAM			g_SaveParam;
float				g_fCurNode;
bool				g_bIsExistModFile;
extern BOOL			g_IsCpSke;
vector<BoxNode>		g_vecNode;

std::map< string, string >	BoneFixMap;
bool					    HaveBoneFix;

extern const CMatrix GetNodeGlobeMatrix( INode* MaxNode, int Time, BOOL Scale );
extern const CMatrix GetNodeLocalMatrix( INode* MaxNode, int Time, BOOL Scale );

/// 头像信息
//struct PortraitInfo
//{
//	PortraitInfo()
//		: fourcc('PTIF'), translate(0.0f, 0.0f, 0.0f), cameraPos(0,50,-200)
//		, fov(D3DXToRadian(30)) {}
//	FourCC		fourcc;
//	CVector3f translate;
//	CVector3f cameraPos;
//	float		fov;
//};
PortraitInfo g_portraitInfo;
//////////////////////////////////////////////////////////////////////////
//用于记录路径的全局名
string g_ARPPath = "";
string g_TexPath = "";
string g_RSName = "";
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////
// 每个CSoftSim实例都有一组和弹簧，结点相关的数据
// 一个CSoftSim实例的每个结点的物理参数是一样的
// 弹簧数据有三种，而除SoftCloth以外的其他类型的
//     柔体只会用到一种弹簧类型，即StretchSpring
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// class CBallNode
/////////////////////////////////////////////////

//enum EVerType{VerPos, VerColor3D, VerNorTex, VerNor, VerNorDif}; //预定义顶点类型
#define  VerFormat   VerNorTex //布匹的顶点格式,有位置，法线和纹理坐标



////////////////////////////////////////////////////////////////////////
// 功能：获取关键帧
// 参数：Animatable* animatable 动画接口, 
//       list<int>& KeyFrame    关键帧列表, 
//       Interval& range        动画场景时间范围
// 返回：无
////////////////////////////////////////////////////////////////////////// 
void InsertAnimatableKeyFrames( Animatable* animatable, list<int>& KeyFrame, Interval& range )
{
	int numKeyFrames = animatable->NumKeys();
	if( numKeyFrames == NOT_KEYFRAMEABLE )
		return;

	TimeValue curTime;

	for( int i = 0; i < numKeyFrames; i++ )
	{
		curTime = animatable->GetKeyTime( i );
		if( curTime >= range.Start() && curTime <= range.End() )
			KeyFrame.push_back( curTime/GetTicksPerFrame() );
	}
}


////////////////////////////////////////////////////////////////////////
// 功能：获取关键帧
// 参数：Control* controller	动画接口, 
//       list<int>& KeyFrame    关键帧列表, 
//       Interval& range        动画场景时间范围
// 返回：无
////////////////////////////////////////////////////////////////////////// 
void InsertControlKeyFrames( Control* pController, list<int>& KeyFrame, Interval& range )
{
	InsertAnimatableKeyFrames( pController, KeyFrame, range );

	if( pController->GetPositionController() )
		InsertAnimatableKeyFrames( pController->GetPositionController(), KeyFrame, range );
	if( pController->GetRotationController() )
		InsertAnimatableKeyFrames( pController->GetRotationController(), KeyFrame, range );
}

////////////////////////////////////////////////////////////////////////
// 功能：插入次级动画帧
// 参数：Control* controller    动画关键帧控制接口, 
//       list<int>& KeyFrame    关键帧列表, 
//       Interval& range        动画场景时间范围
// 返回：无
////////////////////////////////////////////////////////////////////////// 
void InsertSubKeyFrames( Control* controller, list<int>& KeyFrame, Interval& range )
{
	for( int i = 0; i < controller->NumSubs(); i++ )
	{
		Animatable* subAnim = controller->SubAnim( i );
		if( !subAnim )
			continue;
		InsertAnimatableKeyFrames( subAnim, KeyFrame, range );
	}
}

////////////////////////////////////////////////////////////////////////
// 功能：获取IK关键帧
// 参数：Control* pControl		控制接口, 
//       list<int>& KeyFrame    关键帧列表, 
//       Interval& range        动画场景时间范围
// 返回：无
////////////////////////////////////////////////////////////////////////// 
void InsertIKKeyFrames( Control* pControl, list<int>& KeyFrame, Interval& range )
{
	TSTR ClassName;
	pControl->GetClassName( ClassName );
	//printf( "Controler:%s\n", ClassName );
	IIKControl* pIK = (IIKControl*)pControl->GetInterface( I_IKCONTROL );
	if( pIK ||
		( pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ) ||
		( pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID ) ||
		( pControl->ClassID() == FOOTPRINT_CLASS_ID ) )
	{
		//INodeTab Starts = pIK->IKChains( IKSys::RotationalJoint );
		//INodeTab Ends	= pIK->IKChains( IKSys::SlidingJoint );

		//list<int> IKKeyFrame;
		//Control* controller;
		//for( int i = 0; i < Starts.Count(); i++ )
		//{
		//	Control* pIKControl = Starts[i]->GetTMController();
		//	InsertControlKeyFrames( pIKControl, IKKeyFrame, range );
		//	InsertSubKeyFrames( pIKControl, IKKeyFrame, range );
		//}

		//for( int i = 0; i < Ends.Count(); i++ )
		//{
		//	Control* pIKControl = Ends[i]->GetTMController();
		//	InsertControlKeyFrames( pIKControl, IKKeyFrame, range );
		//	InsertSubKeyFrames( pIKControl, IKKeyFrame, range );
		//}

		//IKKeyFrame.sort();
		//int nStart = max( range.Start(), *IKKeyFrame.begin() );
		//int nEnd   = (int)min( DWORD( range.End() ), DWORD( *IKKeyFrame.rbegin() ) );
		for( int i = range.Start(); i < range.End(); i += GetTicksPerFrame() )
			KeyFrame.push_back( i/GetTicksPerFrame() );
		KeyFrame.push_back( range.End()/GetTicksPerFrame() );
	}
}

string tranString(string& str)
{
	int pos = str.rfind("/");
	if(pos!=string::npos)
		str = str.substr(pos+1,str.size());

	if(str.find("rlm")!=string::npos)
		str = str.substr(0,3);
	if(str.find("rlw")!=string::npos)
		str = str.substr(0,3);
	if(str.find("srm")!=string::npos)
		str = str.substr(0,3);
	str += ".ske";
	return str;
}
////////////////////////////////////////////////////////////////
// 功能：VerSkinNCT 比较函数
// 参数：略
// 返回：无
////////////////////////////////////////////////////////////////

bool operator > ( const VerSkinNCT& v1, const VerSkinNCT& v2 )
{
	return memcmp( &v1, &v2, sizeof(VerSkinNCT) ) > 0;
}

bool operator < ( const VerSkinNCT& v1, const VerSkinNCT& v2 )
{
	return memcmp( &v1, &v2, sizeof(VerSkinNCT) ) < 0;
}



inline void D3DXQuaternionVectorMultiply(CVector3f& Out,const D3DXQUATERNION& Quat,const CVector3f& In)
{
	CVector3f uv, uuv;
	CVector3f qvec(Quat.x, Quat.y, Quat.z);
	uv = qvec.Cross(In);
	uuv = qvec.Cross(uv);
	//D3DXVec3Cross(&uv,&qvec,&In); 
	//D3DXVec3Cross(&uuv,&qvec,&uv);
	D3DXVec3Scale((D3DXVECTOR3*)&uv,(D3DXVECTOR3*)&uv,2.0f*Quat.w);
	D3DXVec3Scale((D3DXVECTOR3*)&uuv,(D3DXVECTOR3*)&uuv,2.0f);
	D3DXVec3Add((D3DXVECTOR3*)&Out,(D3DXVECTOR3*)&In,(D3DXVECTOR3*)&uv); 
	D3DXVec3Add((D3DXVECTOR3*)&Out,(D3DXVECTOR3*)&Out,(D3DXVECTOR3*)&uuv);
}

string ProcessTextureName( const string& szTexName )
{
	size_t Size = szTexName.size();
	string sTex = szTexName.c_str();
	sTex = sTex.substr(0, sTex.size()-4) + ".dds";
	string st;
	// 临时修改，解决编译不过问题
	//st = CModRsTexPathCfgMgr::Inst()->GetParaTex();
	//st += sTex;
	//CModRsTexPathCfgMgr::Inst()->AddPathLog(st);

	return st;
}

string ProcesePieceRSName( const string& szRSName )
{
	size_t Size = szRSName.size();
	string sTex = szRSName.c_str();
	sTex = sTex.substr(0, sTex.size()-4) + ".dds";
	string st;
	// 临时修改，解决编译不过问题
	//st = CModRsTexPathCfgMgr::Inst()->GetParaRs();
	//st += sTex;
	//CModRsTexPathCfgMgr::Inst()->AddPathLog(st);

	return st;
}

////////////////////////////////////////////////////////////////////////
// 功能：构造和析构
// 参数：略
// 返回：无
////////////////////////////////////////////////////////////////////////// 
CPluginModel::CPluginModel(void)
{
	m_AniName = "";
	m_Root   = NULL;
}

CPluginModel::~CPluginModel(void)
{
	Free();
}

void CPluginModel::Free()
{

	delete m_Root;
	m_Root   = NULL;
}
void CPluginModel::SetGlobeIsExist(bool IsExist)
{
	g_bIsExistModFile = IsExist;
}

void CPluginModel::ClearModData()
{
	if(!m_PieceGroupOld.m_Piece.empty())
		m_PieceGroupOld.Free();
}
CPluginPieceGroup& CPluginModel::GetCPieceGroup()
{
	return m_PieceGroup;
}

void CPluginModel::SetExpFrame(BOOL IsExpFrame)
{
	m_IsExpAllFrame = IsExpFrame;
}

void CPluginModel::SetIndxSkeName(string& name)
{
	string tmp = name;
	int pos = tmp.rfind("\\");
	if(pos!=string::npos)
		tmp = tmp.substr(pos+1,tmp.size()-pos);

	m_Motions.SetIndxSkeName(tmp);
}
////////////////////////////////////////////////////////////////////////
// 功能：根据选择填充结构
// 参数：const SAVEPARAM& SaveParam    存储选项和参数,
// 返回：>0成功,否则失败
////////////////////////////////////////////////////////////////////////// 
int CPluginModel::FillFrom3DMax( const SAVEPARAM& SaveParam )
{
	Free();
	g_fCurNode		= 0;
	g_SaveParam.Free();
	g_SaveParam     = SaveParam;
	m_Root          = NodeEnum( g_SaveParam.m_uiSP_pInterface->GetRootNode(), NULL );  //枚举节点
	CBoneFix().DoModal();


	FixParent( m_Root );

	//填充动画信息
	if( m_Root )
	{
		m_Motions.SetExpFrame(m_IsExpAllFrame);
		m_Motions.FillFromExportNode( m_Root ); 
	}
	//填充顶点信息
	m_PieceGroup.GetMeshFromExpNode( m_Motions );

	return 0;
}
BOOL CPluginModel::SkeMatching()
{
	string ErrSkeletal;

	if(m_AnimateGroup.IsFitSkeForMe( m_Motions, ErrSkeletal ))
		return TRUE;
	else
	{
		m_AnimateGroup.Free();
		return FALSE;
	}
}
BOOL CPluginModel::CheckTrouser()
{
	if( !m_Motions.TransToProAni() )
		return FALSE;
	else
		return TRUE;
}
bool CPluginModel::IsFit(CString FileName)
{
	string ErrSkeletal;
	OutputDebugString("待检测文件开始");
	m_Motions.TransToProAni();
	OutputDebugString("待检测文件结束");
	if(/*m_AnimateGroup.GetIndxSkeName()==m_Motions.GetIndxSkeName() && */m_AnimateGroup.IsFitForMe( m_Motions, ErrSkeletal ))
	{
		//vector<CAnimate*>& NewList = m_Motions.GetAnimateList();
		//vector<CAnimate*>& OrgList = m_AnimateGroup.GetAnimateList();
		//for( int i = 0; i < (int)NewList.size(); i++ )
		//{
		//	IDNAME NewName = NewList[i]->GetName();
		//	int j = 0;
		//	for( ; j < (int)OrgList.size(); j++ )
		//	{
		//		IDNAME OrgName = OrgList[j]->GetName();
		//		if( OrgName == NewName )
		//			break;
		//	}
		//	if( j == (int)OrgList.size() )
		//	{
		//		OrgList.push_back( NewList[i] );
		//	}
		//}
		//OutputDebugString("合并完成");
		//NewList.clear();
		return TRUE;
	}
	else
	{
		FileName += "文件的骨架结构和MAX中的骨架结构不符, 请检查骨骼:";
		if(!ErrSkeletal.empty())
			FileName += ErrSkeletal.c_str();
		else
		{
			FileName += m_AnimateGroup.GetIndxSkeName().c_str();
			FileName += "<=======>";
			FileName += m_Motions.GetIndxSkeName().c_str();
		}

		m_Motions.Free();
		m_AnimateGroup.Free();
		Free();
		::MessageBox( 0, FileName, "骨架不匹配错误,请点击Schematic View查看骨架!", 0 );
		return FALSE;
	}
}
int	CPluginModel::FillFromFile (string FileName ,string& aniname)
{
	//m_Root->Free();
	string skefile = FileName.substr(0,FileName.size()-4)+".ske";
	m_AnimateGroup.BinLoadSke( skefile );
	m_AnimateGroup.SetAniName(aniname);
	m_AnimateGroup.BinLoad( FileName );
	return 1;
}

int CPluginModel::FillAgpFromFile(string FileName)
{
	m_AnimateGroup.BinLoadAgp( FileName );
	return 1;
}

bool CPluginModel::LoadSkeFromFile(string FileName)
{
	if(m_AnimateGroup.BinLoadSke( FileName )!=-1)
		return true;
	else
		return false;
}

string& CPluginModel::GetIndxNameFromAgp()
{
	return m_AnimateGroup.GetIndxSkeName();
}

void CPluginModel::GetOldModInfoFromFile(string FileName)
{
	// zjtemp
#if 0
	CModFormat modFmt;
	modFmt.Load(FileName.c_str(), &m_PieceGroupOld);
#else
	m_PieceGroupOld.PluginBinLoad(FileName);
#endif
}
void CPluginModel::FixParent( ExportNode* Root )
{
	std::map < std::string, ExportNode * > String2ExportNode;
	std::list< ExportNode * > BLStack;

	BLStack.push_back( Root );
	while( !BLStack.empty() )
	{
		ExportNode * p = BLStack.front();
		BLStack.pop_front();

		String2ExportNode[ p->m_3DMaxNode->GetName() ] = p;

		for( int i = 0; i < p->m_Children.size(); ++i)
		{
			BLStack.push_back( p->m_Children[i] );
		}
	}

	BLStack.push_back( Root );
	while( !BLStack.empty() )
	{
		ExportNode * p = BLStack.front();
		BLStack.pop_front();

		INode * MaxNode = p->m_3DMaxNode;
		CStr t;
		string pname;
		bool b = false;
		MaxNode->GetUserPropBuffer( t );

		string & SName = string(MaxNode->GetName());
		if( BoneFixMap[SName] != "" )
			pname = BoneFixMap[SName];
		else
		{
			for(int i=0;i<t.length();++i)
			{
				if( t[i] == '[' ) 
				{
					b = true;
				} else if( t[i] == ']' )
				{
					b = false;				
				} else if( b == true )
				{
					pname = pname + t[i];
				}
			}
		}

		if ( !pname.empty() )
		{
			vector< ExportNode*>::iterator iter = find( p->m_Parent->m_Children.begin(), p->m_Parent->m_Children.end(), p);
			p->m_Parent->m_Children.erase( iter );

			String2ExportNode[pname]->m_Children.push_back( p );
			p->m_Parent = String2ExportNode[pname];
		}

		for( int i = 0; i < p->m_Children.size(); ++i)
		{
			BLStack.push_back( p->m_Children[i] );
		}
	}

}

//////////////////////////////////////////////////////////
// 功能：遍历所有节点并且填充到相应的结构里面
// 参数：INode* MaxNode     当前3DMAX节点, 
//       ExportNode* Parent 上一级输出节点
// 返回：新创建的当前节点
//////////////////////////////////////////////////////////
ExportNode* CPluginModel::NodeEnum( INode* MaxNode, ExportNode* Parent )
{	
	g_SaveParam.m_uiSP_pInterface->ProgressUpdate( int( g_fCurNode += 0.4f )*100/g_SaveParam.m_uiSP_NodeNum );
	if( !MaxNode )
		return NULL;

	map< INode*, PIECETYPE >* NodeType = ( map< INode*, PIECETYPE >* )g_SaveParam.m_uiSP_pNodeType;

	ExportNode* ExpNode = new ExportNode();

	if( !ExpNode )
		return NULL;

	//填充当前节点

	ExpNode->m_3DMaxNode = MaxNode;
	ExpNode->m_Parent    = Parent;

	if(NodeType->find( MaxNode )!=NodeType->end())
	{
		ExpNode->m_PieceType = NodeType->find( MaxNode )->second;

		//将有效的网格归类
		if( enumPT_VERTEXANI   == ExpNode->m_PieceType ||
			enumPT_SKELETALANI == ExpNode->m_PieceType ||
			enumPT_NONEANI     == ExpNode->m_PieceType )
		{
			if( MaxNode->GetUserPropBool( "Cloth", ExpNode->m_bSoftLinkBone ) == FALSE )
				ExpNode->m_bSoftLinkBone = FALSE;
			if( MaxNode->GetUserPropBool( "SoftCloth", ExpNode->m_bSoftCloth ) == FALSE )
				ExpNode->m_bSoftCloth = FALSE;
			if( MaxNode->GetUserPropInt( "CapsuleIndex", ExpNode->m_nCapsules ) == FALSE )
				ExpNode->m_nCapsules = -1;
			if( MaxNode->GetUserPropInt( "CapsuleSide", ExpNode->m_nCapsuleSide ) == FALSE )
				ExpNode->m_nCapsuleSide = 0;        

			float fAddBoneWeight = 0.0f;
			if( MaxNode->GetUserPropFloat( "AddOneWhenWeight", fAddBoneWeight ) )
			{
				ExpNode->m_AddBoneWeight[fAddBoneWeight] = 1;
				//printf( "AddOneWhenWeight = %f", fAddBoneWeight );
			}
			if( MaxNode->GetUserPropFloat( "AddTwoWhenWeight", fAddBoneWeight ) )
			{
				ExpNode->m_AddBoneWeight[fAddBoneWeight] = 2;
				//printf( "AddTwoWhenWeight = %f", fAddBoneWeight );
			}

			CPluginPiece* pPiece = new CPluginPiece(&m_PieceGroup);
			CPluginRenderStyle* pRenderStyle = new CPluginRenderStyle;
			// 临时修改，解决编译不过问题
			//string szPieceRSName = CModRsTexPathCfgMgr::Inst()->GetParaRs() + "mipmap1.marl";
			//CModRsTexPathCfgMgr::Inst()->AddPathLog(szPieceRSName.c_str());
			//pRenderStyle->m_RenderStyleName = szPieceRSName.c_str();

			CPluginMesh* Mesh = 0;
			switch ( ExpNode->m_PieceType )
			{
			case enumPT_VERTEXANI:
				Mesh = new CPluginVertexAniMesh;
				pPiece->SetMesh((CMesh*)(CPluginVertexAniMesh*)Mesh);
				break;

			case enumPT_SKELETALANI:
				Mesh = new CPluginSkeletalMesh;
				pPiece->SetMesh((CMesh*)(CPluginSkeletalMesh*)Mesh);
				break;

			case enumPT_NONEANI:
				Mesh = new CPluginStaticMesh;
				pPiece->SetMesh((CMesh*)(CPluginStaticMesh*)Mesh);
				break;
			}

			pPiece->SetPluginMesh( Mesh );
			pPiece->GetPieceName() = MaxNode->GetName();
			Mesh->SetExpNode( ExpNode );

			if( !Mesh )
				return NULL;


			for(int i=0;i<g_vecNode.size();i++)
			{
				CBaseAABB aabb;
				if ( g_vecNode[i].m_MeshName == pPiece->GetPieceName().c_str())
				{
					aabb.m_vMaximum = g_vecNode[i].m_vMaximum;
					aabb.m_vMinimum = g_vecNode[i].m_vMinimum;
					Mesh->m_BoxMap.insert(CPluginMesh::BoxMap::value_type(uint16(g_vecNode[i].m_Id),aabb));
				}
			}

			Mtl* Material = MaxNode->GetMtl();
			TSTR className;

			if( Material )
			{
				if (Material->NumSubMtls() > 0)  
				{
					for (int i=1; i<Material->NumSubMtls(); i++) 
					{
						Mtl* subMtl = Material->GetSubMtl(i);
						if (subMtl) {
							className = subMtl->GetFullName();
							Texmap* tex = subMtl->GetSubTexmap( ID_DI );
							if( tex )
							{
								if( tex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
								{
									TCHAR* pChar = ((BitmapTex*)tex)->GetMapName();

									for( int i = (int)strlen( pChar ); i >= 0 ; i-- )
									{
										if( pChar[i] == '\\' || pChar[i] == '/' )
											break;
									}
									string tmpTexName = pChar + i + 1;
									tmpTexName = ProcessTextureName(tmpTexName);
									pRenderStyle->m_TextureName.push_back(tmpTexName.c_str());
								}
							}
						}
					}
					if ( g_bIsExistModFile )
					{
						CPluginPiece* pOldPiece = m_PieceGroupOld.GetPieceByName(pPiece->GetPieceName().c_str());
						if ( pOldPiece != 0 && ! pOldPiece->m_RenderStyle.empty() )
						{
							*pRenderStyle = *(CPluginRenderStyle*)(pOldPiece->m_RenderStyle[0]);
						}
					}
					pPiece->AddRenderStyle( pRenderStyle );
				}
				else if(Material->ClassID() == Class_ID( DMTL_CLASS_ID,0 ))
				{
					// 得到diffuse贴图
					Texmap* tex = Material->GetSubTexmap( ID_DI );
					if( tex )
					{
						if( tex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
						{
							TCHAR* pChar = ((BitmapTex*)tex)->GetMapName();


							for( int i = (int)strlen( pChar ); i >= 0 ; i-- )
							{
								if( pChar[i] == '\\' || pChar[i] == '/' )
									break;
							}
							string tmpTexName = pChar + i + 1;
							tmpTexName = ProcessTextureName(tmpTexName);
							pRenderStyle->m_TextureName.push_back(tmpTexName.c_str());
						}
					}	
					if ( g_bIsExistModFile )
					{
						CPluginPiece* pOldPiece = m_PieceGroupOld.GetPieceByName(pPiece->GetPieceName().c_str());
						if ( pOldPiece != 0 && ! pOldPiece->m_RenderStyle.empty() )
						{
							*pRenderStyle = *(CPluginRenderStyle*)(pOldPiece->m_RenderStyle[0]);
						}
					}
					pPiece->AddRenderStyle( pRenderStyle );
				}
				else
				{
					TCHAR Buf[256];
					sprintf_s( Buf, "Material->ClassID() = %x(), !=Class_ID( DMTL_CLASS_ID,0 )", Material->ClassID() );
					MessageBox( 0, Buf, "贴图ID错误", 0 );
				}
			}
			m_PieceGroup.AddPiece( pPiece );
		}
	}

	//继续考察子节点
	for ( int i=0; i<MaxNode->NumberOfChildren(); i++ )
	{
		ExportNode* childNode = NodeEnum( MaxNode->GetChildNode(i), ExpNode );
		if ( !childNode )
			return NULL;

		ExpNode->m_Children.push_back( childNode );
	}

	return ExpNode;
	//}
	//else
	//	return NULL;

}

////////////////////////////////////////////////////////////////////////
// 功能：按类结构保存到文件
// 参数：略
// 返回：略
////////////////////////////////////////////////////////////////////////// 
int CPluginModel::SaveFrom3DMax(string FileName)
{
	ofstream File( FileName.data(), ios::binary|ios::out );

	size_t nPos = FileName.find("model");
	g_ARPPath = FileName.substr(0, nPos);
	g_ARPPath += "path\\";

	size_t nPos0 = FileName.rfind("\\");
	size_t nPos1 = FileName.find('.');

	g_ARPPath += FileName.substr(nPos0 + 1, nPos1 - nPos0 - 1);

	g_ARPPath += ".path";

	vector<IDNAME>& IdNameList = m_AnimateGroup.GetIdNameList();
	m_Motions.BinSaveFromMax(File,FileName,IdNameList);
	m_Motions.Free();
	File.close();
	return 0;
}

int CPluginModel::SaveFromFile(string FileName)
{
	ofstream File( FileName.data(), ios::binary|ios::out );

	size_t nPos = FileName.find("model");
	g_ARPPath = FileName.substr(0, nPos);
	g_ARPPath += "path\\";

	size_t nPos0 = FileName.rfind("\\");
	size_t nPos1 = FileName.find('.');

	g_ARPPath += FileName.substr(nPos0 + 1, nPos1 - nPos0 - 1);

	g_ARPPath += ".path";

	replace(FileName.begin(), FileName.end(), '\\', '/');
	m_AnimateGroup.BinSaveFromFile(File,FileName);
	m_AnimateGroup.Free();
	File.close();
	return 0;
}


void CPluginModel::SynAniname()
{
	vector<IDNAME>&		AgpList = m_AnimateGroup.GetIdNameList();
	vector<CPluginAnimate*>&	NewList = m_Motions.GetAnimateList();
	vector<IDNAME>&		MaxList = m_Motions.GetIdNameList();
	if(!NewList.empty())	
	{
		IDNAME name = NewList[0]->GetName();
		bool isSame = false;
		for(int i =0;i<AgpList.size();i++)
		{
			if(name==AgpList[i])
				isSame = true;
			MaxList.push_back(AgpList[i]);
		}
		if(!isSame)
			MaxList.push_back(name);
	}
}

int CPluginModel::SaveAgp(string FileName)
{
	m_Motions.BinSaveAgp(FileName);
	return 0;
}

int CPluginModel::Save(string FileName)
{
	ofstream File( FileName.data(), ios::binary|ios::out );

	size_t nPos = FileName.find("model");
	g_ARPPath = FileName.substr(0, nPos);
	g_ARPPath += "path\\";

	size_t nPos0 = FileName.rfind("\\");
	size_t nPos1 = FileName.find('.');

	g_ARPPath += FileName.substr(nPos0 + 1, nPos1 - nPos0 - 1);

	g_ARPPath += ".path";


	if( g_SaveParam.m_eSP_SaveType == eST_Piece )
	{
		if(g_bIsExistModFile)
			m_PieceGroup.SetPieceGroup(&m_PieceGroupOld);
		m_PieceGroup.PluginBinSave( File, FileName );
	}
	else if( g_SaveParam.m_eSP_SaveType == eST_Skeletal )
	{
		replace(FileName.begin(), FileName.end(), '\\', '/');
		m_Motions.BinSave( File, FileName );
	}
	File.close();

	return 0;
}


//#define oldway
#define newway
////////////////////////////////////////////////////////////////////////
// 功能：填充动画信息
// 参数：ExportNode* ExpNode      根节点
//       CAnimate& Animate		  动画信息
// 返回：>0成功,否则失败
////////////////////////////////////////////////////////////////////////// 
int CPluginSkeletal::FillFromExportNode( ExportNode* ExpNode, CPluginAnimate& Animate, vector<CPluginSkeletalUnit*>& SkeletalList ,BOOL IsExpAllFrame)
{
	g_SaveParam.m_uiSP_pInterface->ProgressUpdate( int( g_fCurNode += 0.5f )*100/g_SaveParam.m_uiSP_NodeNum );

	m_SkeletalID   = Animate.GetSkeletalNum();
	m_SkeletalName = ExpNode->m_3DMaxNode->GetName();
	
	CKeyFrames KeyFrames;
	CKeyFrames* pKeyFrames = new CKeyFrames;
	Animate.AddSkeletalKeyFrames( pKeyFrames );
  
	CPluginSkeletalUnit* SkeletalUnit = new CPluginSkeletalUnit;
	SkeletalList.push_back( SkeletalUnit );
	SkeletalUnit->m_InverseMatrix.Identity();
	//D3DXMatrixIdentity( &SkeletalUnit->m_InverseMatrix );

	bool bIK = false;

	if( ExpNode->m_Parent )
	{
		WORD uMaxFrame = 0;
		list<int> KeyFrameList;
		//得到矩阵控制
		INode* MaxNode = ExpNode->m_3DMaxNode;
		Control* controller = MaxNode->GetTMController();

		KeyFrameList.push_back( g_SaveParam.m_uiSP_pInterface->GetAnimRange().End()/GetTicksPerFrame() );
		KeyFrameList.push_back( g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start()/GetTicksPerFrame() );
		if( controller )
		{
			if(!IsExpAllFrame)
			{
				//// 插入关键帧
				InsertControlKeyFrames( controller, KeyFrameList, g_SaveParam.m_uiSP_pInterface->GetAnimRange() );
				InsertSubKeyFrames( controller, KeyFrameList, g_SaveParam.m_uiSP_pInterface->GetAnimRange() );
				InsertIKKeyFrames( controller, KeyFrameList, g_SaveParam.m_uiSP_pInterface->GetAnimRange() );
			}
			else
			{
				// 处理所有帧信息
				Interval& range = g_SaveParam.m_uiSP_pInterface->GetAnimRange();
				for( int i = range.Start(); i < range.End(); i += GetTicksPerFrame() )
					KeyFrameList.push_back( i/GetTicksPerFrame() );
				KeyFrameList.push_back( range.End()/GetTicksPerFrame() );
			}

			//CKeyBuider( g_SaveParam.m_uiSP_pInterface, controller, KeyTimes );

			bIK = controller->GetInterface( I_IKCONTROL )||
				  controller->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||
				  controller->ClassID() == BIPBODY_CONTROL_CLASS_ID ||
				  controller->ClassID() == FOOTPRINT_CLASS_ID;
		}
		KeyFrameList.sort();
		KeyFrameList.unique();	
		//////////////////////////////////////////////////////////////////////////得到一共有多少帧数

		//printf( ExpNode->m_3DMaxNode->NodeName() );
		//printf( " Keys:%d\n", KeyFrameList.size() );

		//得到局部到全局的矩阵，并求得逆阵
#ifdef oldway
		Matrix3 nodeTM = MaxNode->GetNodeTM(0);
		//SkeletalUnit->m_InverseMatrix = Matrix3ToD3DXMatrix(Inverse(nodeTM),FALSE);
		Point3 inversePos;
		Point3 inverseScale;
		Quat inverseOriet;
		DecomposeMatrix(Inverse(nodeTM), inversePos, inverseOriet, inverseScale);
		D3DXQUATERNION tquat(inverseOriet.x,inverseOriet.z,inverseOriet.y,inverseOriet.w);
		CMatrix tmat;
		D3DXMatrixRotationQuaternion(&tmat,&tquat);
		tmat._41 = inversePos.x;
		tmat._42 = inversePos.z;
		tmat._43 = inversePos.y;
		SkeletalUnit->m_InverseMatrix = tmat;
#endif

#ifdef newway
		SkeletalUnit->m_InverseMatrix = GetNodeGlobeMatrix( MaxNode, (*KeyFrameList.begin())*GetTicksPerFrame(), FALSE );
		SkeletalUnit->m_InverseMatrix.Invert();
		//D3DXMatrixInverse( &SkeletalUnit->m_InverseMatrix, NULL, &SkeletalUnit->m_InverseMatrix );
#endif
		for( list<int>::const_iterator it = KeyFrameList.begin(); it != KeyFrameList.end(); it++ )//循环每一帧
		{
			int curTime = (*it)*GetTicksPerFrame();

			//得到到父节点的转换矩阵，并求出旋转四元数和平移矢量
			TransFrameInfo SingleTransFrame;
			QuatFrameInfo  SingleQuatFrame;
			WORD FrameNO = ( (*it) - *( KeyFrameList.begin() ) );

#ifdef oldway
			Matrix3 nodeTM = MaxNode->GetNodeTM(curTime);
			Matrix3 parentTM = MaxNode->GetParentNode()->GetNodeTM(curTime);
			Matrix3 relativeTM = nodeTM * Inverse(parentTM);

			Point3 nodePos;
			Point3 nodeScale;
			Quat nodeOriet;

			DecomposeMatrix(relativeTM, nodePos, nodeOriet, nodeScale);
			SingleQuatFrame.m_FrameQuat = D3DXQUATERNION(nodeOriet.x,nodeOriet.z,nodeOriet.y,nodeOriet.w);
			SingleTransFrame.m_FrameTrans = CVector3f( nodePos.x, nodePos.z, nodePos.y);
#endif

#ifdef newway
			CMatrix CurNodeMat = GetNodeGlobeMatrix( MaxNode, curTime, FALSE )*
				ExpNode->m_Parent->m_InvMatrix[ min( FrameNO, ExpNode->m_Parent->m_InvMatrix.size() - 1 ) ];
			D3DXQuaternionRotationMatrix( &SingleQuatFrame.m_FrameQuat, (D3DXMATRIX*)&CurNodeMat );
			SingleTransFrame.m_FrameTrans = CVector3f( CurNodeMat.m[3][0], CurNodeMat.m[3][1], CurNodeMat.m[3][2] );
#endif
			if( g_SaveParam.m_bSP_RootScale && m_SkeletalID == 1 && it == KeyFrameList.begin() )
			{
				CMatrix ScaleMat = GetNodeLocalMatrix( MaxNode, curTime, TRUE );
				CVector3f Scale( 1, 1, 1 );
				D3DXVec3TransformNormal( (D3DXVECTOR3*)&Scale, (D3DXVECTOR3*)&Scale, (D3DXMATRIX*)&ScaleMat );
				Animate.SetScale( D3DXVec3Length( (D3DXVECTOR3*)&Scale )/1.732f );
				//printf( "Scale: %f\n", D3DXVec3Length( &Scale )/1.732f );
			}
			SingleQuatFrame.m_FrameTinyQuat = CPluginTinyQuaternion( SingleQuatFrame.m_FrameQuat );
			KeyFrames.m_KeyTransFrames[FrameNO] = SingleTransFrame;
			KeyFrames.m_KeyQuatFrames[FrameNO] = SingleQuatFrame;
			uMaxFrame = max( uMaxFrame, FrameNO );
		}
		Animate.SetMaxFrame( uMaxFrame );

		if( KeyFrames.m_KeyTransFrames.size() )
		{
			//for(int i = 0;i<KeyFrames.m_KeyTransFrames.size();i++)
			//{
			//	pKeyFrames->m_KeyTransFrames[i] = KeyFrames.m_KeyTransFrames[i];
			//	Animate.SetMaxLength(D3DXVec3Length(&KeyFrames.m_KeyTransFrames[i].m_FrameTrans));
			//}

			map<WORD,TransFrameInfo>::iterator itPre = KeyFrames.m_KeyTransFrames.begin();
			map<WORD,TransFrameInfo>::iterator it = KeyFrames.m_KeyTransFrames.begin();
			pKeyFrames->m_KeyTransFrames[it->first] = it->second;
			Animate.SetMaxLength( D3DXVec3Length( (D3DXVECTOR3*)&it->second.m_FrameTrans ) );
			it++;
			for( it; it != KeyFrames.m_KeyTransFrames.end(); it++ )
			{
				WORD no = it->first;
				map<WORD,TransFrameInfo>::iterator itNext = it;
				itNext++;
				if( itNext == KeyFrames.m_KeyTransFrames.end() )
					break;

				TransFrameInfo& Pre = itPre->second;
				TransFrameInfo& Cur = it->second;
				TransFrameInfo& Nxt = itNext->second;
				float weight = float( itNext->first - it->first )/( itNext->first - itPre->first );
				CVector3f FrameTrans;
				FrameTrans = Pre.m_FrameTrans*weight + Nxt.m_FrameTrans*( 1.0f - weight );
				FrameTrans = FrameTrans - Cur.m_FrameTrans;

				//if( D3DXVec3Length( (CVector3f*)&FrameTrans ) > 0.1f )
				{
					itPre = it; 
					pKeyFrames->m_KeyTransFrames[it->first] = it->second;
					//if(m_SkeletalName == "Bip01 L Hand")
						//db("x = %f,y = %f,z = %f",it->second.m_FrameTrans.x,it->second.m_FrameTrans.y,it->second.m_FrameTrans.z);
					Animate.SetMaxLength( D3DXVec3Length( (D3DXVECTOR3*)&it->second.m_FrameTrans ) );
				}
			}
			pKeyFrames->m_KeyTransFrames[it->first] = it->second;
			Animate.SetMaxLength( D3DXVec3Length( (D3DXVECTOR3*)&it->second.m_FrameTrans ) );
			KeyFrames.m_KeyTransFrames.clear();
		}


		if( KeyFrames.m_KeyQuatFrames.size() )
		{
			//for(int i = 0;i<KeyFrames.m_KeyQuatFrames.size();i++)
			//	pKeyFrames->m_KeyQuatFrames[i] = KeyFrames.m_KeyQuatFrames[i];

			map<WORD,QuatFrameInfo>::iterator itPre = KeyFrames.m_KeyQuatFrames.begin();
			map<WORD,QuatFrameInfo>::iterator it = KeyFrames.m_KeyQuatFrames.begin();
			pKeyFrames->m_KeyQuatFrames[it->first] = it->second;
			it++;
			for( it; it != KeyFrames.m_KeyQuatFrames.end(); it++ )
			{
				map<WORD,QuatFrameInfo>::iterator itNext = it;
				itNext++;
				if( itNext == KeyFrames.m_KeyQuatFrames.end() )
					break;

				QuatFrameInfo& Pre = itPre->second;
				QuatFrameInfo& Cur = it->second;
				QuatFrameInfo& Nxt = itNext->second;
				float weight = float( itNext->first - it->first )/( itNext->first - itPre->first );
				D3DXQUATERNION FrameQuat;
				D3DXQuaternionSlerp( &FrameQuat, &Pre.m_FrameQuat, &Nxt.m_FrameQuat, 1.0f - weight );
				//if( CTinyQuaternion( FrameQuat )._q != CTinyQuaternion( Cur.m_FrameQuat )._q )
				{
					itPre = it; 
					pKeyFrames->m_KeyQuatFrames[it->first] = it->second;
				}
			}
			pKeyFrames->m_KeyQuatFrames[it->first] = it->second;
			KeyFrames.m_KeyQuatFrames.clear();
		}

		//消除积累误差
		CMatrix mat;
		for( WORD i = 0; i < uMaxFrame; i++ )
		{
			map< WORD, TransFrameInfo >::const_iterator nextTran = pKeyFrames->m_KeyTransFrames.upper_bound( i );
			map< WORD, QuatFrameInfo >::const_iterator  nextQuat = pKeyFrames->m_KeyQuatFrames.upper_bound( i );
			map< WORD, TransFrameInfo >::const_iterator itTran = nextTran;
			map< WORD, QuatFrameInfo >::const_iterator  itQuat = nextQuat;
			itTran--;
			itQuat--;

			CVector3f FrameTrans;
			CVector3f FrameTrans1;
			CVector3f FrameTrans2;
			D3DXQUATERNION FrameQuat;
			D3DXQUATERNION FrameQuat1;
			D3DXQUATERNION FrameQuat2;
			float weightTran = float( nextTran->first - i )/( nextTran->first - itTran->first );
			float weightQuat = float( nextQuat->first - i )/( nextQuat->first - itQuat->first );

			FrameTrans1.x = int( itTran->second.m_FrameTrans.x*50 )/50.0f;
			FrameTrans1.y = int( itTran->second.m_FrameTrans.y*50 )/50.0f;
			FrameTrans1.z = int( itTran->second.m_FrameTrans.z*50 )/50.0f;
			FrameTrans2.x = int( nextTran->second.m_FrameTrans.x*50 )/50.0f;	
			FrameTrans2.y = int( nextTran->second.m_FrameTrans.y*50 )/50.0f;	
			FrameTrans2.z = int( nextTran->second.m_FrameTrans.z*50 )/50.0f;	
			FrameQuat1  = itQuat->second.m_FrameTinyQuat;
			FrameQuat2  = nextQuat->second.m_FrameTinyQuat;	
			FrameTrans  = FrameTrans1*weightTran + FrameTrans2*( 1.0f - weightTran );
			D3DXQuaternionSlerp( &FrameQuat, &FrameQuat1, &FrameQuat2, 1.0f - weightQuat );

			FrameQuat = itQuat->second.m_FrameQuat;
			FrameTrans.x = itTran->second.m_FrameTrans.x;
			FrameTrans.y = itTran->second.m_FrameTrans.y;
			FrameTrans.z = itTran->second.m_FrameTrans.z;
			

			D3DXMatrixRotationQuaternion( (D3DXMATRIX*)&mat, &FrameQuat );
			mat.m[3][0] = FrameTrans.x;
			mat.m[3][1] = FrameTrans.y;
			mat.m[3][2] = FrameTrans.z;

			mat = mat*ExpNode->m_Parent->m_Matrix[ min( i, ExpNode->m_Parent->m_Matrix.size() - 1 ) ];
			ExpNode->m_Matrix.push_back( mat );
			mat.Invert();
			//D3DXMatrixInverse( &mat, NULL, &mat );
			ExpNode->m_InvMatrix.push_back( mat );
		}

		map< WORD, TransFrameInfo >::const_iterator itTran = pKeyFrames->m_KeyTransFrames.end();
		map< WORD, QuatFrameInfo >::const_iterator  itQuat = pKeyFrames->m_KeyQuatFrames.end();
		itTran--;
		itQuat--;
		D3DXQUATERNION FrameQuat;
		FrameQuat = itQuat->second.m_FrameQuat;
		
		D3DXMatrixRotationQuaternion( (D3DXMATRIX*)&mat, &FrameQuat );
		mat.m[3][0] = itTran->second.m_FrameTrans.x;
		mat.m[3][1] = itTran->second.m_FrameTrans.y;
		mat.m[3][2] = itTran->second.m_FrameTrans.z;

		mat = mat*ExpNode->m_Parent->m_Matrix[ min( uMaxFrame, ExpNode->m_Parent->m_Matrix.size() - 1 ) ];
		ExpNode->m_Matrix.push_back( mat );
		mat.Invert();
		//D3DXMatrixInverse( &mat, NULL, &mat );
		ExpNode->m_InvMatrix.push_back( mat );
	}
	else
	{
		ExpNode->m_Matrix.resize(1);
		ExpNode->m_InvMatrix.resize(1);
		ExpNode->m_Matrix[0].Identity();
		ExpNode->m_InvMatrix[0].Identity();
		//D3DXMatrixIdentity( &ExpNode->m_Matrix[0] );
		//D3DXMatrixIdentity( &ExpNode->m_InvMatrix[0] );
	}

	//printf( "%s:%d, %d\n", ExpNode->m_3DMaxNode->GetName(), m_SkeletalID, pKeyFrames->m_KeyFrames.size() );

	//继续考察子节点
	for( vector<ExportNode*>::iterator it = ExpNode->m_Children.begin(); it != ExpNode->m_Children.end(); it++ )
	{
		//string tmp = (*it)->m_3DMaxNode->GetName();
		//size_t found;
		//found=tmp.find("Nub");
		//if( (*it)->m_PieceType == enumPT_SKELETAL && !(found!=string::npos))
		if( (*it)->m_PieceType == enumPT_SKELETAL)
		{
			CPluginSkeletal* SkeletalChild = new CPluginSkeletal;
			SkeletalChild->FillFromExportNode( *it, Animate, SkeletalList,IsExpAllFrame );
			m_Children.push_back( SkeletalChild );
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：根据骨骼名字返回骨骼ID
// 参数：const string Name  骨骼名字
// 返回：>0骨骼ID,否则失败
////////////////////////////////////////////////////////////////////////// 
int CPluginSkeletal::GetSkeletalIDByName( const string& Name )
{
	if( Name == m_SkeletalName )
		return m_SkeletalID;
	int ID = -1;
	for( int i = 0; i < (int)m_Children.size(); i++ )
		if( SUCCEEDED( ID = m_Children[i]->GetSkeletalIDByName( Name ) ) )
			return ID;
	return ID;
}


CPluginSkeletal* CPluginSkeletal::GetSkeletalByName(const string& Name)
{
	if( Name == m_SkeletalName )
		return this;
	CPluginSkeletal* pSke = NULL;
	for( int i = 0; i < (int)m_Children.size(); i++ )
		if( NULL != ( pSke = m_Children[i]->GetSkeletalByName( Name ) ) )
			return pSke;
	return pSke;
}

////////////////////////////////////////////////////////////////////////
// 功能：构造和析构
// 参数：略
// 返回：无
////////////////////////////////////////////////////////////////////////// 
CPluginSkeletal::CPluginSkeletal()
{
	m_SkeletalID      = 0;
	m_SkeletalName    = "";			//骨头名字
	m_Children.clear();
}

CPluginSkeletal::~CPluginSkeletal()
{
	Free();
}

void CPluginSkeletal::Free()
{
	for( int i=0; i<(int)m_Children.size(); i++ )
		delete m_Children[i];
	m_Children.clear();
}


//检查骨骼匹配
BOOL CPluginSkeletal::IsFitForMe( const CPluginSkeletal* Skeletal, string& ErrSkeletal )
{
	if( m_Children.size() == Skeletal->m_Children.size() )
	{
		for( int i = 0; i< (int)m_Children.size(); i++ )
		{
			if( !m_Children[i]->IsFitForMe( Skeletal->m_Children[i], ErrSkeletal ) )
				return FALSE;
		}
		return TRUE;
	}
	string Src = "文件中骨骼",Dest = "max中骨骼";
	Src += m_SkeletalName + "子骨骼:";
	Dest += Skeletal->m_SkeletalName + "子骨骼:";
	for( int i = 0; i < (int)m_Children.size(); i++ )
	{
		Src += m_Children[i]->m_SkeletalName;
		if(i!=(m_Children.size()-1))
			Src += ",";
	}
	for( int i = 0; i < (int)Skeletal->m_Children.size(); i++ )
	{
		Dest += Skeletal->m_Children[i]->m_SkeletalName;
		if(i!=(Skeletal->m_Children.size()-1))
			Dest += ",";
	}
	ErrSkeletal = Src + "<==>" + Dest;
	return FALSE;
}

int CPluginSkeletal::BinSave( ofstream& File )
{
	int Size = (int)m_SkeletalName.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	File.write( (TCHAR*)m_SkeletalName.data(), Size );
	File.write( (TCHAR*)&m_SkeletalID, sizeof(m_SkeletalID) );

	Size = (int)m_Children.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( vector<CPluginSkeletal*>::iterator it = m_Children.begin(); it!=m_Children.end(); it++ )
		(*it)->BinSave( File );

	return 0;
}

int CPluginSkeletal::BinLoad( ifstream& File, CPluginSkeletal** Buf )
{
	int Size;
	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_SkeletalName.resize( Size );
	File.read( (TCHAR*)m_SkeletalName.data(), Size );
	File.read( (TCHAR*)&m_SkeletalID, sizeof(m_SkeletalID) );

	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_Children.resize( Size, NULL );
	for( int i = 0; i < (int)m_Children.size(); i++ )
	{
		m_Children[i] = new CPluginSkeletal;
		m_Children[i]->BinLoad( File, Buf );
	}

	Buf[m_SkeletalID] = this;

	return 0;
}
CPluginPiece* CPluginPieceGroup::GetOldPieceByName(string pieceName)
{
	if(pieceName!="")
	{
		for ( int i = 0; i<m_Piece.size(); i++ )
		{
			for ( size_t j=0; j<m_Piece[i].size(); ++j )
			{
				if(pieceName == GetPiece(i, j)->m_sPieceName.c_str())
					return GetPiece(i, j);
			}
			
		}
	}
	return NULL;
}
int CPluginPieceGroup::PluginBinLoad( string LoadName )
{
	if( LoadName.size() == 0 )
		return GERROR;

	Free();
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( LoadName.data(), ios::binary|ios::in );
	locale::global(loc);
	if( File.fail() )
	{
		MessageBox( 0, LoadName.c_str(), "找不到文件", 0 );
		return GERROR;
	}

	DWORD Res;

	File.read( (TCHAR*)&Res, sizeof(DWORD) );
	File.read( (TCHAR*)&m_Version, sizeof(DWORD) );

	DWORD tVersionBox = GETBOX(m_Version);
	DWORD tVersion = GETVERSION(m_Version);
	DWORD tVersionCp = GETCP(m_Version);

	if( Res != MAKEFOURCC( 'A', 'R', 'P', 0 ) || tVersion > ARP_COMP_VERSION )//!=
	{
		if( Res != MAKEFOURCC( 'M', 'O', 'D', 0 ) || tVersion > ARP_COMP_VERSION )//!=
		{
			TCHAR Buf[256];
			sprintf( Buf, "当前程序版本：%f\n%s文件版本：%f", ARP_COMP_VERSION/10000.0f, LoadName.c_str(), tVersion/10000.0f );
			MessageBox( 0, Buf, "版本不匹配", 0 );
			Free();
			return GERROR;
		}
	}

	m_Piece.resize(1);
	int Size;
	File.read( (TCHAR*)&Size, sizeof(int) );
	m_Piece[0].resize(Size);
	for( int i = 0; i < (int)m_Piece.size(); i++ )
	{
		for ( size_t j = 0; j < m_Piece[i].size(); ++j )
		{
			m_Piece[i][j] = new CPluginPiece(this);
			if( FAILED( GetPiece(i, j)->BinLoad(  File, LoadName ,m_Version) ) )
			{
				Free();
				return GERROR;
			}
		}
	}
	// 头像信息
	if (!File.eof())
	{
		FourCC fourcc;
		File.read(reinterpret_cast<TCHAR*>(&fourcc), sizeof(fourcc));
		if (fourcc == ('PTIF'))
		{
			File.read(reinterpret_cast<TCHAR*>(&g_portraitInfo.translate), sizeof(g_portraitInfo) - sizeof(fourcc));
		}
	}
	File.close();

	return 0;
}
int CPluginPieceGroup::PluginBinSave( ofstream& File, string SaveName )
{	
	// zjtemp
#if 0
	BinSave(SaveName);
#else
	DWORD Version = ENABLECP(ARP_COMP_VERSION);
	Version = ENABLEBOX(Version);
	File.write( (TCHAR*)&RESTYPE( "MOD" ), sizeof(RESTYPE) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );

	vector<int> Capsules;

	int Size = 0;
	for( int i = 0; i < (int)m_Piece.size(); ++i)
	{
		for ( size_t j = 0; j < m_Piece[i].size(); ++j )
		{
			if( GetPiece(i, j)->GetMash()->GetExpNode()->m_nCapsules == -1 )
				Size++;
			else
				Capsules.push_back( i );
		}
	}

	vector<CapsuleNode> CapsuleNodes;
	if ( Capsules.size()>0) CapsuleNodes.resize( Capsules.size() / 2 ); // Fix a bug!!! .resize( Capsules.size() );
	for( int i = 0; i < (int)Capsules.size(); ++i)
	{
		int PieceIndex = Capsules[i];
		vector<CVector3f>& VerBuf = GetPiece(0, PieceIndex)->GetMash()->m_FramesVertex[0];
		CVector3f Pos( VerBuf[0] );
		float XMax = -1.0f * FLT_MAX , XMin = FLT_MAX;
		for( int j = 1; j < VerBuf.size(); ++j)
		{
			if ( VerBuf[j].x > XMax) XMax = VerBuf[j].x;
			if ( VerBuf[j].x < XMin) XMin = VerBuf[j].x;
			Pos += VerBuf[j];
		}
		Pos /= (float)VerBuf.size();

		ExportNode * Expn = GetPiece(0, PieceIndex)->GetMash()->GetExpNode();

		if ( Expn->m_nCapsuleSide == 1 )
		{
			CapsuleNodes[ Expn->m_nCapsules ].Begin = *(CVector3f*)&Pos;
		} else {
			CapsuleNodes[ Expn->m_nCapsules ].End = *(CVector3f*)&Pos;
		}

		CapsuleNodes[ Expn->m_nCapsules ].Raidus = abs( XMax - XMin ) / 2.0f;
		CapsuleNodes[ Expn->m_nCapsules ].w = CTinyWeight( GetPiece(0, PieceIndex)->GetMash()->m_Weight[0]._w[0],
			GetPiece(0, PieceIndex)->GetMash()->m_Weight[0]._w[1],
			GetPiece(0, PieceIndex)->GetMash()->m_Weight[0]._w[2],
			GetPiece(0, PieceIndex)->GetMash()->m_Weight[0]._w[3]);
		memcpy( (void*)&CapsuleNodes[ Expn->m_nCapsules ].b, (void*)&(GetPiece(0, PieceIndex)->GetMash()->m_SkeletalIndex[0]), sizeof( SkltIndex ) );
	}

	File.write( (TCHAR*)&Size, sizeof(int) );
	for( int i = 0; i < (int)m_Piece.size(); i++ )
	{
		for ( size_t j=0; j<m_Piece[i].size(); ++j )
		{
			if( GetPiece(i, j)->GetMash()->GetExpNode()->m_nCapsules == -1 )
			{
				if(g_bIsExistModFile)
				{
					CPluginPiece* piece = m_OldPieceGroup->GetOldPieceByName(GetPiece(i, j)->m_sPieceName.c_str());
					if(piece)
						GetPiece(i, j)->SetPiece(piece);
					else
						MessageBox(0,"没找到匹配的piece名,不能覆盖!", GetPiece(i, j)->m_sPieceName.c_str(),MB_OK);
				}
				GetPiece(i, j)->BinSave( File, SaveName, CapsuleNodes);
				g_SaveParam.m_uiSP_pInterface->ProgressUpdate( 5 + 95*i/GetPieceTotalNum() );
			}
		}
	}
	if(g_bIsExistModFile)
		File.write(reinterpret_cast<TCHAR*>(&g_portraitInfo), sizeof(g_portraitInfo));
#endif

	return 0;
}

//////////////////////////////////////////////////////////
// 功能：根据相应的m_ExpNode获得MESH信息
// 参数：CMotion& Motion       骨骼层次和动画
// 返回：无
//////////////////////////////////////////////////////////
void CPluginPieceGroup::GetMeshFromExpNode( CAnimateGroup& AnimateGroup )
{
	for( int i = 0; i < (int)m_Piece.size(); ++i )
	{
		for ( size_t j=0; j<m_Piece[i].size(); )
		{
			if( GetPiece(i, j)->GetMeshFromExpNode( AnimateGroup ) )
				j++;
			else
				m_Piece[i].erase( m_Piece[i].begin() + j );
		}
	}
}

void CPluginPieceGroup::AddPiece( CPluginPiece* Piece )
{
	m_Piece[0].push_back( Piece );
}

CPluginPiece* CPluginPieceGroup::GetPiece( size_t nClass, size_t nPiece )
{
	return static_cast<CPluginPiece*>(m_Piece[nClass][nPiece]);
}

size_t CPluginPieceGroup::GetPieceTotalNum()
{
	size_t nPieceNum = 0;
	for ( size_t i=0; i<m_Piece.size(); ++i )
	{
		nPieceNum += m_Piece[i].size();
	}
	return nPieceNum;
}

CPluginPiece* CPluginPieceGroup::GetPieceByName( const string& pieceName )
{
	if(pieceName!="")
	{
		for ( int i = 0; i<m_Piece.size(); i++ )
		{
			for ( size_t j=0; j<m_Piece[i].size(); ++j )
			{
				if(pieceName == GetPiece(i, j)->m_sPieceName.c_str())
					return GetPiece(i, j);
			}
		}
	}
	return NULL;
}

int CPluginPiece::BinLoad( ifstream& File, const string LoadName ,DWORD Version)
{	
	int Size;

	File.read( (char*)&Size, sizeof(int) );
	string szPieceName;
	szPieceName.resize(Size);
	File.read( (char*)&szPieceName[0], Size );
	m_sPieceName = szPieceName.c_str();

	const char* pStr = m_sPieceName.c_str();
	int i = 0;

	//切掉下划线_后的所有字符
	for( ; i < (int)m_sPieceName.size(); i++ )
		if( m_sPieceName[ m_sPieceName.size() - i - 1 ] == '_' )
			break;
	pStr += m_sPieceName.size() - i;
	m_ClassName = pStr;

	//去掉字符串中的数字
	for( i = (int)m_ClassName.size() - 1; i > 0 ; i-- )
	{
		if( m_ClassName[i] < '0' || m_ClassName[i] > '9' )
			break;
		m_ClassName.erase( i, 1 );
	}

	File.read( (TCHAR*)&Size, sizeof(int) );
	m_RenderStyle.resize( Size ); 
	DWORD tVersionLow = GETVERSION(Version);
	for( int i = 0; i < (int)m_RenderStyle.size(); i++ )
	{
		//对每一个piece加入对应的所有渲染类型rs
		//m_RenderStyle[i] = new CRenderStyle;
		m_RenderStyle[i] = new CPluginRenderStyle;
		GetPluginRenderStyle(i)->SetVersion(tVersionLow);
		GetPluginRenderStyle(i)->SetNum(i);
		if( FAILED( GetPluginRenderStyle(i)->BinLoad( File, LoadName ) ) )
			return GERROR;
	}

	size_t cur_pos = File.tellg();
	VERTEXTYPE vertType;
	File.read( (char*)&vertType, sizeof(vertType) );
	File.seekg(cur_pos, ios::beg);
	
	switch( vertType )
	{
	case VT_VERTEXANI:
		m_pPluginMesh = new CPluginVertexAniMesh;
		m_Mesh = (CMesh*)(CPluginVertexAniMesh*)(m_pPluginMesh);
		break;

	case VT_STATICVERTEX:
		m_pPluginMesh = new CPluginStaticMesh;
		m_Mesh = (CMesh*)(CPluginStaticMesh*)(m_pPluginMesh);
		break;

	default:
		m_pPluginMesh = new CPluginSkeletalMesh;
		m_Mesh = (CMesh*)(CPluginSkeletalMesh*)(m_pPluginMesh);
		break;
	}

	return m_pPluginMesh->BinLoad(File ,Version);
}

int CPluginPiece::BinSave( ofstream& File, string SaveName, vector<CapsuleNode> & CapsuleNodes )
{	
	int Size = 0;

	Size = (int)m_sPieceName.size();
	File.write( (char*)&Size, sizeof(int) );
	File.write( m_sPieceName.data(), Size );

	char szMeshName[256];
	sprintf_s( szMeshName, "_%s.msh", m_sPieceName.c_str() );
	string str = SaveName;
	str.erase( str.size() - 4, 4 );
	str += szMeshName;

	Size = (int)m_RenderStyle.size();


	File.write( (char*)&Size, sizeof(int) );
	for( int i = 0; i < (int)m_RenderStyle.size(); i++ )
	{
		if(g_bIsExistModFile&&m_oldpiece)
			GetPluginRenderStyle(i)->SetOldRenderSyle(m_oldpiece->GetPluginRenderStyle(i));
		GetPluginRenderStyle(i)->SetNum(i);
		GetPluginRenderStyle(i)->BinSave( File);
	}	
	if(g_bIsExistModFile&&m_oldpiece)
	{
		m_pPluginMesh->SetOldMesh(m_oldpiece->m_pPluginMesh);
	}
	m_pPluginMesh->BinSave( File, CapsuleNodes );
	return 0;
}

//////////////////////////////////////////////////////////
// 功能：获得MESH信息
// 参数：CMotion& Motion       骨骼层次和动画
// 返回：无
//////////////////////////////////////////////////////////
bool CPluginPiece::GetMeshFromExpNode( CAnimateGroup& AnimateGroup )
{
	m_pPluginMesh->SetMeshName(m_sPieceName.c_str());
	return m_pPluginMesh->GetMeshFromExpNode( AnimateGroup );
}

CPluginRenderStyle* CPluginPiece::GetPluginRenderStyle( size_t i )
{
	return static_cast<CPluginRenderStyle*>(m_RenderStyle[i]);
}

void CPluginPiece::SetPluginMesh( CPluginMesh* pMesh )
{
	m_pPluginMesh = pMesh;
}

void CPluginPiece::SetMesh( CMesh* pMesh )
{
	m_Mesh = pMesh;
}
int CPluginRenderStyle::BinLoad( ifstream& File, const string LoadName )
{	
	//static string texpath = g_D3d.GetTexturePath();

	int Size; 
	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_RenderStyleName.resize( Size );
	File.read( (TCHAR*)m_RenderStyleName.data(), Size );

	if(m_Version >= 10006)
	{
		File.read((TCHAR*)&Size,sizeof(Size));
		m_RSIndexName.resize(Size);
		File.read((TCHAR*)m_RSIndexName.data(),Size);
	}
	else
	{
		CString numstr;
		numstr.Format("%d",m_Num);
		string tmp = "默认";
		tmp += numstr.GetBuffer(numstr.GetLength());
		m_RSIndexName = tmp.c_str();
	}

	int TextNum = 0;
	size_t nPos = 0;
	File.read( (TCHAR*)&TextNum, sizeof(int) );
	m_TextureName.resize(TextNum);
	for( int i = 0; i < TextNum; i++ )
	{
		File.read( (TCHAR*)&Size, sizeof(Size) );
		string Name;
		Name.resize( Size );
		File.read( (TCHAR*)Name.data(), Size );
		m_TextureName[i] = Name.c_str();
	}
	return 0;
}
int CPluginRenderStyle::BinSave( ofstream& File )
{	
	int Size = 0; 

	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream pathFile( g_ARPPath.c_str(), ios::binary|ios::in );
	locale::global(loc);

	CString tmp;
	tmp.Format("%d",m_Num);
	tmp = "默认"+tmp;
	m_RSIndexName = tmp.GetBuffer(tmp.GetLength());

	if( !pathFile.fail() )
	{
		//读入每一个渲染状态文件名
		pathFile.read( (TCHAR*)&Size, sizeof(Size) );
		g_TexPath.resize( Size );
		pathFile.read( (TCHAR*)g_TexPath.data(), Size );

		pathFile.read( (TCHAR*)&Size, sizeof(Size) );
		g_RSName.resize( Size );
		pathFile.read( (TCHAR*)g_RSName.data(), Size );

		if( !g_RSName.empty() )
		{
			size_t npos = g_RSName.rfind(".rs");
			if(npos != -1)
			{
				g_RSName = g_RSName.substr(0, npos);
				g_RSName += ".marl";
			}
		}
		else
			g_RSName = "commonrs\\default.marl";

		Size = (int)g_RSName.size();
		File.write( (TCHAR*)&Size, sizeof(Size) );
		File.write( g_RSName.data(), Size );

		Size = (int)m_RSIndexName.size();
		File.write( (TCHAR*)&Size,sizeof(Size) );
		File.write( m_RSIndexName.data(), Size);

		Size = m_TextureName.size();
		File.write( (TCHAR*)&Size, sizeof(int) );
		for(int i=0;i<m_TextureName.size();i++)
		{
			string texName;
			size_t nPos = m_TextureName[i].find('.');
			texName = m_TextureName[i].c_str();
			if(m_TextureName[i].find("dds")==string::npos)
			{
				texName.clear();
				texName = ("\\" + m_TextureName[i].substr(0, nPos)).c_str();
				texName = g_TexPath + texName;
				texName += ".tex";
			}	

			Size = (int)texName.size();
			File.write( (TCHAR*)&Size, sizeof(Size) );
			File.write( texName.data(), Size );
		}
	}
	else
	{
		int rsNameLen = (int)m_RenderStyleName.size();
		int rsIndexNameLen = (int)m_RSIndexName.size();
		size_t texNum = m_TextureName.size();
		if( g_bIsExistModFile&&m_oldrenderStyle )
		{
			rsNameLen			= m_oldrenderStyle->m_RenderStyleName.size();
			m_RenderStyleName	= m_oldrenderStyle->m_RenderStyleName;
			rsIndexNameLen		= m_oldrenderStyle->m_RSIndexName.size();
			m_RSIndexName		= m_oldrenderStyle->m_RSIndexName;
			texNum = m_oldrenderStyle->m_TextureName.size();
		}
	
		File.write( (TCHAR*)&rsNameLen, sizeof(Size) );
		File.write( m_RenderStyleName.data(), rsNameLen );
		File.write( (TCHAR*)&rsIndexNameLen,sizeof(Size) );
		File.write( m_RSIndexName.data(), rsIndexNameLen);
		
		File.write((TCHAR*)&texNum, sizeof(int));
		if(g_bIsExistModFile&&m_oldrenderStyle)
		{
			for(size_t i = 0; i<m_oldrenderStyle->m_TextureName.size(); ++i)
			{
				Size = (int)m_oldrenderStyle->m_TextureName[i].size();
				File.write( (TCHAR*)&Size, sizeof(Size) );
				string tex = m_oldrenderStyle->m_TextureName[i].c_str();
				File.write( tex.data(), Size );
			}
		}
		else
		{
			for(size_t i = 0; i<m_TextureName.size(); ++i)
			{
				Size = (int)m_TextureName[i].size();
				string sTex = m_TextureName[i].c_str();
				Size = sTex.size();
				File.write( (char*)&Size, sizeof(Size) );
				File.write( sTex.data(), sTex.size() );
				// 临时修改，解决编译不过问题
				//CModRsTexPathCfgMgr::Inst()->AddPathLog(sTex);
			}
		}
	}

	return 0;
}

HRESULT CPluginRenderStyle::_CreateRenderStyle( const char* szName, CRenderStyle** pRS )
{
	return S_OK;
}

HRESULT CPluginRenderStyle::_CreateStyleTexture( const TCHAR* szTexName )
{
	m_TextureName.push_back(szTexName);
	
	return 0;
}

TransFrameInfo CKeyFrames::GetTransFrameInfo(WORD FrameNo)
{
	map<WORD,TransFrameInfo>::iterator it = m_KeyTransFrames.find(FrameNo);
 	if( it !=  m_KeyTransFrames.end() )
 	{
 		return it->second;
 	}
 	else
 	{	
		map<WORD,TransFrameInfo>::iterator minIt = m_KeyTransFrames.begin();
		map<WORD,TransFrameInfo>::iterator maxIt = m_KeyTransFrames.end();
	
		if( FrameNo < minIt->first )
			return minIt->second;
		
		for( ;minIt != maxIt && minIt->first<FrameNo; ++minIt );
		map<WORD,TransFrameInfo>::iterator temp = minIt;
		--temp;
		if( minIt == maxIt ) 
			return temp->second;	
		
		TransFrameInfo RetTrans;

		float Weight = float(FrameNo - temp->first)/float(minIt->first - temp->first);
		RetTrans.m_FrameTrans =  minIt->second.m_FrameTrans * Weight + temp->second.m_FrameTrans * (1.0f - Weight);
		return RetTrans;
	}
}

QuatFrameInfo CKeyFrames::GetQuatFrameInfo(WORD FrameNo)
{
	map<WORD,QuatFrameInfo>::iterator it = m_KeyQuatFrames.find(FrameNo);
	if( it !=  m_KeyQuatFrames.end() )
	{
		return it->second;
	}
	else
	{
		map<WORD,QuatFrameInfo>::iterator minIt = m_KeyQuatFrames.begin();
		map<WORD,QuatFrameInfo>::iterator maxIt = m_KeyQuatFrames.end();
		if( FrameNo < minIt->first )
			return minIt->second;
		for( ;minIt != maxIt && minIt->first<FrameNo; ++minIt );
		map<WORD,QuatFrameInfo>::iterator temp = minIt;
		--temp;
		if( minIt == maxIt ) 
			return temp->second;	
		QuatFrameInfo RetQuat;
		float Weight = float(FrameNo - temp->first)/float(minIt->first - temp->first);
		D3DXQuaternionSlerp( &(RetQuat.m_FrameQuat), &(minIt->second.m_FrameQuat), &(temp->second.m_FrameQuat), 1.0f - Weight );
		RetQuat.m_FrameTinyQuat = CPluginTinyQuaternion(RetQuat.m_FrameQuat);
		return RetQuat;
	}
}

bool CKeyFrames::InsertFrame( WORD FrameNo,const TransFrameInfo& Tran,const QuatFrameInfo& Quat )
{
	m_KeyTransFrames[FrameNo] = Tran;
	m_KeyQuatFrames[FrameNo] = Quat;
	return true;
}

bool CKeyFrames::IsHaveTransFrame( WORD FrameNo )
{
	map<WORD,TransFrameInfo>::iterator it = m_KeyTransFrames.find(FrameNo);
	return it != m_KeyTransFrames.end();
}

bool CKeyFrames::IsHaveQuatFrame( WORD FrameNo )
{
	map<WORD,QuatFrameInfo>::iterator it = m_KeyQuatFrames.find(FrameNo);
	return it != m_KeyQuatFrames.end();
}

CAnimateGroup::CAnimateGroup()
{
	m_SkeletalTotalNum = 0;
	m_FrameCircle=0;
	m_AniName = "";
	//CMatrix					m_Globe;
	//CSkeletal					m_SkeletalRoot;			//根骨骼
	m_SkeletalList.clear();			//所有骨骼矩阵（SOCKET）单元列表
	m_AnimationList.clear();		//所有动画列表
	m_IdNameList.clear();
}

CAnimateGroup::~CAnimateGroup() 
{ 
	Free(); 
}

bool CAnimateGroup::TransToProAni(void)
{
	//return true;
	const string MagicBoneName = "Bip SpinePro";
	int ID = m_SkeletalRoot.GetSkeletalIDByName(MagicBoneName.c_str());
	CPluginSkeletal* pSkePelvis = m_SkeletalRoot.GetSkeletalByName("Bip01 Pelvis");
	if( NULL == pSkePelvis )
		return true;
	CPluginSkeletal* pSkeSpine = pSkePelvis->GetSkeletalByName("Bip01 Spine");

	if( NULL == pSkeSpine || m_AnimationList.size()==0 )
		return true;
	BYTE SkePelvisID = pSkePelvis->m_SkeletalID;
	BYTE SkeSpineID = pSkeSpine->m_SkeletalID;

	if( -1==ID  )
	{
		OutputDebugString("需要转换");
		int ThighID = pSkeSpine->GetSkeletalIDByName("Bip01 L Thigh");
		CPluginAnimate* pAni = m_AnimationList[0];
		if( -1 != ThighID )
		{
			MessageBox( NULL,"请把这个人的三角裤头搞掉","温馨提示：",0);
			return false;
		}

		if(pAni == NULL)
		{
			MessageBox( NULL,"没有找到有效的动画","温馨提示：",0);
			return false;
		}
		
		//修改骨架----------------------------------------------------------------
		CPluginSkeletal* pMagicBone = new CPluginSkeletal;
		pMagicBone->m_SkeletalID = pAni->GetSkeletalNum();
		pMagicBone->m_SkeletalName = MagicBoneName;
		pMagicBone->m_Children.push_back(pSkeSpine);
		++m_SkeletalTotalNum;
		vector<CPluginSkeletal*>::iterator it,eit = pSkePelvis->m_Children.end();

		bool isStrict = false; //严格性判断
		for(it= pSkePelvis->m_Children.begin(); it!=eit; ++it)
		{
			if(*it == pSkeSpine )
			{
				pSkePelvis->m_Children.erase(it);
				isStrict = true;
				break;
			}
		}
		pSkePelvis->m_Children.push_back(pMagicBone);

		if(!isStrict)
		{
			delete pMagicBone;
			MessageBox( NULL,"骨骼命名不严格，请检查","宇宙帅哥袁海林温馨提示：",0);
			return false;
		}

		//控制骨架位置----------------------------------------------------------
		CPluginSkeletalUnit* pMagicSkeUnit = new CPluginSkeletalUnit(pMagicBone);
		m_SkeletalList.push_back(pMagicSkeUnit);
		pMagicSkeUnit->m_InverseMatrix = m_SkeletalList[SkePelvisID]->m_InverseMatrix;

		//修改导出动画----------------------------------------------------------
		size_t AinNo = m_AnimationList.size();
		for(size_t i = 0; i < AinNo; ++i)
		{
			CPluginAnimate* pModifyAni = m_AnimationList[i];
			if(NULL == pModifyAni || pModifyAni->GetSkeletalNum()==m_SkeletalTotalNum)
			{
				continue;
			}

			CKeyFrames* pMagicKeyFrame = new CKeyFrames;
			CKeyFrames* pPelKey = pModifyAni->GetKeyFrames(SkePelvisID);
			CKeyFrames* pPinKey = pModifyAni->GetKeyFrames(SkeSpineID);


			map<WORD,TransFrameInfo>::iterator PelItTran , EPelItTran = pPelKey->m_KeyTransFrames.end();
			map<WORD,QuatFrameInfo>::iterator PelItQuat , EPelItQuat = pPelKey->m_KeyQuatFrames.end();
			OutputDebugString("BeginStep");
			for( PelItTran = pPelKey->m_KeyTransFrames.begin();	PelItTran != EPelItTran; ++PelItTran )
			{
				TransFrameInfo Tran = PelItTran->second;
				Tran.Inverse(); 
				pMagicKeyFrame->m_KeyTransFrames[PelItTran->first] = Tran;
			}


			for( PelItQuat = pPelKey->m_KeyQuatFrames.begin(); PelItQuat != EPelItQuat; ++PelItQuat )
			{
				QuatFrameInfo  Quat = PelItQuat->second;
				Quat.Inverse();
				pMagicKeyFrame->m_KeyQuatFrames[PelItQuat->first] = Quat;
			}

			EPelItTran = pPinKey->m_KeyTransFrames.end();
			EPelItQuat = pPinKey->m_KeyQuatFrames.end();

			for(PelItQuat = pPinKey->m_KeyQuatFrames.begin();PelItQuat != EPelItQuat;++PelItQuat)
			{				
				QuatFrameInfo  Quat = pPelKey->GetQuatFrameInfo(PelItQuat->first);
				D3DXQuaternionMultiply(&(PelItQuat->second.m_FrameQuat),&(PelItQuat->second.m_FrameQuat),&(Quat.m_FrameQuat));// 16次乘法 12次加法
				PelItQuat->second.m_FrameTinyQuat = CPluginTinyQuaternion(PelItQuat->second.m_FrameQuat);
			}

			for( PelItTran = pPinKey->m_KeyTransFrames.begin(); PelItTran != EPelItTran; ++PelItTran )
			{
				TransFrameInfo Tran = pPelKey->GetTransFrameInfo(PelItTran->first);
				QuatFrameInfo  Quat = pPelKey->GetQuatFrameInfo(PelItTran->first);
				D3DXQuaternionVectorMultiply(PelItTran->second.m_FrameTrans,Quat.m_FrameQuat,PelItTran->second.m_FrameTrans);// 19次乘法 12次加法
				D3DXVec3Add((D3DXVECTOR3*)&(PelItTran->second.m_FrameTrans),(D3DXVECTOR3*)&(PelItTran->second.m_FrameTrans),(D3DXVECTOR3*)&(Tran.m_FrameTrans));// 3次加法				
			}


			pModifyAni->AddSkeletalKeyFrames(pMagicKeyFrame);
		}
	}
	return true;
}

int CAnimateGroup::BinSaveFromMax( ofstream& File, string SaveName ,vector<IDNAME>& IdNameList)
{
	if( !TransToProAni() )
		return 0;
	OutputDebugString("文件保存开始:BinSaveFromMax");
	float FrameCircle = MS_PER_FRAME;
	CMatrix Globe;
	Globe.Identity();
	//D3DXMatrixIdentity( &Globe );

	DWORD Version = ARA_COMP_VERSION;
	File.write( (TCHAR*)&RESTYPE( "SKE" ), sizeof(RESTYPE) );

	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	File.write( (TCHAR*)&FrameCircle, sizeof(float) );
	File.write( (TCHAR*)&Globe, sizeof(CMatrix) );
	File.write( (TCHAR*)&m_SkeletalTotalNum, sizeof(m_SkeletalTotalNum) );

	m_SkeletalRoot.BinSave( File );

	int Size;
	int i;
	Size = (int)m_SkeletalList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
		m_SkeletalList[i]->BinSave( File );

	Size = (int)IdNameList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );

	for( i = 0; i < (int)IdNameList.size(); i++ )
	{
		IDNAME Name = IdNameList[i];
		File.write( (TCHAR*)(&Name), sizeof(IDNAME) );
	}
	return 0;
}
int CAnimateGroup::BinSaveFromFile( ofstream& File, string SaveName )
{
	if( !TransToProAni() )
		return 0;
	OutputDebugString("文件保存开始:BinSaveFromFile");
	DWORD Version = AGP_COMP_VERSION;
	File.write( (TCHAR*)&RESTYPE( "AGP" ), sizeof(RESTYPE) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	
	m_IndexSkeName = SaveName.substr(0,SaveName.size()-4);

	m_IndexSkeName = tranString(m_IndexSkeName);


	int dataSize = (int)m_IndexSkeName.size();
	File.write( (TCHAR*)&dataSize, sizeof(dataSize) );
	File.write( m_IndexSkeName.data(), dataSize );

	int Size;
	int i;
	Size = (int)m_AnimationList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );

	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		IDNAME Name = m_AnimationList[i]->GetName();
		File.write( (TCHAR*)(&Name), sizeof(IDNAME) );

		TCHAR szAniName[1024];
		TCHAR AniName[1024];
		string sAniName;
		sprintf_s( AniName, "%s", (LPCTSTR)Name );
		sAniName = AniName;
		if(sAniName == m_AniName)
		{
			sprintf_s( szAniName, "_%s.ani", (LPCTSTR)Name );

			string str = SaveName;
			str.erase( str.size() - 4, 4 );
			str += szAniName;
			m_AnimationList[i]->BinSave( str );
		}
	}
	return 0;
}



int	CAnimateGroup::BinLoadAgp( string LoadName )
{
	m_IdNameList.clear();

	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( LoadName.data(), ios::binary|ios::in );
	locale::global(loc);
	if( File.fail() )
	{
		MessageBox( 0, LoadName.c_str(), "找不到文件", 0 );
		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (TCHAR*)&Res, sizeof(DWORD) );
	File.read( (TCHAR*)&Version, sizeof(DWORD) );
	bool bAraMark = false;

	if ( Res != MAKEFOURCC( 'A', 'G', 'P', 0 ) || Version != AGP_COMP_VERSION )
	{
		TCHAR Buf[256];
		sprintf_s( Buf, "当前程序版本：%f\n%s文件版本：%f", ARA_COMP_VERSION/10000.0f, LoadName.c_str(), Version/10000.0f );
		MessageBox( 0, Buf, "版本不匹配", 0 );
		return GERROR;
	}
	if(Version>=10002)
	{
		int dataSize;
		File.read((TCHAR*)&dataSize,sizeof(int));
		m_IndexSkeName.resize(dataSize);
		File.read((TCHAR*)m_IndexSkeName.data(), dataSize );
	}

	int Size;
	int i;
	File.read( (TCHAR*)&Size, sizeof(Size) );
	//m_AnimationList.resize( Size, NULL );

	TCHAR* szPreName = (TCHAR*)LoadName.c_str();
	szPreName[LoadName.size() - 4] = 0;

	for( i = 0; i < Size; i++ )
	{
		IDNAME Name;
		File.read( (TCHAR*)(&Name), sizeof(IDNAME) );
		TCHAR szAniName[256];
		TCHAR aniname[256];
		string saniname;
		sprintf_s( aniname, "%s.ani", (LPCTSTR)Name );
		saniname = aniname;
		if(saniname == m_AniName)
			continue;
		sprintf_s( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)Name );

		m_IdNameList.push_back(Name);
	}
	File.close();

	return 0;
}

int	CAnimateGroup::BinSaveSke( ofstream& File,string SaveName )
{
	return 1;
}

int	CAnimateGroup::BinSaveAgp( string SaveName )
{
	//string agpfilename = SaveName.substr(0,SaveName.size()-4)+".agp";
	ofstream AgpFile( SaveName.data(), ios::binary|ios::out );
	DWORD Version = AGP_COMP_VERSION;
	AgpFile.write( (TCHAR*)&RESTYPE( "AGP" ), sizeof(RESTYPE) );
	AgpFile.write( (TCHAR*)&Version, sizeof(DWORD) );

	int dataSize = (int)m_IndexSkeName.size();
	AgpFile.write( (TCHAR*)&dataSize, sizeof(dataSize) );
	AgpFile.write( m_IndexSkeName.data(), dataSize );


	int Size = (int)m_IdNameList.size();
	AgpFile.write( (TCHAR*)&Size, sizeof(Size) );

	for(int i = 0; i < (int)m_IdNameList.size(); i++ )
	{
		IDNAME Name = m_IdNameList[i];
		AgpFile.write( (TCHAR*)(&Name), sizeof(IDNAME) );
	}

	//int Size = (int)m_AnimationList.size();
	//AgpFile.write( (TCHAR*)&Size, sizeof(Size) );

	for(int i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		IDNAME Name = m_AnimationList[i]->GetName();
		//AgpFile.write( (TCHAR*)(&Name), sizeof(IDNAME) );

		TCHAR szAniName[1024];

		sprintf( szAniName, "_%s.ani", (LPCTSTR)Name );

		string str = SaveName;
		str.erase( str.size() - 4, 4 );
		str += szAniName;
		m_AnimationList[i]->BinSave( str );
	}
	AgpFile.close();
	
	return 1;
}

int CAnimateGroup::BinSave( ofstream& File, string SaveName )
{
	if( !TransToProAni() )
		return 0;
	OutputDebugString("文件保存开始:BinSave");

	float FrameCircle = MS_PER_FRAME;
	CMatrix Globe;
	Globe.Identity();
	//D3DXMatrixIdentity( &Globe );

	DWORD Version = ARA_COMP_VERSION;
	File.write( (TCHAR*)&RESTYPE( "SKE" ), sizeof(RESTYPE) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	File.write( (TCHAR*)&FrameCircle, sizeof(float) );
	File.write( (TCHAR*)&Globe, sizeof(CMatrix) );
	File.write( (TCHAR*)&m_SkeletalTotalNum, sizeof(m_SkeletalTotalNum) );

	m_SkeletalRoot.BinSave( File );
	
	int Size;
	int i;
	Size = (int)m_SkeletalList.size();

	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
		m_SkeletalList[i]->BinSave( File );
	File.close();
	OutputDebugString("骨骼保存完毕");

/*	string agpfilename = SaveName.substr(0,SaveName.size()-4)+".agp";
	ofstream AgpFile( agpfilename.data(), ios::binary|ios::out );
	Version = AGP_COMP_VERSION;
	AgpFile.write( (TCHAR*)&RESTYPE( "AGP" ), sizeof(RESTYPE) );
	AgpFile.write( (TCHAR*)&Version, sizeof(DWORD) );

	m_IndexSkeName = SaveName.substr(0,SaveName.size()-4);

	m_IndexSkeName = tranString(m_IndexSkeName);

	int dataSize = (int)m_IndexSkeName.size();
	AgpFile.write( (TCHAR*)&dataSize, sizeof(dataSize) );
	AgpFile.write( m_IndexSkeName.data(), dataSize );

	Size = (int)m_AnimationList.size();
	AgpFile.write( (TCHAR*)&Size, sizeof(Size) );
	
	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		IDNAME Name = m_AnimationList[i]->GetName();
		AgpFile.write( (TCHAR*)(&Name), sizeof(IDNAME) );

		TCHAR szAniName[1024];
		
		sprintf( szAniName, "_%s.ani", (LPCTSTR)Name );
		
		string str = SaveName;
		str.erase( str.size() - 4, 4 );
		str += szAniName;
		m_AnimationList[i]->BinSave( str );
	}
	AgpFile.close();
	*/
	return 0;
}

BOOL CAnimateGroup::IsFitForMe( CAnimateGroup& AG, string& ErrSkeletal )
{
	return m_SkeletalRoot.IsFitForMe( &AG.m_SkeletalRoot, ErrSkeletal );
}

BOOL CAnimateGroup::IsFitSkeForMe( CAnimateGroup& AG, string& ErrSkeletal )
{
	BOOL mark = TRUE;
	mark = m_SkeletalRoot.IsFitForMe( &AG.m_SkeletalRoot, ErrSkeletal );
	if(mark==FALSE)
		return mark;
	vector<CPluginSkeletalUnit*>& tSkeList = AG.GetSkeList();
	if(m_SkeletalList.size()!=tSkeList.size())
		return FALSE;
	for(int i = 0;i<m_SkeletalList.size();i++)
	{
		if(m_SkeletalList[i]->m_InverseMatrix!=tSkeList[i]->m_InverseMatrix)
			return FALSE;
	}
	return TRUE;
}

int CAnimateGroup::BinLoadSke( string LoadName )
{
	Free();
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( LoadName.data(), ios::binary|ios::in );
	locale::global(loc);
	if( File.fail() )
	{
		MessageBox( 0, LoadName.c_str(), "找不到文件", 0 );
		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (TCHAR*)&Res, sizeof(DWORD) );
	File.read( (TCHAR*)&Version, sizeof(DWORD) );
	bool bAraMark = false;

	if( Res != MAKEFOURCC( 'A', 'R', 'A', 0 ) || Version != ARA_COMP_VERSION )
	{
		if( Res != MAKEFOURCC( 'S', 'K', 'E', 0 ) || Version != ARA_COMP_VERSION )
		{
			TCHAR Buf[256];
			sprintf_s( Buf, "当前程序版本：%f\n%s文件版本：%f", ARA_COMP_VERSION/10000.0f, LoadName.c_str(), Version/10000.0f );
			MessageBox( 0, Buf, "版本不匹配", 0 );
			return GERROR;
		}
		else
			bAraMark = false;
	}
	else
		bAraMark = true;

	File.read( (TCHAR*)&m_FrameCircle, sizeof(float) );
	File.read( (TCHAR*)&m_Globe, sizeof(CMatrix) );
	File.read( (TCHAR*)&m_SkeletalTotalNum, sizeof(m_SkeletalTotalNum) );

	CPluginSkeletal* S[256];
	m_SkeletalRoot.BinLoad( File, S );

	int Size;
	int i;
	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_SkeletalList.resize( Size, NULL );
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		m_SkeletalList[i] = new CPluginSkeletalUnit( S[i] );
		m_SkeletalList[i]->BinLoad( File );
	}
	File.close();
	return 0;
}
int CAnimateGroup::BinLoad( string LoadName )
{
	//Free();
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( LoadName.data(), ios::binary|ios::in );
	locale::global(loc);
	if( File.fail() )
	{
		MessageBox( 0, LoadName.c_str(), "找不到文件", 0 );
		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (TCHAR*)&Res, sizeof(DWORD) );
	File.read( (TCHAR*)&Version, sizeof(DWORD) );
	bool bAraMark = false;

	if ( Res != MAKEFOURCC( 'A', 'G', 'P', 0 ) || Version != AGP_COMP_VERSION )
	{
		TCHAR Buf[256];
		sprintf_s( Buf, "当前程序版本：%f\n%s文件版本：%f", ARA_COMP_VERSION/10000.0f, LoadName.c_str(), Version/10000.0f );
		MessageBox( 0, Buf, "版本不匹配", 0 );
		return GERROR;
	}
	if(Version>=10002)
	{
		int dataSize;
		File.read((TCHAR*)&dataSize,sizeof(int));
		m_IndexSkeName.resize(dataSize);
		File.read((TCHAR*)m_IndexSkeName.data(), dataSize );
		replace(m_IndexSkeName.begin(), m_IndexSkeName.end(), '\\', '/');
		int pos = m_IndexSkeName.rfind("/");
		m_IndexSkeName = m_IndexSkeName.substr(pos+1,m_IndexSkeName.size()-pos);
	}

	int Size;
	int i;
	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_AnimationList.resize( Size, NULL );

	TCHAR* szPreName = (TCHAR*)LoadName.c_str();
	szPreName[LoadName.size() - 4] = 0;

	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		IDNAME Name;
		File.read( (TCHAR*)(&Name), sizeof(IDNAME) );
		TCHAR szAniName[256];
		TCHAR aniname[256];
		string saniname;
		sprintf_s( aniname, "%s.ani", (LPCTSTR)Name );
		saniname = aniname;
		if(saniname == m_AniName)
			continue;
		sprintf_s( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)Name );

		m_AnimationList[i] = new CPluginAnimate;

		m_AnimationList[i]->SetName( Name );
		m_IdNameList.push_back(Name);
	}

	return 0;
}

void CAnimateGroup::Free()
{ 
	int i;
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
		delete m_SkeletalList[i];
	for( i = 0; i < (int)m_AnimationList.size(); i++ )
		delete m_AnimationList[i];
	m_SkeletalTotalNum = 0;  
	m_SkeletalRoot.Free();
	m_SkeletalList.clear();
	m_AnimationList.clear();
	//m_IdNameList.clear();
}
////////////////////////////////////////////////////////////////////////
// 功能：填充动画信息
// 参数：ExportNode* Root      根节点
// 返回：>0成功,否则失败
////////////////////////////////////////////////////////////////////////// 
int CAnimateGroup::FillFromExportNode( ExportNode* Root )
{
	m_SkeletalTotalNum = 0;
	int MaxFrame = 0;
	IDNAME Name = g_SaveParam.m_uiSP_AnimationName;

	CPluginAnimate* Animate = new CPluginAnimate;
	Animate->SetName( Name );
	m_SkeletalRoot.FillFromExportNode( Root, *Animate, m_SkeletalList ,m_IsExpAllFrame);
	m_SkeletalTotalNum = Animate->GetSkeletalNum();
	m_AnimationList.push_back( Animate );

	return 0;
}

int CPluginSkeletalUnit::BinSave( ofstream& File )
{
	int i;
	File.write( (TCHAR*)&m_InverseMatrix, sizeof( CMatrix ) );

	int Size = (int)m_SocketList.size();
	File.write( (TCHAR*)&Size, sizeof( Size ) );
	for( i = 0; i < (int)m_SocketList.size(); i++ )
	{
		File.write( m_SocketList[i].m_SocketName, sizeof( IDNAME ) );
		File.write( (TCHAR*)&m_SocketList[i].m_Socket, sizeof( CMatrix ) );
	}
	return 0;
}

int CPluginSkeletalUnit::BinLoad( ifstream& File )
{
	int i;
	File.read( (TCHAR*)&m_InverseMatrix, sizeof( CMatrix ) );

	int Size;
	File.read( (TCHAR*)&Size, sizeof( Size ) );
	m_SocketList.resize( Size );
	for( i = 0; i < (int)m_SocketList.size(); i++ )
	{
		File.read( (TCHAR*)&m_SocketList[i].m_SocketName, sizeof( IDNAME ) );
		File.read( (TCHAR*)&m_SocketList[i].m_Socket, sizeof( CMatrix ) );
	}

	return 0;
}

int CPluginAnimate::BinSave( string SaveName )
{
	ofstream File( SaveName.data(), ios::binary|ios::out );

	DWORD Version = ANF_VERSION;
	float FrameRatio = 33.33f;
	float fTranParam = m_fMaxLength/_CovData::eCD_WGT_MSK;

	m_IsCpAni = g_IsCpSke==TRUE;
	File.write( (TCHAR*)&RESTYPE( "ANI" ), sizeof(RESTYPE) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	File.write( (TCHAR*)&m_MaxFrame, sizeof(m_MaxFrame) );
	File.write( (TCHAR*)&FrameRatio, sizeof(FrameRatio) );
	File.write( (TCHAR*)&m_fScale, sizeof(float) );
	File.write( (TCHAR*)&fTranParam, sizeof(float) );
	File.write(	(TCHAR*)&m_GroundSpeed, sizeof(float) );
	File.write( (TCHAR*)&m_IsCpAni,sizeof(bool));

	int Size = (int)m_SkeletalList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( int i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		int TranFramNum = (int)m_SkeletalList[i]->m_KeyTransFrames.size();
		File.write( (TCHAR*)&TranFramNum, sizeof(TranFramNum) );
		for( map<WORD,TransFrameInfo>::iterator it = m_SkeletalList[i]->m_KeyTransFrames.begin(); 
			it != m_SkeletalList[i]->m_KeyTransFrames.end(); it++ )
		{
			File.write( (TCHAR*)&( it->first ), m_MaxFrame < 256 ? sizeof(BYTE) : sizeof(WORD) );
			if (m_IsCpAni)
			{
				CTinyVector v( it->second.m_FrameTrans, 1.0f/fTranParam );
				File.write( (TCHAR*)&v, sizeof(CTinyVector) );
			}
			else
			{
				CVector3f v = it->second.m_FrameTrans;
				File.write( (TCHAR*)&v, sizeof(CVector3f) );
			}
		}

		int QuatFramNum = (int)m_SkeletalList[i]->m_KeyQuatFrames.size();
		File.write( (TCHAR*)&QuatFramNum, sizeof(QuatFramNum) );
		for( map<WORD,QuatFrameInfo>::iterator it = m_SkeletalList[i]->m_KeyQuatFrames.begin(); 
			it != m_SkeletalList[i]->m_KeyQuatFrames.end(); it++ )
		{
			File.write( (TCHAR*)&( it->first ), m_MaxFrame < 256 ? sizeof(BYTE) : sizeof(WORD) );
			if(m_IsCpAni)
				File.write( (TCHAR*)&( it->second.m_FrameTinyQuat ), sizeof(CPluginTinyQuaternion) );
			else
				File.write( (TCHAR*)&(it->second.m_FrameQuat), sizeof(D3DXQUATERNION));
		}
	}

	Size = (int)m_FrameString.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( map<WORD,IDNAME>::iterator it = m_FrameString.begin(); it != m_FrameString.end(); it++, i++ )
	{
		File.write( (TCHAR*)( &(*it).first ), m_MaxFrame < 256 ? sizeof(BYTE) : sizeof(WORD) );
		File.write( (TCHAR*)( &(*it).second ), sizeof( IDNAME ) );
	}
	return 0;
}

