#ifndef _CMesh_Level_H_
#define _CMesh_Level_H_
//#include "CSimCloth.h"
#include "CRenderOperation.h"
#include "CBufFile.h"

namespace sqr
{
class CMeshLevel
{
public:
	CMeshLevel() {}
	virtual ~CMeshLevel();

	inline float	GetDist()
	{
		return m_Dist;
	};

	inline void		SetDist( float dist )
	{
		m_Dist = dist;
	}

	inline uint		GetFaceNum()
	{
		return (uint(m_IndexBuf.indexCount - m_IndexBuf.indexStart)) / 3;
	}
	IndexData*		GetIndexData()
	{
		return &m_IndexBuf;
	}
	int				BinLoad(  CBufFile& QuickFile , size_t IBSize );//, SaveInfo& SI ,int m_Channel = 0);

	void			_FreeRes(void);
	void			_Reclaim(void);
protected:
	IndexData			m_IndexBuf;
	float				m_Dist;
};

/*
	class CStaticLevel : public CMeshLevel
	{
	protected:
		float			m_Dist;		//标志
		void			ReadIndex( CBufFile& GraphicFile );
		GVector<uint16>	 m_IndexBuf;
	public:
		CStaticLevel();
		virtual ~CStaticLevel();
		float			GetDist(){ return m_Dist; };
		int				GetFaceNum()	{ return m_IndexBuf.size()/3; }
		void*			GetIndexBuf()	{ return &m_IndexBuf[0];	}
		//RenderParam*	GetFilterNode() { return &m_RenderNode.m_RP; }
	};

	class CSkeletalLevel : public CStaticLevel
	{

	private:
		BOOL  IsAddface( INT32& index, GMap<int32, int32>& loadbonelist,
			GMap<int32, int32>& bonelist,
			GVector<VertexSkeletalFormat>& SrcVertexBuf,
			GVector<int32>& mindexinfo,
			GMap<int32, int32>& mvector,
			GMap<int32, int32>& mallvector,
			GMap<int32, int32>& mSrcId );
		static BOOL  IsLoadBone( GMap<int32, int32>& loadbonelist, INT32& boneid );
		static int32 FindBone( GMap<int32, int32>& loadbonelist, INT32& boneid );
		static int32 FindBoneIndex( GMap<int32, int32>& loadbonelist, INT32& boneid );

	public:
		CSkeletalLevel();
		~CSkeletalLevel();

		int			GetVerNum(){ return m_VertexBuf.size(); }
		void		SetSkeletalMask( SKELETAL_MASK& Mask );
		int			BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel = 0 );
		void*		GetVertexBuf() { return &m_VertexBuf[0]; }
		bool Render(DX9Graphic* pCraphic,CPieceRenderStyle* CurRS, uint8 SkeletalNum, CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser);
		RenderParam* GetFilterNode() { return &m_RenderNode.m_RP; }
	};

	class CVertexStaticLevel : public CStaticLevel
	{
		struct VertexFomat
		{
			float		x,y,z;
			uint16		n;
			uint16		u,v;
			uint16		u1,v1;
			uint16		u2,v2;
		};
	public:
		int			BinLoad(  CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel = 0 );
		bool		Render(DX9Graphic* pCraphic, CPieceRenderStyle* CurRS, uint8 SkeletalNum,
			CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser);
	};

	class CVertexAniLevel : public CStaticLevel
	{
	protected:
		struct VertexFomat{	int16 x,y,z; uint16 n; };
		struct VertexFomatEx : public VertexFomat
		{
			uint16		u, v ;
			uint16		u1,v1;
			uint16		u2,v2;
		};
		struct TextCoord { uint16 u,v; };

		struct LevelAni
		{
			IDNAME							m_AnimationeName;	//动画名
			uint32							m_FramePerKeyFrame;	//顶点动画采样率
			GVector< GVector<VertexFomat> >	m_FrameVertex;;
			GVector<VertexHardwareBuffer*>	m_FrameBuffer;
		};

	public:
		typedef GVector<LevelAni> AniVector;
		struct VertexAniParams
		{
			float			Weight;
			float			InvWeight;
			AniVector*		pAniVector;
			CRenderPiece*	pRenderPiece;
			uint8			iPiece;
		};
	protected:
		AniVector							m_MeshFrame;
	virtual FilterNode*	 GetFilterNodeEx() { return NULL; }

		VertexHardwareBuffer*				m_TextBuffer;
		VertexHardwareBuffer*				m_TextBuffer1;
		VertexHardwareBuffer*				m_TextBuffer2;

	public:
		~CVertexAniLevel();
		int			GetVerNum(){ return m_MeshFrame[0].m_FrameVertex[0].size(); }
		D3DXVECTOR3 GetVertex( int nNum ){ return D3DXVECTOR3( m_MeshFrame[0].m_FrameVertex[0][nNum].x, m_MeshFrame[0].m_FrameVertex[0][nNum].y, m_MeshFrame[0].m_FrameVertex[0][nNum].z ); }
		int			GetAniNum(){ return m_MeshFrame.size(); }
		int			GetAniFrameNum( int Ani ){ return m_MeshFrame[Ani].m_FramePerKeyFrame*(m_MeshFrame[Ani].m_FrameBuffer.size()-1); }
		IDNAME		GetAniName( int Ani ){ return m_MeshFrame[Ani].m_AnimationeName; }
		bool		Render(DX9Graphic* pCraphic, CPieceRenderStyle* CurRS, uint8 SkeletalNum,
			CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser);
		int			BinLoad(  CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel = 0);

	};


	//////////////////////////////////////////////////////////////////////////
	//柔体结构

	/////////////////////////////////////////////////
	// class CBallNode
	/////////////////////////////////////////////////


	enum ESoftType
	{
		SoftPlane,    //平面状柔体:绳子 ＋ 一片
		SoftCross,    //交叉状柔体：绳子 ＋ 两片
	};

	struct NodePhyPara
	{
		float      m_fAccGravity;     //重力加速度（牛顿/千克）
		float      m_fMass;           //每个质点的质量
		float      m_fCoeDamp;        //阻力系数
	};

	struct CVerNode
	{
		float			  m_fTU1;			 //纹理横坐标
		float			  m_fTV1;			 //纹理纵坐标
		float			  m_fTU2;			 //纹理横坐标
		float			  m_fTV2;			 //纹理纵坐标
		float			  m_fWidth;			 //节点宽
		float			  m_fNatualLen;		 //自然长度
	};

	struct CBallNode
	{
		CVector3f		  m_vPos;			 //顶点位置,
		CVector3f		  m_vNormal;		 //顶点法线,
		CVector3f         m_vForce;          //顶点所受的力
		CVector3f         m_vAccel;          //顶点所受的加速度
		CVector3f         m_vSpeed;          //顶点速度

		void Init()
		{
			m_vPos		= CVector3f( 0, 0, 0 );
			m_vNormal	= CVector3f( 0, 0, -1 );
			m_vForce	= m_vAccel = m_vSpeed = CVector3f( 0, 0, 0 );
		}
	};

public:
	int			BinLoad(  CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel = 0 );
	bool		Render(DX9Graphic* pCraphic, CPieceRenderStyle* CurRS, uint8 SkeletalNum,
		CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser);
	FilterNode* GetFilterNodeEx(){ return &m_RenderNode; }
};


	struct SpringInfo
	{
		float         m_fRatioCompress;        //弹簧缩短比率
		float         m_fRatioPull;            //弹簧拉长比率
		float         m_fCoe;
	};

	//碰撞物体结构
	struct CollideStatic
	{
		CollideStatic(){};
		CollideStatic(BYTE nID):m_nBnID(nID){};
		CollideStatic(BYTE nID, float fRadius):m_nBnID(nID), m_fRadius(fRadius){};

		BYTE      m_nBnID;  //要进行碰撞检测的骨骼ID
		float     m_fRadius;//半径
	};

	//碰撞物体结构
	struct CollideDynamic
	{
		CVector3f m_vBegin;
		CVector3f m_vEnd;
	};

	struct CDynamicSoft
	{
		CDynamicSoft() : m_nPreTime(0),m_nNextTime(0){}
		uint32					m_nPreTime;
		uint32					m_nNextTime;
		CVector3f				m_vecWind;
		CVector3f				m_NexWind;
		GVector<CBallNode>		m_arrSpNode;			//存储结点的数组
		GVector<CollideDynamic>	m_arrCollideDynamic;	//碰撞检测物体数组
	};

	class CSoftLevel : public CMeshLevel
	{
		struct TopVer
		{
			CVector3f	p;
			float		f[4];
			BYTE		b[4];
		};


		NodePhyPara				m_nodePhyPara;			//结点物理参数
		SpringInfo				m_spInfo;				//弹簧参数
		ESoftType				m_eSoftType;			//柔体类型
		GVector<CVerNode>		m_arrVertex;			//存储顶点点的数组
		GVector<CollideStatic>	m_arrCollideStatic;		//碰撞检测物体数组
		BYTE					m_bySkeletalID;			//连接的骨骼
		TopVer					m_vecTop[2];

	public:
		CSoftLevel();
		int				GetVerNum()				{ return (int)m_arrVertex.size()*2; }
		int				GetFaceNum()			{ return (int)m_arrVertex.size()*2 - 2; }

		int				GetCollisionCount()		{ return m_arrCollideStatic.size(); }
		CollideStatic&	GetCollision( int n )	{ return m_arrCollideStatic[n]; }
		void			InitDynData( CDynamicSoft& Data );

		static CVector3f s_vecMin;
		static CVector3f s_vecMax;

		static void SetWind( CVector3f& vecMax, CVector3f& vecMin );
		static void UpdateWind( CDynamicSoft& DS, uint32 nDelTime );

		void		SetSkeletalMask( SKELETAL_MASK& Mask );
		const void* CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format,
			uint32 stride, CRenderMatrix& ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const;

		int BinLoad( CBufFile& GraphicFile, SaveInfo& SI  ,int m_Channel = 0);

	private:
		//操作函数
		void  ResetData( CDynamicSoft& DS, uint32 nDelTime ) const;												 //初始化结点和弹簧的值
		void  UpdateFrame( CRenderMatrix& ModelMatrix, CVector3f& P1, CVector3f& P2, CDynamicSoft& DS  ) const;  //更新每帧数据
		void  UpdateNormal( CDynamicSoft& DS ) const;															 //更新顶点法线
		void  CalCollideData( CRenderMatrix& ModelMatrix, CDynamicSoft& DS  ) const;
	};

	//////////////////////////////////////////////////////////////////////////
	// 柔布(披风)结构

	struct SoftClothProp
	{
		uint8   uWidth;
		uint8   uHeight;
		uint16  uMassA;
		uint16  uMassB;
		uint16  uMassC;
		uint16  uWindMin;
		uint16  uWindMax;
		uint16  uWindMaxH;
		float   fGravity;
		float   fGassForce;
		float   fCoefficientK;
		float   fLongRatio;
		float   fShortRatio;
		float   fTimePass;
	};

	////////////////////////////////////////////////////////////////
	// 披风碰撞胶囊结构
	class CSoftClothLevel;

	struct CapsuleNode
	{
		CVector3f   Begin;
		CVector3f   End;
		float       Raidus;
		CTinyWeight w;
		BYTE        b[4];
	};

	class CSoftClothLevel : public CMeshLevel
	{
		friend struct CDynamicSoftCloth;

		struct VertexFomat
		{
			VertexFomat( CVector3f& vnt, CTinyWeight &wi, BYTE* bi)
				:VertexT(vnt), w(wi)
			{
				*((uint32*)b)   = *((uint32*)bi);
			}

			CVector3f  VertexT;
			CTinyWeight w;
			BYTE        b[4];
		};

		struct CollideCapsule
		{
			CVector3f m_Begin;
			CVector3f m_End;
			float     m_Radius;
			CTinyWeight BoneWeight;
			BYTE      BoneIndex[4];

			CollideCapsule():m_Begin(0),m_End(0),m_Radius(0){}
			CollideCapsule( CVector3f & b, CVector3f & e, float r, CTinyWeight &wi, BYTE bi[4])
				:m_Begin(b),m_End(e),m_Radius(r),BoneWeight(wi)
			{
				*(uint32*)BoneIndex = *(uint32*)bi;
			}
		};

		GVector< CollideCapsule >                 m_Collides;

		GVector< CSoftClothLevel::VertexFomat >   m_TopFixVerBuf;

		GVector<float>			m_HSprings;
		GVector<float>			m_VSprings;

		GVector<uint16>			m_TextureU;
		GVector<uint16>			m_TextureV;

		int                     m_VertexNum;
		GVector< uint16 >	    m_IndexBuf;

		uint8   m_ClothWidth;
		uint8   m_ClothHeight;

		SoftClothProp           m_ClothProp;

	public:
		CSoftClothLevel();
		~CSoftClothLevel();

		int     GetVerNum()     { return m_VertexNum;  }
		int		GetFaceNum()	{ return m_IndexBuf.size()/3; }
		void*	GetIndexBuf()	{ return &m_IndexBuf[0]; }

		void    Update( CRenderMatrix & RenderMatrix,  CRenderPiece* pRenderPiece, int32 iPiece ) const;
		void    UpdateClothNormal( CDynamicSoftCloth& DS , void* DestBuf ) const;
		const void* CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format,
			uint32 stride, CRenderMatrix& ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const;

		void	SetSkeletalMask( SKELETAL_MASK& Mask );
		void	InitDynData( CDynamicSoftCloth& Data );
		int     BinLoad( CBufFile& GraphicFile, SaveInfo& SI  ,int m_Channel = 0);
	};

	struct CDynamicSoftCloth
	{
		struct VertexFomat
		{
			VertexFomat( CVector3f& vnt, CTinyWeight &wi, BYTE* bi)
				:VertexT(vnt), w(wi)
			{
				*((uint32*)b)   = *((uint32*)bi);
			}

			CVector3f   VertexT;
			CTinyWeight w;
			BYTE        b[4];
		};

		struct Capsule
		{
			CVector3f m_Begin;
			CVector3f m_End;

			Capsule(){}
			Capsule( CVector3f & b, CVector3f & e ):m_Begin(b),m_End(e){ }
		};

		CDynamicSoftCloth():m_VertexBuf(NULL),m_Cloth(NULL),m_upLevel(NULL),m_VertexNum(0),m_Width(0),m_Height(0){}

		CDynamicSoftCloth( const CDynamicSoftCloth& b);


		~CDynamicSoftCloth()
		{
			if( m_VertexBuf ) delete[] m_VertexBuf;
			if( m_Cloth )     delete m_Cloth;
		}

		GVector< VertexFomat >   m_TopFixVerBuf;
		CVector3f *				m_VertexBuf;

		Cloth::CCloth *         m_Cloth;
		int16                   m_VertexNum;
		uint8                   m_Width;
		uint8                   m_Height;

		GVector< Capsule >       m_Capsules;
		CSoftClothLevel*		m_upLevel;
		// CVector3f				m_NormalAVG;

	};
	*/
}
#endif