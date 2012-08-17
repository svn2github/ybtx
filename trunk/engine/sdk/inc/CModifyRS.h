#pragma once
#include "CGraphicMallocObject.h"

namespace sqr
{
	class CRenderStyleExInst;
	class CModifyRS : public CGraphicMallocObject
	{
		friend class CRenderStyleExInst;
	public:
		typedef	GList<CModifyRS*> ModifyPool;

	protected:
		virtual void	DoModify(void)	= 0;
		virtual void	UnModify(void);
	protected:
		struct InstPair : public CGraphicMallocObject
		{
			CRenderStyleExInst*		pRSInst;
			ModifyPool::iterator	itPos;
		};

		typedef	GList<InstPair> InstPool;

				CModifyRS(void);
		virtual ~CModifyRS(void);
	};
}