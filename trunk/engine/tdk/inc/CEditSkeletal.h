#pragma once
#include "CSkeletal.h"

namespace sqr_tools
{
	class CEditSkeletalUnit : public sqr::CSkeletalUnit
	{
	public:
		bool	m_IsSelected;
		const CEditSkeletalUnit& operator = ( const CEditSkeletalUnit& fEsu );
		int		BinSave( ofstream& File );
	};

	class CEditSkeletal 
		: public sqr::CSkeletal
	{
	public:
		bool	IsFitForMe( const CEditSkeletal* Skeletal, string& ErrSkeletal );
		const	CEditSkeletal& operator = ( const CEditSkeletal& fES );
		void	RefreshSkeletalUnit(GVector<CSkeletalUnit*>& VecUnit);
		int		BinSave( ofstream& File );
	};
}