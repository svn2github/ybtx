#pragma once
#include "CModifyRS.h"
#include "CRenderStyleEx.h"

namespace sqr
{
	class CRenderStyleEx;
	class CRenderStyleExInst
	{
	public:
		CRenderStyleExInst(CRenderStyleEx* pRenderStyle,TexturesPool* pTexturePool = NULL );
		~CRenderStyleExInst();

		void	Execute( void );
		void	UnExecute( void );
		void	Update( uint32 uFrameTime, uint32 uCurTime );

		ITexture*
			GetTexture( uint32 idx );

		float	GetTimeRange() const;
		bool	GetIsSync()	const;

		bool	PushModify(const CModifyRS* pMrs);
		bool	PopModify(const CModifyRS* pMrs);
	protected:
		CRenderStyleEx*			m_pRenderStyle;
		TexturesPool			m_TexturePool;
		CModifyRS::ModifyPool	m_ModifyPool;
	};
}