#pragma once
#include "Exp.h"
#include "GraphicBase.h"
#include "GraphicCommon.h"
#include "CAxisAlignedBox.h"
#include "CMesh.h"
#include "CMeshLevel.h"
using namespace std;

namespace sqr
{
	class CPluginMesh;

	struct CPluginMeshLevel : public CMeshLevel
	{
		CPluginMeshLevel()
		{
		}

		~CPluginMeshLevel()
		{
			m_IndexBuf.Destroy();
		}

		void	CreateIndexBuffer( size_t nIndexNum );
		void	SetIndex( size_t nIndex, uint16 vertIndex );
		uint16	GetIndex( size_t nIndex );
		uint32	GetIndexNum();
	}; 

	class CPluginMesh
	{	
	public:
		CVector2f				m_MinUV;
		CVector2f				m_MaxUV;

		string					m_ObjectName;					//Mesn名字
		bool					m_tIsCompress;					

		ExportNode*				m_ExpNode;
		CPluginMesh*					m_oldMesh;

		DWORD							m_FramePerKeyFrame;	//顶点动画采样率

		vector< vector<CVector3f> >		m_FramesVertex;		//多帧顶点缓冲区
		vector< vector<CVector3f> >		m_FramesNormal;		//多帧法线缓冲区
		vector<CVector2f>				m_TextCoord;		//贴图坐标
		vector<CVector2f>				m_TextCoord1;
		vector<CVector2f>				m_TextCoord2;
		vector<CWeight>					m_Weight;			//顶点骨骼权重
		vector<SkltIndex>				m_SkeletalIndex;	//顶点骨骼索引

	public:
		typedef	map<uint16, CBaseAABB>					BoxMap;
		BoxMap					m_BoxMap;						//包围盒列表

		CPluginMesh();
		virtual ~CPluginMesh();

		void	SetMeshName(string name) 
		{
			m_ObjectName = name;
		}

		void	SetOldMesh(CPluginMesh* mesh)
		{
			m_oldMesh = mesh;
		}

		void	SetIsCompress(bool bIsCompress) 
		{ 
			m_tIsCompress = bIsCompress; 
		}

		void	asynNewCompressState()	
		{
			GetIsCompress() = m_tIsCompress ;
		}

		void	asynOldCompressStete()	
		{
			m_tIsCompress = GetIsCompress();
		}

		ExportNode* GetExpNode() 
		{ 
			return m_ExpNode; 
		}

		void	SetExpNode( ExportNode* ExpNode )
		{ 
			m_ExpNode = ExpNode; 
		}

		bool	GetMeshFromExpNode( CAnimateGroup& AnimateGroup );
		void	GetPhysiqueWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
		void	GetSkinWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
		void	GetBPWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup );
		void	FormatFromMaxMesh( Mesh& MaxMesh, vector<VerSkinNCT>& Vertex, vector<WORD>& VertexIndex );

		//int ASCSave( CSaxXmlWriter& Node );
		int BinLoad( ifstream& File ,DWORD Version);
		int BinSave( ofstream& File, vector<CapsuleNode> & CapsuleNodes );

		typedef vector<CWeight>		SklWeightList;
		typedef vector<SkltIndex>	SklIndexList;
		typedef vector<VerSkinNCT>	VerSkinNCTList;

		

		virtual CPluginMeshLevel* 
				GetPluginMeshLevel(size_t level) = 0;

		virtual size_t 
				GetPluginMeshLevelNum() = 0;

	protected:
		void	getMeshSmoothInfo( const Mesh& mesh, vector<BOOL>& norSmoothList );

		bool	generateBaseInfo( CPluginMeshLevel* pMeshLevel, CAnimateGroup& animGroup );

		bool	generateVertexAniInfo( CPluginMeshLevel* pMeshLevel, CAnimateGroup& animGroup );

		bool	generateFrameVertList( VerSkinNCTList& verList, TimeValue time, CAnimateGroup& animGroup );

		bool	generateUV( Mesh& mesh, VerSkinNCTList& verList, const CMatrix& matWorld );

		void	generateSkeletalInfo( 
					SklWeightList&	sklWeightList, 
					SklIndexList&	sklIndexList, 
					INode*			pMaxNode, 
					TimeValue		time,
					CAnimateGroup&	animGroup 
					);

		void	optimizeUV( CPluginMeshLevel* pMeshLevel );



		virtual CPluginMeshLevel* 
				createMeshLevel() = 0;

		virtual void
				setBufVertexNum( int level, int vertNum ) {}

		virtual void
				generatePosBuffer( int level, const vector<CVector3f>& posBuf ) {}

		virtual void 
				generateNormalBuffer( int level, const vector<CVector3f>& normBuf ) {}

		virtual void
				generateTexCoordBuffer0( int level, const vector<CVector2f>& texBuf0 ) {}

		virtual void
				generateTexCoordBuffer1( int level, const vector<CVector2f>& texBuf1 ) {}

		virtual void 
				generateTexCoordBuffer2( int level, const vector<CVector2f>& texBuf2 ) {}

		virtual void
				generateSkeWeightBuffer( int level, const vector<CWeight>& skeWeightBuf ) {}

		virtual void
				generateSkeIndexBuffer( int level, const vector<SkltIndex>& skeIndexBuf ) {}

	// temp code
	public:
		virtual VERTEXTYPE&
				GetMeshType() = 0;

		virtual float&
				GetBaize() = 0;

		virtual CVector3f&
				GetScale() = 0;

		virtual CVector3f&
				GetOff() = 0;

		virtual CVector3f&
				GetOrgScale() = 0;

		virtual CVector3f&
				GetOrgOff() = 0;

		virtual float& 
				GetTexScale() = 0;

		virtual int&
				GetChannel() = 0;

		virtual bool&
				GetIsCompress() = 0;
	};
}