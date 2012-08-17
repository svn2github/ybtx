#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

#include "Exp.h"
#include "GraphicBase.h"
#include "GraphicCommon.h"
#include "CAxisAlignedBox.h"
#include "CMesh.h"
#include "CEditPieceGroup.h"
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

#pragma pack( push, 1 )
struct CapsuleNode{
    CVector3f   Begin;
    CVector3f   End;
    float       Raidus;
    CTinyWeight w;		// float       w[4];
    BYTE        b[4];   
	CapsuleNode():w(0,0,0,0){}
};
#pragma pack(pop)

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

	vector<CMatrix>	m_Matrix;
	vector<CMatrix>	m_InvMatrix;

	int					m_nMaxFrame;


	ExportNode() : m_3DMaxNode(NULL), m_Parent(NULL){};
	~ExportNode()
	{
		
		for( UINT i = 0; i < m_Children.size(); i++ )delete m_Children[i]; 
	};
};

//用于标志动画名字
//class IDNAME
//{
//	INT64      m_Buf[2]; 
//
//public:
//	IDNAME()
//	{ 
//		memset( m_Buf, 0, 16 ); 
//	}
//	IDNAME( LPCTSTR str )
//	{
//		memset( m_Buf, 0, 16 );
//		if( str )
//		{
//			size_t len = min( strlen( str ), 15 );
//			memcpy( m_Buf, str, len );
//		}
//	} 
//
//	const IDNAME& operator= ( const IDNAME& BN )
//	{
//		memset(m_Buf,0,16);
//		m_Buf[0] = BN.m_Buf[0];
//		m_Buf[1] = BN.m_Buf[1];
//		/*m_Buf[2] = BN.m_Buf[2];
//		m_Buf[3] = BN.m_Buf[3];*/
//		return *this;
//	}
//
//	const IDNAME& operator= ( LPCTSTR str )
//	{
//		memset( m_Buf, 0, 16 );
//		if( str )
//		{
//			size_t len = min( strlen( str ), 15 );
//			memcpy( m_Buf, str, len );
//		}
//		return *this;
//	}
//
//	BOOL operator== ( const IDNAME& BN )
//	{
//		return 	m_Buf[0] == BN.m_Buf[0] && m_Buf[1] == BN.m_Buf[1];
//	}
//
//	operator LPCTSTR() const
//	{
//		return (LPCTSTR)m_Buf;
//	}
//};

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

struct BoxNode
{
	INode* m_Node;
	string m_BoxName;
	string m_MeshName;
	string m_BoneName;
	CVector3f m_vMinimum;
	CVector3f m_vMaximum;
	uint16 m_Id;
};

//渲染类型
struct CPluginRenderStyle : public CPieceRenderStyle
{
public:
	int					m_Num;
	CPluginRenderStyle*		m_oldrenderStyle;
	CPluginRenderStyle()	{ m_RSIndexName=""; m_oldrenderStyle = NULL;}
	
	int					BinSave( ofstream& File );
	int					BinLoad( ifstream& File, const string LoadName );
	void				SetNum(int i)						{ m_Num = i;}
	void				SetVersion(DWORD ver)				{ m_Version = ver; }
	void				SetOldRenderSyle(CPluginRenderStyle* oldrendersytle){m_oldrenderStyle = oldrendersytle;}

	using CPieceRenderStyle::m_TextureName;
	using CPieceRenderStyle::m_RSIndexName;
	using CPieceRenderStyle::m_RenderStyleName;

protected:
	virtual HRESULT 
			_CreateRenderStyle( const char* szName, CRenderStyle** pRS );

	virtual HRESULT	
			_CreateStyleTexture( const TCHAR* szTexName );
};

struct TransFrameInfo
{
	CVector3f					m_FrameTrans;			//动画关键帧平移

	void Inverse(void)
	{
		m_FrameTrans = CVector3f(-m_FrameTrans.x,-m_FrameTrans.y,-m_FrameTrans.z);
	}
};

struct QuatFrameInfo
{
	D3DXQUATERNION				m_FrameQuat;			//动画关键帧旋转
	CPluginTinyQuaternion				m_FrameTinyQuat;		//动画关键帧旋转

