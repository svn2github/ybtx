#include "stdafx.h"
#include "CompressBZIP2.h"
#include "ExpHelper.h"
#include "bzip2/bzlib.h"
#include "CMemData.inl"

namespace sqr
{

	void CompressBufferBZIP2( CMemData& bufOut,const char* szData,size_t stSize )
	{
		unsigned uOutSize= unsigned( stSize * 1.01f+0.5f) + 600;

		const size_t stOrgOutSize=bufOut.Size();

		bufOut.ReSize( uOutSize + stOrgOutSize );

		if( BZ_OK != BZ2_bzBuffToBuffCompress( bufOut.AsPtr<char>( stOrgOutSize )  , &uOutSize,
			const_cast<char*>(szData)  , unsigned(stSize),1,1,0 ) )
		{
			GenErr("bzip compress failed!");
		}

		bufOut.ReSize( uOutSize + stOrgOutSize );

		//
		//printf("%d\t%d\t%0.3f\n",int(stSize),int(bufOut.size()),bufOut.size()/(float)stSize);
		//CCompressString sOutText;
		//DecompressBufferBZIP2(sOutText,bufOut.data(),bufOut.size());
		//Ast( sOutText==CCompressString(szData,stSize) );
	}

	bool DecompressBufferBZIP2( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize )
	{
		unsigned uOutSize= unsigned( stOrgDataSize );

		bool bResult = ( BZ_OK == BZ2_bzBuffToBuffDecompress( szOutBuffer, &uOutSize,
			const_cast<char*>( szData ) , unsigned( stSize ),1,1 ) );

		Ast( stOrgDataSize >= uOutSize );

		return bResult;
	}

}
