#include "stdafx.h"
#include "LogHelper.h"


namespace sqr
{
	bool ClearLog( const char* szFileName)
	{
		FILE* file=fopen(szFileName,"w");
		if( !file )
			return false;

		fclose(file);
		return true;
	}

	bool WriteLog( const char *szFileName,const char* szMessage )
	{
		FILE* file=fopen( szFileName, "a");
		if( !file )
			return false;

		fputs( szMessage, file );

		fclose( file );

		return true;
	}

	bool WriteBinary( const char* szFileName,const void* pBuffer,size_t stLen )
	{
		FILE* file=fopen( szFileName, "a");
		if( !file )
			return false;

		for( size_t i =0 ;i<stLen; ++i)
			fprintf( file, "%02x",int(static_cast<const char*>(pBuffer)[i]) );
		
		fclose( file );

		return true;
	}

}
