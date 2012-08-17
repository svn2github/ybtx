#pragma once

namespace sqr
{

	bool ClearLog( const char* szFileName);

	bool WriteLog( const char *szFileName,const char* szMessage);

	bool WriteBinary( const char* szFileName,const void* pBuffer,size_t stLen );


}
