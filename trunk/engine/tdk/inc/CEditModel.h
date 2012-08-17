#pragma once
#include "CModel.h"
#include "CEditorClassPre.h"
#include "float.h"
#include "CEditSubject.h"
#include "CRenderModel.h"

namespace sqr_tools
{
	struct PickSkeletalParam
	{
		PickSkeletalParam(){};
		PickSkeletalParam( int x, int y, int w, int h )
		{
			m_SkeletalID = 0;
			m_Z = FLT_MAX;
			m_CurPos.x = x;
			m_CurPos.y = y;
			m_Size.x = w;
			m_Size.y = h;
		};
		BYTE          m_SkeletalID;
		float         m_Z;
		POINT         m_CurPos;
		POINT         m_Size;
		string        m_SkeletalName;
	};

	struct PickSocketParam
	{
		PickSocketParam(){};
		PickSocketParam( int x, int y, int w, int h )
		{
			m_SkeletalID = 0;
			m_Dist = FLT_MAX;
			m_CurPos.x = x;
			m_CurPos.y = y;
			m_Size.x = w;
			m_Size.y = h;
		};
		BYTE          m_SkeletalID;
		BYTE		  m_SocketNum;
		BYTE		  m_Coordinate;
		float		  m_Dist;
		POINT         m_CurPos;
		POINT         m_Size;
	};

	/// 骨骼mesh
	struct SkeletalPos
	{
		SkeletalPos():m_IsSelected(FALSE){};
		CVector3f	m_Scale;
		BOOL				m_IsSelected;  //骨骼是否被选中

		//sqr::VerColor3D			m_SklPos[6];		//生成的骨骼MESH
	};

