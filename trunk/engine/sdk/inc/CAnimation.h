#pragma once
#include "GraphicBase.h"
#include "CBufFile.h"
#include "CResourceContainer.h"
#include "CAnimationGroup.h"
#include "TraceHelper.h"
#include "CCoder.h"
namespace sqr
{
	struct CKeyFrames : public CGraphicMallocObject
	{
		virtual ~CKeyFrames() {}
	protected:
		GVector<uint16>				m_TranFrame;
		GVector<CVector3f>			m_TranData;
		GVector<uint16>				m_QuatFrame;
		GVector<CQuaternion>		m_QuatData;
	public:
		bool IsFrameExist() const
		{
			return !m_TranData.empty() && !m_QuatData.empty();
		};
		WORD GetFrameNum() const
		{
			return *m_TranFrame.rbegin();
		}
		void GetFirst( CQuaternion& Quat, CVector3f& Tran, float fParam )const
		{
			Quat = (CQuaternion)(*m_QuatData.begin());
			Tran = m_TranData[0]/*m_TranData.begin()->Get( fParam )*/;
		}

		void GetLast( CQuaternion& Quat, CVector3f& Tran, float fParam )const
		{
			Quat = (CQuaternion)(*m_QuatData.rbegin());
			Tran =m_TranData[m_TranData.size()-1]/* m_TranData.rbegin()->Get( fParam )*/;
		}

		void GetFrameData( CQuaternion& Quate, CVector3f& Trans, float fFrame, float fParam )const;

		void BinLoad( CBufFile& GraphicFile ,float fParam, size_t kSize);
		void BinLoadFull(CBufFile& GraphicFile, size_t kSize );
		//void BinSaveFull(WriteDataInf& wdi );
		virtual void _RecordTinyData(GVector<CTinyVector>& tinyVector,GVector<CTinyQuaternion>& tinyQuaternion)	{};

	};

	//动画类
	class CAnimate 
		: public CBaseGraphicRead
		, public CResUintNode
	{
		friend class CAniCoder;
	protected:
		//CAnimationGroup*		m_pAnimationGroup;
		int						m_MaxFrame;				//动画最大帧数
		float					m_FrameRatio;			//帧率
		float					m_fScale;				//放大率
		IDNAME					m_AnimationeName;		//动画名
		GVector<CKeyFrames>		m_SkeletalList;			//骨骼的集合
		GMap<WORD,IDNAME>		m_FrameString;			//动画帧特征字符串

		bool					m_IsCpAni;				//骨骼是否压缩

		float					m_fTransParam;
		float					m_GroundSpeed;			//每个动作对地的速度

		bool					m_bLoad;

		void OnRead( CRefBufFile& GraphicFile );
		//void BinLoad();

	public:
		CAnimate(CAnimationGroup* pAniGp, const string& szFileName, const wstring& Alias, IDNAME& AniName );
		
		virtual ~CAnimate(){}

	public:
		int32	CreateResFile( void );
		float	GetFrameRatio();
		void    SetFrameRatio( float Ratio );

		//返回骨骼数目
		uint32	GetSkeletalNum();

		//返回动画名字
		IDNAME	GetName();

		//得到动画帧数
		int		GetFrameNum();

		//返回指定骨骼的动画关键帧列表
		const CKeyFrames* GetKeyFrame( uint32 SkeletalID );

		//返回本动画所有动画帧特征字符串
		const GMap<WORD,IDNAME>* GetFrameString();

		//返回本动画中所有脚步声的关键帧及关键帧名
		const GMap<WORD, GString> GetFootStepFrames();
		//得到指定帧动画帧特征字符串
		const IDNAME* GetFrameString( WORD CurFrameNum );

		//得到动作对地的速度
		float	GetGroundSpeed();
		float GetScale();
		float	GetTransParam();
		CAnimationGroup* GetRootGroup(void);	

		bool IsLoaded();
	};

	inline float	CAnimate::GetFrameRatio()
	{
		return m_FrameRatio;
	}

	inline void    CAnimate::SetFrameRatio( float Ratio )
	{
		m_FrameRatio = Ratio;
	}

	//返回骨骼数目
	inline uint32	CAnimate::GetSkeletalNum()
	{
		if ( IsValid() )
			return (uint32)m_SkeletalList.size();
		else
			return 0;
	}

	//返回动画名字
	inline IDNAME	CAnimate::GetName()
	{
		return m_AnimationeName;
	};

	//得到动画帧数
	inline int		CAnimate::GetFrameNum()
	{
		//if ( !m_szName.empty() )BinLoad();
		if(IsValid())
			return m_MaxFrame;
		else
			return 0;
	}

	//返回指定骨骼的动画关键帧列表
	inline const CKeyFrames* CAnimate::GetKeyFrame( uint32 SkeletalID )
	{
		//if ( !m_szName.empty() )BinLoad();
		if( IsValid() && SkeletalID < m_SkeletalList.size() )
		{
			return &m_SkeletalList[SkeletalID];
		}
		else 
			return NULL;
	}

	//返回本动画所有动画帧特征字符串
	inline const GMap<WORD,IDNAME>* CAnimate::GetFrameString()
	{
		//if ( !m_szName.empty() )BinLoad();
		return &m_FrameString;
	}

	//得到动作对地的速度
	inline float	CAnimate::GetGroundSpeed()
	{
		return m_GroundSpeed;
	}

	inline float CAnimate::GetScale()
	{
		return m_fScale;
	}

	inline float	CAnimate::GetTransParam()
	{
		return m_fTransParam;
	}

	inline CAnimationGroup* CAnimate::GetRootGroup(void)
	{
		return (CAnimationGroup*)m_pGroup;
	}
}