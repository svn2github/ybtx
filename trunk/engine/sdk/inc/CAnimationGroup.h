#pragma once
#include "GraphicClassDefine.h"
#include "CRenderStyle.h"
#include "CTinyVector.h"
#include "CSkeletalFrame.h"
#include "CResUnitGroup.h"

namespace sqr
{
	enum DoAniIsValidResult
	{
		eAni_NoExist  = 0,
		eAni_InValid  = 1,
		eAni_Valid	  = 2,
	};

	class CAnimationGroup : public CResUnitGroup
	{
		friend class CAgpCoder;
		friend class CDataSources;
	public:
		typedef GVector<CAnimate*>		CAnimateList;
	protected:
		CAnimateList				m_AnimationList;		//所有动画列表
		GMap<GString,int32>			m_NameList;				//名字索引
		GList<CAnimate*>			m_AniList;				//优先载入队列
		GString						m_SkeName;				//索引骨架名字
		CSkeletalFrame*				m_pSke;

	public:
		CAnimationGroup(const string& szName = "", const wstring& Alias = L"");
		virtual ~CAnimationGroup() { Destroy(); }
		virtual	void	Destroy();

		inline uint32 GetAnimateNum() const;				//得到动画数量
		const char* GetSkeletalFrameName() const;
		//inline CAnimate* GetAnimate( uint32 Num );	
		inline CAnimate* operator [] ( uint32 Num )		const;			//得到指定动画
		inline CAnimate* operator [] ( IDNAME Name )	const;
		inline CAnimationGroup& operator = (const CAnimationGroup& OrgAniGroup );
		bool	Merge( CAnimationGroup* pGroup );
		int		GetAniIDbyName( IDNAME Name ) const;		//得到动画标号
		CAnimate* GetAniById(uint32 id);
		CAnimate* GetAniByName(IDNAME name);
		int		GetAnimationNum( IDNAME AnimationName );
		int		GetAnimationNum();
		IDNAME	GetAnimationName( int Num ) const;
		int		GetAnimationMaxFrame( int nAni );
		float   GetGroundSpeed(int nAni);
		int		GetFrameCountToFrameStr( int nAni, const char* szFrameStr, int nFrameStart, int nFrameEnd, int nStr );
		DoAniIsValidResult	IsAniValid(const char* szAniName);
		bool	IsDefaultAniValid(int id);
		void	InsertAni(CAnimate* pAni);
		void	LoadNextAni();

		CSkeletalFrame*	GetSkeletalFrame();

		//导入文件
		int		BinLoad(void);
		void	OnRead( CRefBufFile& GraphicFile );
		bool	IsValid();
		inline  int32  AddRef() { return CRefObject::AddRef(); }
	};

	inline const char* CAnimationGroup::GetSkeletalFrameName() const
	{
		return m_SkeName.c_str();
	}

	inline uint32 CAnimationGroup::GetAnimateNum() const
	{
		return (uint32)m_AnimationList.size();
	}

	inline CAnimate* CAnimationGroup::operator []( uint32 Num ) const
	{
		return Num < GetAnimateNum() ? m_AnimationList[Num] : NULL;
	}

	inline CAnimate* CAnimationGroup::operator []( IDNAME Name ) const
	{
		return (*this)[GetAniIDbyName(Name)];
	}

	inline CAnimationGroup& CAnimationGroup::operator = (const CAnimationGroup& OrgAniGroup )
	{
		m_AnimationList = OrgAniGroup.m_AnimationList;
		m_NameList		= OrgAniGroup.m_NameList;
		m_SkeName		= OrgAniGroup.m_SkeName;
		return *this;
	}
};