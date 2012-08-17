#include "stdafx.h"
#include "CompressGZIP.h"
#include "ExpHelper.h"
#include "zlib/zlib.h"
#include "CMemData.inl"
//#include <iomanip>

namespace sqr
{

	void CompressBufferGZIP( CMemData& bufOut,const char* szData,size_t stSize )
	{
		uLong uOutSize=compressBound( uLong(stSize) );

		const size_t stOrgOutSize = bufOut.Size();

		bufOut.ReSize( uOutSize + stOrgOutSize );

		if( Z_OK != compress2( bufOut.AsPtr<Bytef>(stOrgOutSize)  , &uOutSize,
			reinterpret_cast<const Bytef*>(szData)  , uLong( stSize ),1 ) )
		{
			GenErr("gzip compress failed!");
		}

		bufOut.ReSize( uOutSize + stOrgOutSize );

		//printf("%d %d %0.2f\n",int(bufOut.Size()),int(stSize),bufOut.Size()/(float)stSize );

		//FILE* fp=fopen("compress.txt","a");
		//fwrite( &bufOut[ stOrgOutSize ],1,bufOut.Size()-stOrgOutSize,fp);
		//fclose(fp);
	}

	bool DecompressBufferGZIP( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize )
	{	
		//FILE* fp=fopen("decompress.txt","a");
		//fwrite(szData,1,stSize,fp);
		//fclose(fp);

		uLong uOutSize=uLong(stOrgDataSize);

		bool bResult=( Z_OK == uncompress( reinterpret_cast<Bytef*>( szOutBuffer ), &uOutSize, 
			reinterpret_cast<const Bytef*>( szData )  , uLong( stSize ) ) );

		Ast( stOrgDataSize >= uOutSize );

		return bResult;
	}

}
