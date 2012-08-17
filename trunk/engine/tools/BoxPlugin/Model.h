#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

#include "GraphicCommon.h"
#include "GLobD3d.h"
#include <map>
#include <vector>
#include "iparamm.h"
#include "dummy.h"
#include "Simpobj.h"
#include "surf_api.h"
#include "MNMath.h"
#include "PolyObj.h"
#include "CAxisAlignedBox.h"
using namespace sqr;
#define ANI_SCALE				0x00000001L

using namespace std;
//--------------------------------------------------------------------------------
// 文件：ExpPlugin.h
// 日期：2004.3.22
// 功能：定义模型储存结构
//------------------------------------------------------------------------------------
// 日期：2007.1.19
// 功能：定义披风相关结构 :)
//------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// 披风碰撞胶囊结构



// 3DMAX模型节点的类型
enum PIECETYPE 
{ 
	enumPT_VERTEXANI,               //顶点动画
	enumPT_SKELETALANI,				//骨骼动画
	enumPT_NONEANI,					//骨骼动画
	enumPT_SKELETAL,				//骨骼
	enumPT_SOCKET,					//SOCKET
	enumPT_UNKNOW=0xffffffff,		//未定义的类型
};

enum PIECECPTYPE
{
	enumPT_COMPRESS,				//压缩
	enumPT_UNCOMPRESS,				//不压缩
};

enum NODETYPE
{
	enumNT_MESH,
	enumNT_SKELETALMESH,
	enumNT_SKELETAL,
	enumNT_UNKNOW,
};

struct ListNode
{
	ListNode( NODETYPE NodeType, INode* MaxNode )
	{
		m_NodeType = NodeType;
		m_MaxNode = MaxNode;
		if( NodeType == enumNT_SKELETAL )
			m_ExpType = enumPT_SKELETAL;
		if( NodeType == enumNT_MESH )
			m_ExpType = enumPT_NONEANI;
		if( NodeType == enumNT_SKELETALMESH )
			m_ExpType = enumPT_SKELETALANI;
	};

	PIECETYPE m_ExpType;
	NODETYPE m_NodeType;
	INode*   m_MaxNode;
};



//相应的字符串
static const TCHAR s_PICENAME[4][20]={"PT_VERTEXANI", "PT_SKELETALANI", "PT_SKELETAL", "PT_SOCKET"};

class ExportNode
{
public:
	INode*				m_3DMaxNode;	// 3DMax节点
	ExportNode*			m_Parent;		// 父节点
	vector<ExportNode*> m_Children; 	// 子节点
	PIECETYPE			m_PieceType;	// 节点类型
	BOOL				m_bSoftLinkBone;
    BOOL                m_bSoftCloth;   // 软布（披风)

    int                 m_nCapsules;        // 胶囊编号
    int                 m_nCapsuleSide;    // 起始 1 头， 2 尾

	map<float,int>		m_AddBoneWeight;

	vector<D3DXMATRIX>	m_Matrix;
	vector<D3DXMATRIX>	m_InvMatrix;

	int					m_nMaxFrame;


	ExportNode() : m_3DMaxNode(NULL), m_Parent(NULL){};
	~ExportNode()
	{
		
		for( UINT i = 0; i < m_Children.size(); i++ )delete m_Children[i]; 
	};
};

//用于标志动画名字
class IDNAME
{
	INT64      m_Buf[2]; 

public:
	IDNAME()
	{ 
		memset( m_Buf, 0, 16 ); 
	}
	IDNAME( LPCTSTR str )
	{
		memset( m_Buf, 0, 16 );
		if( str )
		{
			size_t len = min( strlen( str ), 15 );
			memcpy( m_Buf, str, len );
		}
	} 

	const IDNAME& operator= ( const IDNAME& BN )
	{
		memset(m_Buf,0,16);
		m_Buf[0] = BN.m_Buf[0];
		m_Buf[1] = BN.m_Buf[1];
		/*m_Buf[2] = BN.m_Buf[2];
		m_Buf[3] = BN.m_Buf[3];*/
		return *this;
	}

