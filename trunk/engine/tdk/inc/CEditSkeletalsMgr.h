#pragma once
#include "CSkeletalsMgr.h"
#include "CEditSubject.h"

namespace sqr_tools
{
	class CEditSkeletalsMgr
		: public CSkeletalsMgr
		, public CEditSubject 
	{
	public:
		CEditSkeletalsMgr();

		static CEditSkeletalsMgr* GetEditInst();
	};

	inline CEditSkeletalsMgr* CEditSkeletalsMgr::GetEditInst()
	{
		return ( CEditSkeletalsMgr*)GetInst();
	}
}