#pragma once
#include "CMatrixUnit.h"
#include "GraphicBase.h"
#include "CAnimationGroup.h"
#include "CSkeletalMask.h"


namespace sqr
{
	typedef GVector<CMatrixUnit>	CMatrixUnitVector;

	enum EAniPlayMode
	{
		APM_PLAY_ONCE = 0,
		APM_LOOP_PLAY = 1,
		APM_PLAY_BACK = 2,
		APM_KEEP_PLAY = 3,
	};

	enum EAniPlayState
	{
		APS_FADEIN	= 0,
		APS_NORMAL  = 1,
		APS_FADEOUT	= 2,
	};

	class CAnimateSeparation : public CGraphicMallocObject	// 动作分离 结构
	{
	public:
		// BYTE	m_RootSkeletalId;	// 开始骨骼ID
		EAniPlayState	m_State;			// 动作状态	 0 - FadeIn 融合   1 - 正常播放   2 - FadeOut 融合
		BYTE	m_SkeletalID; 
		IDNAME	m_AnimationName;
		WORD	m_CurAnimate;		// 当前动画的ID
		float	m_CurFrame;			// 当前帧
		float	m_AniEndFrame;		// 当前动画最后一帧(end = num )
		float	m_BlendFrame;		// 融合帧数
		float	m_Weight;
		bool	m_nStopAni;
		float	m_AniSpeed;

		EAniPlayMode	m_PlayMode;			// 播放模式  0 - 循环播放  1 - 播放一次   2 - 播放完后持久保持
		CAnimateSeparation(BYTE ID)
			: m_nStopAni(false)
			, m_Weight(0.0f)
			, m_CurAnimate(0xFFFF)
			, m_CurFrame(0)
			, m_State(APS_FADEIN)
			, m_AniEndFrame(0)
			, m_PlayMode(APM_PLAY_ONCE)
			, m_SkeletalID(ID)
			, m_AniSpeed(1.0f)
		{}
			
		void Update( float DeltaFrame, CRenderMatrix & RenderMatrix );
	};



	//渲染时需要的骨骼矩阵列表类
	//如果有时间，我们把这个类里面的所有指针换成容器
	struct CRenderMatrix : public CGraphicMallocObject
	{
		CRenderMatrix();
		~CRenderMatrix();

		BYTE				m_MaxUseSkeletalID;					//骨骼数目

		bool				m_IsBlendStart;						//是否进入前后帧混合状态
		bool				m_IsAniBlend;						//是否需要进行前后帧混合
		bool				m_IsRended;
		WORD				m_CurAnimate;						//当前在计算的动画在CAnimateGroup中的位置

		WORD				m_AniStartFrame;					//下一个要播放的动画的起始帧
		WORD				m_AniEndFrame;						//下一个要播放的动画的结束帧(end = num - 1)
		WORD				m_BlendFrame;						//在下一个动画播放前需要进行融合的帧数
		bool				m_nStopAni;

		//CMatrixUnitPtrVector	mResBoenMatrixlist;				//用到的骨骼个数
		CMatrixUnitVector		m_pBoneMatrixlist;
		CMatrixUnitPtrVector	m_pMatrixUnit;					//保存矩阵计算结果
		CMatrixUnitPtrVector	m_pMatrixUnitOrg;				//保存矩阵
		CAnimateSeparationPtrVector		m_pSkeletalSepList;			
		CAnimationControllerPtrVector	m_pAniCtrlList;			//外部IK
		IModelHandler*				m_pHandler;

		CMatrix				m_MatWorld;
		SKELETAL_MASK		m_SkeletalMask;	
		float				m_CurFrame;							//当前帧
		//float				m_Anivelocity;						//播放的动作速率
		float				m_Weight;
		uint32				m_CurrentTime;
		GVector<IDNAME>		m_vecFrameString;
		GVector<IDNAME>		m_vecSepFrameString;
		AniResult			m_Result;

		void				Initialization( CSkeletalFrame* pSkeFrm );
		void				SetSkeletalMask( const SKELETAL_MASK& Mask );
		uint32				GetSkeletalCount();

		void				ClearSkeletaInfo(void);
		void				ClearAniSeparation(void);
		void				ClearAniController(void);
		void				Release();
		void				CalculateMatrix(void);//计算渲染矩阵
	};
}