	const IDNAME& operator= ( LPCTSTR str )
	{
		memset( m_Buf, 0, 16 );
		if( str )
		{
			size_t len = min( strlen( str ), 15 );
			memcpy( m_Buf, str, len );
		}
		return *this;
	}

	BOOL operator== ( const IDNAME& BN )
	{
		return 	m_Buf[0] == BN.m_Buf[0] && m_Buf[1] == BN.m_Buf[1];
	}

	operator LPCTSTR() const
	{
		return (LPCTSTR)m_Buf;
	}
};

enum SaveType
{
	eST_Piece			= 0,
	eST_Skeletal		= 1,
	eST_SoftBody		= 2,
    eST_SoftCloth       = 3,
};

//保存参数
struct SAVEPARAM
{
	SAVEPARAM()
	{ 
		memset( this, 0, sizeof( SAVEPARAM ) );
		m_uiSP_Size = sizeof( SAVEPARAM );
	}
	~SAVEPARAM()
	{
		
		memset( this, 0, sizeof( SAVEPARAM ) );
		m_uiSP_Size = sizeof( SAVEPARAM );
	}

	Interface*		m_uiSP_pInterface;
	BOOL			m_uiSP_Select;

	UINT			m_uiSP_Size;				//本结构大小
	UINT			m_uiSP_WeightPerVertex;		//每个顶点最多几个权重
	UINT			m_uiSP_FramePerSample;		//隔多少帧取样一帧
	LPCTSTR			m_uiSP_AnimationName;	    //动画名字
	void*			m_uiSP_pNodeType;			//动画名字
	SaveType        m_eSP_SaveType;				//保存类型
	UINT			m_uiSP_NodeNum;				//节点数量
	BOOL			m_bSP_SmoothAll;			//光滑所有面
	BOOL			m_bSP_RootScale;			//缩放根骨骼
	UINT			m_fLevel;					//动作精度
	BOOL			m_dwSP_Reserved[6];			//保留
public:
	void Free()
	{
		memset( this, 0, sizeof( SAVEPARAM ) );
		m_uiSP_Size = sizeof( SAVEPARAM );
	}
};

//渲染类型
struct CRenderStyle
{
	string		m_RenderStyleName;			//渲染类型
	string		m_RSIndexName;
	vector<string>      m_TextureName;				//贴图
	int m_Num;
	//int ASCSave( CSaxXmlWriter& Node );
	void			SetNum(int i)					{ m_Num = i;}
};

struct TransFrameInfo
{
	D3DXVECTOR3					m_FrameTrans;			//动画关键帧平移
	CTinyVector					m_FrameTinyTrans;
};

struct QuatFrameInfo
{
	D3DXQUATERNION				m_FrameQuat;			//动画关键帧旋转
	CTinyQuaternion				m_FrameTinyQuat;		//动画关键帧旋转
};

struct CKeyFrames
{
	map<WORD,TransFrameInfo>	m_KeyTransFrames;		//关键帧的集合
	map<WORD,QuatFrameInfo>		m_KeyQuatFrames;		//关键帧的集合
};

class CAnimate
{
	int							m_MaxFrame;				//动画最大帧数
	float						m_FrameRatio;			//帧率
	float						m_fScale;
	float						m_fTransParam;
	float						m_GroundSpeed;			//每个动作对地的速度
	float						m_fMaxLength;
	IDNAME						m_AnimationeName;		//动画名
	vector<CKeyFrames*>			m_SkeletalList;			//骨骼的集合
	map<WORD,IDNAME>			m_FrameString;			//关键帧字符串

public:
	CAnimate():m_MaxFrame(0),m_fScale(1.0f),m_fMaxLength(0),m_GroundSpeed(0){}
	~CAnimate()
	{
		for( int i = 0; i < (int)m_SkeletalList.size(); i++ )
			delete m_SkeletalList[i];
		
	}

