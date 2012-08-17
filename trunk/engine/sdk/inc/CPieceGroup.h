#pragma once
#include "CMesh.h"
#include "CAxisAlignedBox.h"
#include "CRenderable.h"
#include "CSkeletalMask.h"
#include "CFourCC.h"
#include "CGeometryInstance.h"
#include "CResUnitGroup.h"
#include "CCoder.h"
//#define		StoneLight_MainOne		1001
//#define		StoneLight_MainTwo		1002
//#define		CartoonMain				1003

namespace sqr
{
	//class ITexture;
	//struct CRenderPiece;

	#define GETVERSION(VER)	0x0000ffff&VER
	#define GETCP(VER)  (0x00010000&VER)>>16
	#define GETBOX(VER)	 (0x00100000&VER)>>20
	#define ENABLECP(VER)		0x00010000|VER
	#define ENABLEBOX(VER)		0x00100000|VER
	//渲染类型
	class CPieceRenderStyle : public CGraphicMallocObject
	{
		friend class CPiece;
	protected:
		GString				m_RenderStyleName;			//渲染类型
		GString				m_RSIndexName;
		DWORD					m_Version;
		CRenderStyle*			m_RenderStyle;
		GVector<ITexture*>		m_Texture;
		GVector<GString>		m_TextureName;

	public:
		CPieceRenderStyle();
		~CPieceRenderStyle();

		bool	IsValid();
		bool	IsAlphaBlend( float Frame )
		{
			return m_RenderStyle ? m_RenderStyle->IsAlphaBlend( Frame ) : FALSE;
		}
		void	GetCurRS( STATIC_RS& CurRS, uint32 FrameTime ,uint32 CurTime, void* pUser);
		int		BinLoad( CBufFile& File );

		void	SetVersion(DWORD ver){m_Version = ver;}

		DWORD	GetVersion(){return m_Version;}
		const char*	GetRSIndexName()
		{
			return m_RSIndexName.c_str();
		}

		const char*	GetRSName()
		{
			return m_RenderStyleName.c_str();
		}
		int BinSave( WriteDataInf& wdi );

		CRenderStyle*	GetRenderStyle(void)			{ return m_RenderStyle;	}
		int				GetTextureNum()					{ return (int)m_Texture.size(); }
		ITexture*		GetTexture( int Num )			{ return Num < (int)m_Texture.size() ? m_Texture[Num] : NULL; }
		void			FreeTexture();

	protected:
		virtual HRESULT _CreateRenderStyle( const char* szName, CRenderStyle** pRS );
		virtual HRESULT	_CreateStyleTexture( const TCHAR* szTexName );
				void	_FreeRes(void);
				void	_Reclaim(void);
	};

	struct PortraitInfo : public CGraphicMallocObject
	{
		static const FourCC PICC;//('PTIF');

		PortraitInfo()
			: translate(0.0f, 0.0f, 0.0f), cameraPos(0,50,-200)
			, fov(CMath::ToRadian(30)) {}
		CVector3f translate;
		CVector3f cameraPos;
		float fov;
	};
	//struct CPieceClass
	//{
	//	GVector< CPiece* >			m_Pieces;
	//};
	typedef GVector< CPiece* > CPieceClass;

	//Piece群组
	class CPieceGroup : public CResUnitGroup
	{
		friend class CPieceGroupCoder;
		friend class CDataSources;
		HAS_FILE_FORMAT();	

	public:
		CPieceGroup( const string& szName = "", const wstring& Alias = L"" );
	protected:
		GVector< CPieceClass >		m_Piece;
		GMap< GString, int >			m_PieceClassIndex;
		GVector< IDNAME >			m_PieceAni;
		PortraitInfo*				m_pPortraitInfo;


		friend class				CGraphic;
		virtual void				OnRead( CRefBufFile& GraphicFile );
		void						ReadPortraitInfo( CBufFile& GraphicFile );
	

	public:
		virtual						~CPieceGroup();
		virtual bool				IsValid();
		virtual size_t				GetPieceClassNum()
		{
			return m_Piece.size();
		}
		virtual const char*		GetPieceClassName( int Num );
		virtual int				GetPieceClassAniNum();
		virtual const char*		GetPieceClassAniName( int Ani );
		const GVector<CPiece*>&		GetPieceClass( const string& PieceClassName );
		//const GVector<CPieceClass>&	GetAllPieceClass()
		//{
		//	return m_Piece;
		//}
		int							BinLoad( void );

		//virtual void				CalcuPieceInfo(int &facenum, int &vertexnum);
		virtual PortraitInfo*		GetPortraitInfo();
		
	protected:
		void	_FreeRes(void);
		void	_Reclaim(void);
	public:
		friend class CModFormat;
		static CModFormat	Format;
		bool	m_bOnRead;
	};


	//PIECE
	class CPiece : public CResUintNode
	{
	protected:
		//CPieceGroup*				m_pGroup;
		GString						m_ClassName;		//Piece种类名
		GString						m_sPieceName;		//Piece名
		CMesh*						m_Mesh;				//Piece的MESH
		GVector<CPieceRenderStyle*>	m_RenderStyle;		//Piece的渲染类型
		SKELETAL_MASK				m_SkeletalMask;
		FilterNode					m_RenderNode;
		CAxisAlignedBox				m_AABox;						//CMesh的包围盒
		DWORD						m_Version;

	public:
		DWORD						m_ShaderType;
		GMap<GString,uint32>			m_RSIndxToID;
		CPiece( CPieceGroup* pGroup ):CResUintNode( pGroup ), m_Mesh( NULL )
		{
			m_RenderNode.pAABox = &m_AABox;
			m_RenderNode.Destroy();
		};
		virtual ~CPiece()
		{
			delete m_Mesh;
			for ( int i = 0; i < (int)m_RenderStyle.size(); i++ )
				delete m_RenderStyle[i];
			m_RenderNode.vertexData = NULL;
			m_RenderNode.indexData = NULL;		
		}
		void SetVersion(DWORD ver){m_Version = ver;}
		inline CPieceGroup*	GetPieceGroup(void)
		{
			return (CPieceGroup*)m_pGroup;
		}

		inline const char* GetPieceClassName()
		{
			return m_ClassName.c_str();    //返回类名
		}
		inline const char* GetPieceName()
		{
			return m_sPieceName.c_str();    //返回Piece名
		}
		inline uint32  GetRSNum()
		{
			return (uint32)m_RenderStyle.size();    //返回渲染类型数量
		}

		inline CPieceRenderStyle* GetRS( uint32 Num )
		{
			return Num < GetRSNum() ? m_RenderStyle[Num] : NULL;    //得到指定渲染类型
		}

		inline CMesh* GetMesh()
		{
			return m_Mesh;
		}

		inline const SKELETAL_MASK& GetSkeletalMask()
		{
			return m_SkeletalMask;
		}

		inline float GetZBias()
		{
			return m_Mesh->GetZBias();
		}

		inline CAxisAlignedBox& GetAABox(void)
		{
			return m_AABox;
		}

		bool	IsValid();

		void	_FreeRes(void);
		void	_Reclaim(void);
		int BinLoad( CBufFile& GraphicFile ,DWORD Version);
		int Render( uint32 SelfRSNum, float Dist, CRenderMatrix& ModelMatrix,
			RenderObjStyle* pRORS, CRenderPiece* pRenderPiece, int32 iPiece,const PieceModify& Modify );
		//void	ResetPiece(void);
	protected:
		virtual void _SetPieceName( const string& strPieceName );
		virtual CPieceRenderStyle* _GetRenderStyle(uint32 Num);
	};

}

