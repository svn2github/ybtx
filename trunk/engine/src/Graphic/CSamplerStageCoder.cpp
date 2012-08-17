#include "stdafx.h"
#include "CSamplerStageCoder.h"
#include "CSamplerStage.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CSamplerStageCoder)
		ON_VERSION(1, DeCode_1)
	END_VERSION_MAP()

	FourCC CSamplerStageCoder::GetChunkName()
	{
		return 'SSCK';
	}

	uint16 CSamplerStageCoder::GetNewVer()
	{
		return 1;
	}

	bool CSamplerStageCoder::Code( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CSamplerStage* ss = static_cast<CSamplerStage*>(handle);

		wdi << ss->m_eStage;
		wdi << ss->m_Samplers.size();	
	
		CSamplerStage::SamplerPoolIter it	= ss->m_Samplers.begin();
		CSamplerStage::SamplerPoolIter eit	= ss->m_Samplers.end();
		for ( ; it != eit; ++it )
		{
			wdi << it->first;
			wdi << it->second;
		}

		return true;
	}

	bool CSamplerStageCoder::DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CSamplerStage* ss = static_cast<CSamplerStage*>(handle);
		CBufFile buf( chunk.GetBuffer(), chunk.GetSize() );

		buf >> ss->m_eStage;

		uint32 nSamplerCount = 0;
		buf >> nSamplerCount;

		for ( uint32 i = 0; i < nSamplerCount ; ++i )
		{
			SAMPLERSTATETYPE type;
			uint32			 value;

			buf >> type;
			buf >> value;
			ss->m_Samplers.insert( make_pair(type, value) );
		}

		return true;
	}
}