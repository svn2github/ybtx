#pragma once
#include "CPkgFile.h"
#include "CSkeletalFrame.h"
#include "TSingleton.h"

namespace sqr
{
	class CSkeletalsMgr 
		: public Singleton<CSkeletalsMgr>
		, public CGraphicMallocObject
	{
	public:
		CSkeletalsMgr(bool bInit = false);
		virtual ~CSkeletalsMgr();
		
	public:
		int				GetPoolSize();
		HRESULT			GetSkeInPool( const TCHAR* szName, CSkeletalFrame** pSkeletalFrame );
	public:
		static const string	 st_SkePathConfig;
	protected:
		bool			SkesPoolLoad();
		void			ChangePoolSize(const int& size);
		void			AddSkeToContainer( const string& Name, CSkeletalFrame* pSke);
		CSkeletalFrame* GetSkeFromContainer( const string& Name );

		GMap< GString, CSkeletalFrame* >	m_SkesContainer;
		int									m_PoolSize;
		bool								m_bInit;
	};
}