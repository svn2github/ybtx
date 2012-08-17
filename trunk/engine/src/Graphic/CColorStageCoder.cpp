#include "stdafx.h"
#include "CColorStageCoder.h"
#include "CBufFile.h"
#include "CColorStage.h"
#include "CGraphicMallocObject.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CColorStageCoder)
		ON_VERSION(1, DeCode_1)
	END_VERSION_MAP()

	FourCC CColorStageCoder::GetChunkName()
	{
		return 'CSCK';
	}

	uint16 CColorStageCoder::GetNewVer()
	{
		return 1;
	}

	bool CColorStageCoder::Code( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CColorStage* cs = static_cast<CColorStage*>(handle);
		wdi << cs->m_eStage;
		wdi << cs->m_eStageEx;
		wdi << cs->m_arg1;
		wdi << cs->m_arg2;
		wdi << cs->m_op;

		return true;
	}

	bool CColorStageCoder::DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CColorStage* cs = static_cast<CColorStage*>(handle);
		CBufFile buf( chunk.GetBuffer(), chunk.GetSize() );

		buf >> cs->m_eStage;
		buf >> cs->m_eStageEx;
		buf >> cs->m_arg1;
		buf >> cs->m_arg2;
		buf >> cs->m_op;

		if ( cs->m_eStageEx == CColorStage::SFE_COLOR )
		{
			cs->m_type = CColorStage::ET_COLOR;
		}
		else if ( cs->m_eStageEx == CColorStage::SFE_ALPHA )
		{
			cs->m_type = CColorStage::ET_COLOR;
		}
		else
		{
			GfkLogErr( "CColorStageCoder::DeCode_1() failed.", "文件读取到非法数据，数据包可能以损坏" );
		}

		return true;
	}
}