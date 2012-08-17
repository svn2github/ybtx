#pragma once 
#include "CFixStage.h"

namespace sqr
{
	class CColorStage : public CFixStage
	{
	public:
		CColorStage(StageFlag sf = SF_Shader);
	
		void	Execute( CRenderStyleExInst* pRSInst );
		void	DefaultSetting();

	protected:
		void	BinLoad();
		void	BinSave();

	protected:
		enum	eType
		{
			ET_COLOR = 0,
			ET_ALPYA = 1,
			ET_COUNT,
			ET_FOURCEDWORD = 0xffffffff
		};

		uint32	m_arg1;
		uint32	m_arg2;
		uint32	m_op;
		eType	m_type;

		static const TEXTURESTAGESTATETYPE	ms_Arg1Type[ET_COUNT];
		static const TEXTURESTAGESTATETYPE	ms_Arg2Type[ET_COUNT];
		static const TEXTURESTAGESTATETYPE	ms_OpType[ET_COUNT];

		friend class CColorStageCoder;
	};
}