	float	GetTransParam()										{ return m_fTransParam;	}
	int		GetSkeletalNum()								{ return (int)m_SkeletalList.size(); }
	IDNAME	GetName()										{ return m_AnimationeName; };
	void	SetName( IDNAME& Name )							{ m_AnimationeName = Name; };
	void	SetMaxFrame( int MaxFrame )						{ m_MaxFrame = max( MaxFrame, m_MaxFrame ); }
	void	AddSkeletalKeyFrames( CKeyFrames* KeyFrames )	{ m_SkeletalList.push_back( KeyFrames ); }
	void	SetScale( float fScale )						{ m_fScale = fScale;	}
	float	GetScale()										{ return m_fScale; }
	void	SetMaxLength( float fMaxLength )				{ m_fMaxLength = max( abs( fMaxLength ), m_fMaxLength );	}
	float	GetMaxLength()									{ return m_fMaxLength;	}
	const CKeyFrames& GetKeyFrame( UINT SkeletalID )
	{
		if( m_SkeletalList.size() == 1 )
			return *m_SkeletalList[0];

		//if(m_SkeletalList.empty())
		//	return nu
		return *m_SkeletalList[SkeletalID];
	}
	//int ASCSave( CSaxXmlWriter& Node );
};

//骨骼的单个SOCKET
struct CSkeletalSocket
{
	CSkeletalSocket(){ m_SocketName = 0; D3DXMatrixIdentity( &m_Socket ); }

	IDNAME						m_SocketName;			//SOCKET名字
	D3DXMATRIX					m_Socket;				//SOCKET矩阵
};
class CSkeletal;
//每根骨骼的SOCKET集合
struct CSkeletalUnit
{
	CSkeletalUnit(){}
	CSkeletalUnit( CSkeletal* pSkeletal ) : m_pSkeletal(pSkeletal){}
	CSkeletal*					m_pSkeletal;
	D3DXMATRIX					m_InverseMatrix;		//每个骨骼上面的皮肤从其全局空间到骨骼局部的矩阵
	vector<CSkeletalSocket>		m_SocketList;			//骨骼上SOCKET的列表

	//int ASCSave( CSaxXmlWriter& Node );

};
struct CRenderMatrix;
//骨骼动画
class CSkeletal
{
	BYTE						m_SkeletalID;           //骨头ID
	string						m_SkeletalName;			//骨头名字

	vector<CSkeletal*>			m_Children;				//子关节

public:
	CSkeletal();
	~CSkeletal();
	void Free();
	int FillFromExportNode( ExportNode* ExpNode, CAnimate& Animate, vector<CSkeletalUnit*>& SkeletalList );
	int GetSkeletalIDByName( const string& Name );
	/// 根据骨骼ID得到骨骼名（遍历自身和子骨骼）
	string	GetSkeletalNameByID( BYTE SkeletalID ) const;
	/// 计算骨骼矩阵
	void	CalculateMatrix(const D3DXMATRIX& matWorld
		, CRenderMatrix& RenderMatrix
		, const vector<CAnimate*>& Animates
		, const vector<CSkeletalUnit*>& SkeletalUnit
		, bool& CompleteFlag ) const;


	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( ofstream& File );
	int		BinLoad( ifstream& File, CSkeletal** Buf );
	BOOL	IsFitForMe( const CSkeletal* Skeletal, string& ErrSkeletal );	//匹配骨骼
};

//动画组
class CAnimateGroup
{
	BYTE						m_SkeletalTotalNum;		//骨骼数量

	float						m_FrameCircle;
	D3DXMATRIX					m_Globe;
	CSkeletal					m_SkeletalRoot;			//根骨骼
	vector<CSkeletalUnit*>		m_SkeletalList;			//所有骨骼矩阵（SOCKET）单元列表
	vector<CAnimate*>			m_AnimationList;		//所有动画列表
	vector<IDNAME>				m_IdNameList;

public:
	CAnimateGroup();
	~CAnimateGroup();

	//得到动画列表
	vector<CAnimate*>& GetAnimateList()
	{ 
		return m_AnimationList; 
	}
	vector<IDNAME>& GetIdNameList()
	{
		return	m_IdNameList;
	}
	void  Free();

