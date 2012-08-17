#pragma once
#include "CSkeletal.h"
#include "CResourceContainer.h "
#include "GraphicRes.h"
#include "GraphicBase.h"

namespace sqr
{
	const int32 maxBoneNum = 256;
	

	class CSkeletalFrame
		: public CBaseGraphicRead
	{
		friend class CSkeCoder;
		friend class CDataSources;
	protected:
		CSkeletal*					m_pSkeletalRoot;			//根骨骼
		GVector<CSkeletalUnit*>		m_SkeletalList;			//所有骨骼矩阵（SOCKET）单元列表
		CSkeletalFrame(const string& szName, const wstring& Alias);
	public:
		virtual ~CSkeletalFrame();

		virtual	void	Destroy();

		inline	CSkeletalUnit* operator []( int Num )	const;					//得到指定骨骼矩阵单元
		inline	const CSkeletal& GetRootSkeletal()		const;					//得到根骨骼
		inline	uint32 GetSkeletalNum()					const; 					//得到骨骼数量

		int		GetSkeletalIDBySocketName( IDNAME SocketName, int& SocketNum );	//通过SOCKET名字得到骨骼ID
		bool	SetSkeletalMask( SKELETAL_MASK& Mask );							//设置有效骨骼
		void	GetIDBySocketName( int& SkeletalID, int& SocketID, IDNAME SocketName );
		int		GetSkeletalIDByName( const char* SkeletalName );;
		int		GetSkeletalIDByIndex( const int Index );
		int		GetSkeletalTotalNum();
		int		GetSkeletalChildNum( uint32 SkeletalID );
		int		GetSkeletalChildID( uint32 SkeletalID, uint32 ChildNum );
		int		GetSocketNum( uint32 SkeletalID );
		const char*	GetSkeletalName( uint32 SkeletalID );
		const char*	GetSocketName( uint32 SkeletalID, uint32 SocketID );

		//导入文件
		int		BinLoad(void);
		void	OnRead( CRefBufFile& GraphicFile );
		bool	IsValid();
	protected:
		void	Free(void);
	};

	inline CSkeletalUnit* CSkeletalFrame::operator []( int Num ) const
	{
		return m_SkeletalList[Num];
	}

	inline const CSkeletal& CSkeletalFrame::GetRootSkeletal() const
	{
		return *m_pSkeletalRoot;
	}

	inline uint32 CSkeletalFrame::GetSkeletalNum() const
	{
		return (uint32)m_SkeletalList.size();
	}
}