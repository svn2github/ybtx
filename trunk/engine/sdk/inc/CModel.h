#pragma once
#include "CRenderMatrix.h"
#include "CBaseModel.h"

namespace sqr
{
	class CPieceGroup;

#define STOPALLANI 1024
	class CModel : public CBaseModel
	{
		friend class CRenderGroup;
		friend class CAnimationController;
	public:
		struct BoxPair
		{
			BoxPair( uint16 id,CBaseAABB* aabb)
				: BoxID(id),pAABB(aabb)
			{

			}
			uint16		BoxID;
			CBaseAABB*	pAABB;
		};

		typedef	GVector< BoxPair > BoxPairPool;

	protected:
		CRenderMatrix		m_RenderMatrix;		//渲染时要保存的的骨骼矩阵列表
		
		BoxPairPool		m_BoxMap;		
	protected:
		//int					m_CenterBoneID;

		//bool				m_bDebugOn;
	protected:
		//bool				m_InvLoop;
		bool				m_bNeedPushRender;
		NodePool			m_vRenderNode;
		ERenderObjStyle		m_eRenderStyle;
		sMaterial			m_Material;
		void				_UpdateMesh(SKELETAL_MASK& SkeletalMask);
		void				_AddAABB(CPiece* pPiece);
		void				_UpdateSkeFrm(void);
		void				_UpdateAniGrp(void);
		
		set<uint32>			m_AddSkePool;
		GMap<WORD, GString>	m_CurAniFootStepFrames;///当前动作的脚步声关键帧

	public:
		CModel( ILogicHandler* pHandler = NULL );
		virtual ~CModel( void );

	public:
		virtual void		ClearModelRender(void);
		virtual int			RenderMesh(  RenderObjStyle* pRORS );
		
		inline sMaterial*	GetMaterial()
		{
			return &m_Material;
		}
		inline BoxPairPool*		GetBoxMap()
		{
			return &m_BoxMap;
		}

		inline const GMap<WORD, GString>& GetFootStepFrames() const
		{
			return m_CurAniFootStepFrames;
		};

	public:
		//判断是否位分离的骨骼
		bool IsSepflag( int32 id );
		IDNAME GetCurAnimateName();
		CAnimate* GetCurAni(int32 PartID);
		int GetAnimateMaxFrameNum();
		void GetScaleDimension( float* pHeightBuf, int& Width, int&Depth, float GridSize );
		void ClearAnimateGroup();
		void RenderImme(void);

		//virtual bool GetMeshMatrix( CMatrix& Matrix, uint32 SkeletalID );
		uint32 GetSkeletalCount(void);
		BYTE GetMaxUseSkeletal(void);
		
		void UpdateDynamicBox(void);
		void UpdateBox(void);
		void ShowPiece(const char* szPieceName,bool bShow);
		//void ShowPieceBuffer(const char* szPieceName,bool bShow);
		void SetPieceClassRS( const char* szClassName, const CPieceRS& rs );

		virtual bool SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop = true, int32 DelayTime = 200 /*毫秒*/, float AniSpeed = -1.0f );
	
		bool SetDefaultAniSep( int32 id, int32 DelayTime = 200, float AniSpeed = -1.0f );
		bool SetAniSep( IDNAME AniName, int32 id, bool bLoop, int32 DelayTime, float AniSpeed = -1.0f );// 设置模型子动作
		bool SetAniSep( IDNAME AniName, bool bLoop, int32 DelayTime = 200, float AniSpeed = -1.0f);		// 设置模型子动作
		void ClearAniSep( int32 PartID ,int32 DelayTime = 200);											// 取消模型子动作
		bool SetAniFromSep( int32 PartID,int32 DelayTime = 200 );										// 将子动作设置成全身动作

		void StopAni( int32 PartID );			//暂停模型的动作
		void ResumeAni( int32 PartID );			//继续播放模型的动作	

		void SetVisible(bool isVisible);
		inline		void		SetVisible( CCamera* pCamera ) { CBaseModel::SetVisible(pCamera); }

		int32 AddSkeletal( const char* name );
		void DelSkeletal( const char* name );
		void ClearAddSke(void);
		void DelFramework();

		/*	void DelAniSepAllSkeletal();	*/											// 删除全部子动作骨骼
		bool GetSocketXYZ( const char* strSocketName, CVector3f & p);

		const AniResult Render( uint32 uCurTime, RenderObjStyle* pRORS = NULL ) ;	//渲染模型
		void  SetFixMatrix(const CMatrix& parent, const CMatrix& fix = CMatrix::UNIT_MATRIX);
		void UpdateRenderStyle( RenderObjStyle* pRORS );

		//计算矩阵
		void CalculateMatrix( int32 uDeltaTime );

	
		IntersectFlag IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir );
		//void RenderDebugLine(void);
		// add end
		virtual float GetCurFrameNum();
				void  ClearRsMask(bool bClear = false);
	};
}