	const QuatFrameInfo& operator = ( const QuatFrameInfo& Quat )
	{
		m_FrameQuat = Quat.m_FrameQuat;
		m_FrameTinyQuat = Quat.m_FrameTinyQuat;
		return *this;
	}

	void Inverse(void)
	{
		float InvNorm = m_FrameQuat.x * m_FrameQuat.x + 
			m_FrameQuat.y * m_FrameQuat.y + 
			m_FrameQuat.z * m_FrameQuat.z + 
			m_FrameQuat.w * m_FrameQuat.w;

		if( InvNorm  > 0.0 )
		{	
			InvNorm = float(1.0/InvNorm);
			m_FrameQuat = D3DXQUATERNION( -InvNorm*m_FrameQuat.x, -InvNorm*m_FrameQuat.y, -InvNorm*m_FrameQuat.z, InvNorm*m_FrameQuat.w);
		}
		else
			m_FrameQuat = D3DXQUATERNION(0.0f,0.0f,0.0f,0.0f);
		m_FrameTinyQuat = CPluginTinyQuaternion(m_FrameQuat);
	}
};

struct CKeyFrames
{
	map<WORD,TransFrameInfo>	m_KeyTransFrames;		//关键帧的集合
	map<WORD,QuatFrameInfo>		m_KeyQuatFrames;		//关键帧的集合

	TransFrameInfo GetTransFrameInfo(WORD FrameNo);
	QuatFrameInfo GetQuatFrameInfo(WORD FrameNo);

	bool InsertFrame( WORD FrameNo, const TransFrameInfo& Tran,const QuatFrameInfo& Quat );
	bool IsHaveTransFrame( WORD FrameNo );
	bool IsHaveQuatFrame( WORD FrameNo );
};

class CPluginAnimate
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
	bool						m_IsCpAni;				//是否压缩骨骼动画

public:
	CPluginAnimate():m_MaxFrame(0),m_fScale(1.0f),m_fMaxLength(0),m_GroundSpeed(0),m_IsCpAni(true){}
	~CPluginAnimate()
	{
		for( int i = 0; i < (int)m_SkeletalList.size(); i++ )
			delete m_SkeletalList[i];
	}

	int		GetSkeletalNum()								{ return (int)m_SkeletalList.size(); }
	IDNAME	GetName()										{ return m_AnimationeName; };
	void	SetName( IDNAME& Name )							{ m_AnimationeName = Name; };
	void	SetMaxFrame( int MaxFrame )						{ m_MaxFrame = max( MaxFrame, m_MaxFrame ); }
	void	AddSkeletalKeyFrames( CKeyFrames* KeyFrames )	{ m_SkeletalList.push_back( KeyFrames ); }
	void	SetScale( float fScale )						{ m_fScale = fScale;	}
	void	SetMaxLength( float fMaxLength )				{ m_fMaxLength = max( abs( fMaxLength ), m_fMaxLength );	}
	float	GetMaxLength()									{ return m_fMaxLength;	}
	CKeyFrames*	GetKeyFrames(BYTE SkeId)					{ return m_SkeletalList[SkeId]; }
	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( string SaveName );
};

//骨骼的单个SOCKET
//struct CSkeletalSocket
//{
//	CSkeletalSocket(){ m_SocketName = 0; D3DXMatrixIdentity( &m_Socket ); }
//
//	IDNAME						m_SocketName;			//SOCKET名字
//	CMatrix					m_Socket;				//SOCKET矩阵
//};
class CPluginSkeletal;
//每根骨骼的SOCKET集合
struct CPluginSkeletalUnit
{
	CPluginSkeletalUnit(){}
	CPluginSkeletalUnit( CPluginSkeletal* pSkeletal ) : m_pSkeletal(pSkeletal){}
	CPluginSkeletal*					m_pSkeletal;
	CMatrix					m_InverseMatrix;		//每个骨骼上面的皮肤从其全局空间到骨骼局部的矩阵
	vector<CSkeletalSocket>		m_SocketList;			//骨骼上SOCKET的列表

	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( ofstream& File );
	int BinLoad( ifstream& File );
};

