#pragma once
#include "CGeometryBatch.h"

namespace sqr
{
	class CModifyBatch : public CGeometryBatch
	{
	public:
		CModifyBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		~CModifyBatch(){};
		void			ModifyRS(STATIC_RS&	RS);
		DWORD			GetAddDecl(void);
	protected:
		struct ModifyRenderState 
		{
		public:
			ModifyRenderState(STATIC_RS* RS,DWORD* KEY, DWORD VAL);
			void ModifyRS(STATIC_RS&	RS);
		protected:
			size_t	Key;
			DWORD	Val;
		};

		typedef GVector<ModifyRenderState> ModifyVector;
		DWORD			m_AddVS;
		DWORD			m_AddPS;
		ModifyVector	m_VecModify;
		DWORD			m_ShaderId;
	};
}