	class CEditModel 
		: public sqr::CRenderModel
		, public CEditSubject 
	{
	public:
		static const DWORD  st_EditModelType;
		CEditModel(ILogicHandler* pHandler = NULL);
		~CEditModel();

		bool BinSave( const string& str );
		void SetSelectedPiece( const string& str );
		void AddMeshPieceEx( CEditPiece* pEpc, uint32 RSNum = 0 );

		void GetEditPieceGroup(CEditPieceGroup* pPieceGroup);
		void DeletePiece(CEditPiece* pEpc);

		IEffect*						GetFirstEffect();
		int								GetCurMaxFrame(void);
		const GMap<WORD,sqr::IDNAME>*	GetCurFrameString();
		int								GetCurSelSkelatal();	/// 得到当前所选骨骼ID
		int								GetSkeletalIdBySocketName( sqr::IDNAME SocketName, int* SocketNum = NULL ); /// 通过SOCKET名字得到骨骼ID
		
		/// 导出关键帧字符串
		bool ExportFrameKey( const string& szFileName );
		/// 导入关键帧字符串
		void ImportFrameKey( const string& szFileName );
		/// 插入关键政字符串
		void InsertFrameString( const string& str, int FrameNum );

		/// 点选顶点
		void PickVertex( PickSkeletalParam& Pick );
		/// 点选骨骼
		int PickSkeletal( PickSkeletalParam& Pick );
		/// 点选SOCKET
		int PickSocket( PickSocketParam& Pick );
		///// 编辑SOCKET
		//int EditSocket( PickSocketParam& Pick, VIEWFLAG ViewFlag );

		int SaveJointVerNormal( const string& szFileName );
		int LoadJointVerNormal( const string& szFileName );
		float	GetCurFrameRatio(void);

		//bool	SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop = true, int32 DelayTime = 200 /*毫秒*/, float AniSpeed = -1.0f );
		const	AniResult RenderFrame( const CMatrix& matWorld, float uCurFrame, RenderObjStyle* pRORS = NULL ) ;
		float	GetCurFrameNum();
		PortraitInfo*	 GetPortraitInfo(void) { return &m_PortraitInfo; }

		void			RenderBone(void);
		void			RenderBox(CBaseAABB* pCurBox = NULL,bool bStaticBox = false);
		void			RefreshBox(void);
		void			DelBox(CBaseAABB* pBox);
		void			AddBox(uint16 ID,CBaseAABB* pBox);
		void		    SetSkesMask(const SKELETAL_MASK& mask);

		int				CalSkeletalSize(int nDelta);
		int				CalAniSize(const CEditSkeletal& Skeletal, CEditAnimate& Animate,float Size);
		
		RenderPieceMap& GetRenderPieces(void) { return m_MeshsPiece; }
		void			SetForceRenderStyle(CEditRenderStyle* pStyle);
		bool			IsFindKeyAniName(string& aniname);

		CEditPiece*		GetSelectPiece(){return m_pSelectPiece;}

		CEditPieceGroup* GeneratePieceGroupForSave();
		void			RemoveFrmParent(void);
		virtual void	ClearModelRender(void);

		void			ClearMeshPiece();

		CVector3f		GetModelScale() const;
		CVector3f		GetModelRotate() const;
		//add by guo
		virtual void	SetFrame(float uCurFrame);
		virtual float   GetCurFrame(void);
		virtual std::string GetAnimResName(void);
		virtual void SetAnimResName(std::string name);

		bool			AddPieceGroup( const char* szFileName );
		bool			AddAnimationGroup( const char* szFileName );
		size_t			GetChildCount();
		string			GetModelName();
		string			GetModelName(const size_t index);
		string			GetAnimationGroupName();
		string			GetAnimationGroupName(const size_t index);
		string			GetEffectName(string& effectFileName);
		string			GetEffectName(const size_t index, string& effectFileName);

		void			SetEditAnimateGroup( CAnimationGroup* AnimateGroup );

		bool			IsAnimate(void);

	protected:
		virtual void	SetPortraitInfo( const PortraitInfo& pi );
		//virtual void	pieceSelected( const CEditPiece* piece );
		
		virtual int		RenderMesh(  RenderObjStyle* pRORS );

	protected:
		bool DecideAllIsSet();
		PortraitInfo		m_PortraitInfo;
		CEditPiece*			m_pSelectPiece;
		float				m_Size;
		SkeletalPos*		m_SkeletalPos;
		CEditRenderStyle*	m_pForceStyle;
		GVector<string>		m_KeyFilter;

		//////////////////////////////////////////////////////////////////////////
		//以下属性是地编中用到的，先暂时放这儿，待有合适的地方再处理
	protected:
		string				m_strModelName;
		string				m_strAnimationName;
		bool				m_bPlayAnimation;		///on / off animation		
		bool				m_bShadow;				///是否有影子	
		bool				m_bReceiveShadow;		///写是否接收阴影信息		
		bool				m_bVisibleByConfigure;	//模型可见是否依赖配置值
		bool				m_bUseCameraCollision;	///物件增加摄像机碰撞标记（有些物体不想使用消隐，而是想使用摄像机碰撞）

	public:
		BYTE GetRenderStyleIndex();

		void SetIsPlayAnimation(const bool b)
		{
			m_bPlayAnimation = b;
		}
		bool GetIsPlayAnimation() const
		{
			return m_bPlayAnimation;
		}

		void SetIsCastShadow(const bool b)
		{
			m_bShadow = b;
		}
		bool GetIsCastShadow() const
		{
			return m_bShadow;
		}

		void SetIsReceiveShadow(const bool b)
		{
			m_bReceiveShadow = b;
		}
		bool GetIsReceiveShadow() const
		{
			return m_bReceiveShadow;
		}

		void SetIsVisibleByConfigure(const bool b)
		{
			m_bVisibleByConfigure = b;
		}
		bool GetIsVisibleByConfigure() const
		{
			return m_bVisibleByConfigure;
		}

		void SetIsUseCameraCollision(const bool b)
		{
			m_bUseCameraCollision = b;
		}
		bool GetIsUseCameraCollision() const
		{
			return m_bUseCameraCollision;
		}
		//////////////////////////////////////////////////////////////////////////
	};
}