//骨骼动画
class CPluginSkeletal
{
public:
	BYTE						m_SkeletalID;           //骨头ID
	string						m_SkeletalName;			//骨头名字
	vector<CPluginSkeletal*>			m_Children;				//子关节
public:
	CPluginSkeletal();
	~CPluginSkeletal();
	void Free();
	int FillFromExportNode( ExportNode* ExpNode, CPluginAnimate& Animate, vector<CPluginSkeletalUnit*>& SkeletalList ,BOOL IsExpAllFrame);
	int GetSkeletalIDByName( const string& Name );
	CPluginSkeletal* GetSkeletalByName(const string& Name);

	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( ofstream& File );
	int		BinLoad( ifstream& File, CPluginSkeletal** Buf );
	BOOL	IsFitForMe( const CPluginSkeletal* Skeletal, string& ErrSkeletal );	//匹配骨骼
};

//动画组
class CAnimateGroup
{
	BYTE						m_SkeletalTotalNum;		//骨骼数量

	float						m_FrameCircle;
	CMatrix					m_Globe;
	CPluginSkeletal					m_SkeletalRoot;			//根骨骼
	vector<CPluginSkeletalUnit*>		m_SkeletalList;			//所有骨骼矩阵（SOCKET）单元列表
	vector<CPluginAnimate*>			m_AnimationList;		//所有动画列表
	vector<IDNAME>				m_IdNameList;
	BOOL						m_IsExpAllFrame;
	string						m_IndexSkeName;

	string						m_AniName;

public:
	CAnimateGroup();
	~CAnimateGroup();

	//得到动画列表
	void SetAniName(string& aniname){m_AniName = aniname;}
	vector<CPluginSkeletalUnit*>& GetSkeList(){return m_SkeletalList;}
	vector<CPluginAnimate*>& GetAnimateList()
	{ 
		return m_AnimationList; 
	}
	void SetExpFrame(BOOL IsExpAllFrame)
	{
		m_IsExpAllFrame = IsExpAllFrame;
	}
	vector<IDNAME>& GetIdNameList()
	{
		return	m_IdNameList;
	}
	void  Free();

	int   FillFromExportNode( ExportNode* Root );
	DWORD GetTotalNum()								{ return m_SkeletalTotalNum; }
	void  SetIndxSkeName(string& name)				{ m_IndexSkeName = name; }
	string& GetIndxSkeName()						{ return m_IndexSkeName; }
	int   GetSkeletalIDByName( const string Name )	{ return m_SkeletalRoot.GetSkeletalIDByName( Name ); }

//	int   ASCSave( CSaxXmlWriter& Node );
	int		BinSave( ofstream& File, string SaveName );
	int		BinSaveFromFile( ofstream& File, string SaveName );
	int		BinSaveFromMax( ofstream& File, string SaveName,vector<IDNAME>& IdNameList);
	int		BinLoad( string FileName );
	int		BinLoadSke( string LoadName );

	int		BinLoadAgp( string LoadName );
	int		BinSaveSke( ofstream& File,string SaveName );
	int		BinSaveAgp( string SaveName );
	//骨骼匹配
	BOOL  IsFitForMe( CAnimateGroup& AG, string& ErrSkeletal );
	BOOL  IsFitSkeForMe( CAnimateGroup& AG, string& ErrSkeletal );

	//	  转换为上下半身分离骨骼
	bool  TransToProAni(void);
};

//enum VERTEXTYPE 
//{ 
//	VT_VERTEXANI	= 0, 
//	VT_STATICVERTEX	= 1,
//	VT_SKELETALANI	= 2,
//	VT_SOFTBODYANI	= 3, 
//    VT_SOFTCLOTHANI = 4,
//	VT_UNKNOW=0xffffffff
//};

//相应的字符串
static const TCHAR s_VERTEXTYPE[2][20]={ "VT_VERTEXANI", "VT_SKELETALANI" };

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


//struct CBaseAABB //用于存储的结构
//{
//	CVector3f m_vMinimum;
//	CVector3f m_vMaximum;
//};

namespace sqr
{
	class CPluginMesh;
}

class CPluginPiece;
class CPluginPieceGroup : public CEditPieceGroup
{
public:
	CPluginPieceGroup()
	{
		m_Piece.resize(1);
	}
	~CPluginPieceGroup()				{ Free(); }