	int   FillFromExportNode( ExportNode* Root );
	DWORD GetTotalNum()								{ return m_SkeletalTotalNum; }
	int   GetSkeletalIDByName( const string Name )	{ return m_SkeletalRoot.GetSkeletalIDByName( Name ); }
	/// 得到指定骨骼矩阵单元
	CSkeletalUnit* GetSkeleteUnit( int Num );

	/// 根据骨骼ID得到骨骼名（遍历自身和子骨骼）
	string	GetSkeletalNameByID( BYTE SkeletalID );

	/// 计算骨骼渲染矩阵
	void  CalculateMatrix( const D3DXMATRIX& matWorld, CRenderMatrix& RenderMatrix, bool& CompleteFlag ) const;

//	int   ASCSave( CSaxXmlWriter& Node );

	//骨骼匹配
	BOOL IsFitForMe( const CAnimateGroup& AG, string& ErrSkeletal );
	inline int GetSkeletalNum()
	{
		return (int)m_SkeletalList.size();
	}
};

enum VERTEXTYPE 
{ 
	VT_VERTEXANI	= 0, 
	VT_STATICVERTEX	= 1,
	VT_SKELETALANI	= 2,
	VT_SOFTBODYANI	= 3, 
    VT_SOFTCLOTHANI = 4,
	VT_UNKNOW=0xffffffff
};

//相应的字符串
static const TCHAR s_VERTEXTYPE[2][20]={ "VT_VERTEXANI", "VT_SKELETALANI" };

struct SklWeight
{ 
	float f[4]; 
	float GetWeight( unsigned nIndex ) const { return f[nIndex]*0.005f;}
};
struct SkltIndex{ BYTE  b[4]; };

struct CVertexAndGroup
{
	int				m_index;
	DWORD			m_nGroup;

	CVertexAndGroup(const int& ind,DWORD nGroup)
	{
		m_index=ind;
		m_nGroup=nGroup;
	}

	bool operator<(const CVertexAndGroup& ano)const
	{
		if (m_index<ano.m_index)
			return true;
		else if(m_index>ano.m_index)
			return false;
		else
			return m_nGroup<ano.m_nGroup;
	}

	bool operator ==(const CVertexAndGroup& ano)const
	{
		return m_index == ano.m_index && m_nGroup == ano.m_nGroup;
	}
};
struct CMeshLevel
{
	CMeshLevel() : m_fDist(0){}
	virtual ~CMeshLevel(){}

	float							m_fDist;			//启用距离	
	DWORD							m_FramePerKeyFrame;	//顶点动画采样率
	vector< vector<D3DXVECTOR3> >	m_FramesVertex;		//多帧顶点缓冲区
	vector< vector<D3DXVECTOR3> >	m_FramesNormal;		//多帧法线缓冲区
	vector<DWORD>					m_Diffuse;			//顶点颜色
	vector<D3DXVECTOR2>				m_TextCoord;		//贴图坐标
	vector<D3DXVECTOR2>				m_TextCoord1;
	vector<D3DXVECTOR2>				m_TextCoord2;
	vector<SklWeight>				m_Weight;			//顶点骨骼权重
	vector<SkltIndex>				m_SkeletalIndex;	//顶点骨骼索引
	vector<WORD>					m_VertexIndex;		//面缓冲区
	int		GetVerNum()							{ return (int)m_FramesVertex[0].size(); }
};

class CMesh
{	
	VERTEXTYPE				m_eVertexType;					//顶点类型
	float					m_fBaize;						//z偏移
	D3DXVECTOR3				m_Scale;						//模型在自身坐标空间的大小
	D3DXVECTOR3				m_Off;							//模型中心对自身坐标空间原点的偏移量
	vector<CMeshLevel*>		m_MeshLevels;					//多层LOD
	D3DXVECTOR2				m_MinUV;
	D3DXVECTOR2				m_MaxUV;

	vector<UVVert>			m_uvBuffer;						//临时管理一层uv坐标
	string					m_ObjectName;					//Mesn名字
	int						m_Channel;						//有多少个
	bool					m_IsCompress;					//是否压缩这个piece
	bool					m_tIsCompress;					
	
	ExportNode*				m_ExpNode;
public:
	typedef	map<sqr::uint16, CBaseAABB>					BoxMap;