	//vector<CPluginPiece>			m_Piece;
	DWORD					m_Version;
	CPluginPieceGroup*			m_OldPieceGroup;

	void GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
	CPluginPiece* GetOldPieceByName(string pieceName);
	void AddPiece( CPluginPiece* Piece );
	void Free()						
	{ 
		m_OldPieceGroup = NULL;
	}
	void SetPieceGroup(CPluginPieceGroup* oldpiecegroup){m_OldPieceGroup = oldpiecegroup;}
	int  GetPieceNum()				{ return (int)m_Piece[0].size(); }	
	
	int PluginBinSave( ofstream& File, string SaveName );
	int PluginBinLoad( string FileName );

	using CPieceGroup::m_Piece;

	CPluginPiece* GetPiece( size_t nClass, size_t nPiece );
	size_t	GetPieceTotalNum();

	CPluginPiece* 
			GetPieceByName( const string& pieceName );
};

//PIECE
//class CPluginPieceGroup;
class CPluginPiece : public CEditPiece
{
public:
	//string						m_sPieceName;				//Piece名字
	//string						m_ClassName;		//Piece种类名
    CPluginMesh*				m_pPluginMesh;				//Piece的MESH
	//vector<CPluginRenderStyle>  m_RenderStyle;		//Piece的渲染类型
	CPluginPiece*				m_oldpiece;

	using CEditPiece::m_sPieceName;
	using CEditPiece::m_RenderStyle;

public:
	CPluginPiece( CPluginPieceGroup* pGroup )
		: CEditPiece(pGroup)
	{
	}

	~CPluginPiece() 
	{
		//m_RenderStyle.clear();
		m_oldpiece = NULL;
	}
    
	GString&	GetPieceName() { return m_sPieceName; }
	bool	GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
	void	SetPluginMesh( CPluginMesh* pMesh );			
	void	SetMesh( CMesh* pMesh );
	void	AddRenderStyle( CPluginRenderStyle* pRenderStyle )	{ m_RenderStyle.push_back( pRenderStyle );};

	CPluginRenderStyle* 
			GetPluginRenderStyle( size_t i );

    CPluginMesh*  GetMash()       { return m_pPluginMesh; }
	void	SetPiece(CPluginPiece* piece){m_oldpiece = piece;}
	//int ASCSave( CSaxXmlWriter& Node );
	int BinSave( ofstream& File, string SaveName, vector<CapsuleNode> & CapsuleNodes );
	int BinLoad( ifstream& File, const string LoadName,DWORD Version);
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

class CPluginModel
{
	CPluginPieceGroup				m_PieceGroup;           //网格
	CPluginPieceGroup				m_PieceGroupOld;		//需要保留的信息
	ExportNode*				m_Root;				//根节点
	CAnimateGroup			m_Motions;			//来自MAX
	CAnimateGroup			m_AnimateGroup;		//来自文件
	BOOL					m_IsExpAllFrame;	

	string					m_AniName;
public:
	
	CPluginModel(void);
	~CPluginModel(void);
	void Free();
	void SetGlobeIsExist(bool IsExist);
	void GetOldModInfoFromFile(string FileName);
	void ClearModData();
	bool IsMatching() 
	{
		return (m_PieceGroup.GetPieceTotalNum() == m_PieceGroupOld.GetPieceTotalNum())?TRUE:FALSE;
	}

	BOOL CheckTrouser();

	BOOL SkeMatching();

	int Save(string FileName);
	int SaveAgp(string FileName);
	int SaveFromFile(string FileName);
	int SaveFrom3DMax(string FileName);

	void SynAniname();

private:

	ExportNode*	NodeEnum( INode* MaxNode, ExportNode* Parent );

public:
	CPluginPieceGroup& GetCPieceGroup();
	int			FillFrom3DMax( const SAVEPARAM& SaveParam );
	int			FillFromFile( string FileName,string& aniname );
	bool		LoadSkeFromFile(string FileName);
	int			FillAgpFromFile(string FileName);
	string&		GetIndxNameFromAgp();
	bool		IsFit(CString FileName);
	void		FixParent( ExportNode* Root );
	void		SetExpFrame(BOOL IsExpAllFrame);
	void		SetIndxSkeName(string& name);
};




#endif