	BoxMap					m_BoxMap;						//包围盒列表
	CMesh();
	~CMesh();

	VERTEXTYPE	GetVertexType()			{ return m_eVertexType; }
	void	SetMeshName(string name) {m_ObjectName = name;}
	string	GetMeshName()	{return m_ObjectName;}
	void	SetIsCompress(bool bIsCompress) { m_tIsCompress = bIsCompress; }
	void	asynNewCompressState()	{m_IsCompress = m_tIsCompress ;}
	void	asynOldCompressStete()	{m_tIsCompress = m_IsCompress;}
	bool	GetIsCompress(){return m_IsCompress;}
    vector<CMeshLevel*>&	GetMeshLevels() { return m_MeshLevels; }
    ExportNode* GetExpNode() { return m_ExpNode; }
	void	SetExpNode( ExportNode* ExpNode ){ m_ExpNode = ExpNode; }
	bool	GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
	void	GetPhysiqueWeight( vector<SklWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
	void	GetSkinWeight( vector<SklWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
	void	GetBPWeight( vector<SklWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
	void	FormatFromMaxMesh( Mesh& MaxMesh, vector<VerSkinNCT>& Vertex, vector<WORD>& VertexIndex );

	//int ASCSave( CSaxXmlWriter& Node );
};

//PIECE
class CPiece
{
	string					m_Name;				//Piece名字
	string					m_ClassName;		//Piece种类名
    CMesh*					m_Mesh;				//Piece的MESH
	vector<CRenderStyle>    m_RenderStyle;		//Piece的渲染类型

public:
	CPiece();
	~CPiece();
	string&	GetPieceName() { return m_Name; }
	bool GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
	void SetMesh( CMesh* Mesh )							{ m_Mesh = Mesh; };
	void AddRenderStyle( CRenderStyle& RenderStyle )	{ m_RenderStyle.push_back( RenderStyle );};
    CMesh*  GetMash()       { return m_Mesh; }
	//int ASCSave( CSaxXmlWriter& Node );
};

class CPieceGroup
{
public:
	vector<CPiece>			m_Piece;
	void GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
	void AddPiece( CPiece& Piece ){ m_Piece.push_back( Piece ); }

	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( ofstream& File, string SaveName );
	void Free() { m_Piece.clear();}
};

struct RESTYPE
{
	RESTYPE(){ dwResFlag = 0; };
	RESTYPE( LPCTSTR Type )
	{ 
		dwResFlag = 0;
		memcpy( &dwResFlag, Type, min( 4, strlen( Type ) ) ); 
	};

	DWORD dwResFlag;

	operator DWORD(){ return dwResFlag;	}
};
struct MatrixUnit
{
	D3DXVECTOR3			m_FrameTrans;			///<记录每帧骨骼运动的平移变换
	D3DXQUATERNION		m_FrameQuat;			///<记录每帧骨骼运动的旋转变换
	D3DXMATRIX			m_CurSkeletalMatrix;			///<当前骨骼到世界坐标的矩阵
	D3DXMATRIX			m_GlobeSkeletalMatrix;			///<当前骨骼到世界坐标的矩阵
	CSkeletalUnit*		m_SkeletalUnit;					///<和骨骼相联系的SOCKET集合
};
struct AniResult
{
	AniResult() : m_FrameString(NULL), m_SepFrameString(NULL), m_CurFrame(0.0f), m_IsAniEnd(false), m_InAniBlend(false), m_IsSepAniEnd(false) {}
	AniResult( const IDNAME* pFrameString, float fCurFrame, bool bIsAniEnd, bool bInAniBlend , const IDNAME& pAName = NULL, const IDNAME &pSAName = NULL)
		: m_FrameString(pFrameString), m_CurFrame(fCurFrame), m_IsAniEnd(bIsAniEnd), m_InAniBlend(bInAniBlend)
		, m_PreAnimationName(pAName), m_PreSepAnimationName(pSAName) 
		, m_AniframeIndex(999999), m_SepAniframeIndex(999999){}
	IDNAME			m_PreAnimationName;
	IDNAME			m_PreSepAnimationName;
	const IDNAME*	m_FrameString;		//帧特征字符串
	const IDNAME*	m_SepFrameString;	//帧特征字符串
	sqr::int32			m_AniframeIndex;
	sqr::int32			m_SepAniframeIndex;
	float			m_CurFrame;		//渲染的当前帧
	bool			m_IsAniEnd;		//动画播放完毕
	bool			m_InAniBlend;   //动画单元融合中

	bool			m_IsSepAniEnd;	//分离动画完毕（上半身动画播放完毕)
};
struct CRenderMatrix
{
	CRenderMatrix();
	~CRenderMatrix();
	void SetAnimateGroup( CAnimateGroup* AnimateGroup );

	UINT				m_SkeletalNum;				///<骨骼数目

	BOOL				m_IsBlendStart;				///<是否进入前后帧混合状态
	BOOL				m_IsAniBlend;				///<是否需要进行前后帧混合
	BOOL				m_IsLoop;

	float				m_PreFrameNO;				///<上一帧的帧数

	MatrixUnit*			m_MatrixUnit;				///<保存矩阵计算结果

	UINT				m_CurAnimate;				///<当前在计算的动画在CAnimateGroup中的位置
	float				m_CurFrame;					///<当前帧

	UINT				m_NextAnimate;				///<下一个要播放的动画
	float				m_NextBlendFrame;			///<在下一个动画播放前需要进行融合的帧数

	DWORD				m_CurrentTime;
	AniResult		m_Result;
};
struct BoxNode
{
	INode* node;
	string BoxNodeName;
	Matrix3 mat;
	Matrix3 newMat;
	Point3 boxCenter;
	Point3 pmax;
	Point3 pmin;
};

class CModel
{
	CPieceGroup				m_Pieces;           //网格
	ExportNode*				m_Root;				//根节点
	CAnimateGroup			m_Motions;			//动画
	CAnimateGroup			m_AnimateGroup;

	Interface*				m_ip;
	DummyObject*			m_dumObj;
	IObjCreate*				m_createInterface;
	Point3					max;
	Point3					min;
	SAVEPARAM               m_SaveParam;
	vector< ListNode* >		m_NodeType;
	vector<BoxNode>	m_vecNode;
	//vector<string>			m_SelectBone;
	vector<BoxNode>	m_NodeAndMatrix;
	vector<string>			m_SelectNodeName;
	vector<CMesh*>			m_vecMesh;
public:
	CModel(void);
	~CModel(void);
	void Free();

	int Save(string FileName);
	int SaveFromFile(string FileName);
	int SaveFrom3DMax(string FileName);
	bool IsSkeletal( INode* MaxNode );
	bool IsNodeMesh( INode* MaxNode, Interface* pInterface );
	NODETYPE GetNodeType( INode* MaxNode, Interface* pInterface, BOOL IsSelect );
	UINT NodeEnum( INode* MaxNode, Interface* pInterface, BOOL IsSelect, vector<ListNode*>& List,UINT& nSkeletal );
	void SetAnimateGroup(); 
	void BuildBigBox();
	void BuildAABBBox();
	void SetBigBox();
	void SetAABBBox();
	void Init(Interface *ip);
	void UnInit();
	/// 得到矩阵
	void GetMatrix( D3DXMATRIX& Matrix, UINT SkeletalID,string nodeName ,bool isGlobe);
	int AddSkeletalBox( const string& SkName );

	/// 计算矩阵
	INode* GetINodeByName(string nodeName);
	void FixPose();
	void ResumePose();
	void DelAllBox();
	void SelAllNode();
	void ReCalBox();
	void UpdateBox();
	void Update();
	UINT NodeEnum(INode* MaxNode);
	void BuildAllBox();
	void GetSelectNodes();
	void AddBoxs();
	bool IsFindBoxNode(string nodename);
	void ClearBoxMap();
private:

	ExportNode*	NodeEnum( INode* MaxNode, ExportNode* Parent );

public:
	CPieceGroup& GetCPieceGroup();
	int			FillFrom3DMax(const SAVEPARAM& SaveParam);
	void		FixParent( ExportNode* Root );
};




#endif
