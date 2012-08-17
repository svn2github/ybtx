#include "stdafx.h"
#include "PathHelper.h"
#include "BaseDefs.h"

#include <direct.h>

	bool MakeDirectory( const TCHAR* szDirectory )
	{
		TCHAR szCurDir[MAX_PATH];
		TCHAR szBuf[MAX_PATH];
		_getcwd( szCurDir, MAX_PATH );
		strcpy_s( szBuf, MAX_PATH, szDirectory );
		TCHAR* pCur = szBuf;
		for(;;)
		{
			TCHAR* pCurDir = pCur;
			while( pCurDir[0] != '\\' && pCurDir[0] != '/' )
			{
				if( !pCurDir[0] )
				{
					int32 nResult;
					if( pCurDir != pCur )
						nResult=_mkdir( pCur );

					if( _chdir( szCurDir ) )
						return false;

					return true;
				}
				pCurDir++;
			}

			pCurDir[0] = 0;
			_mkdir( pCur ); 

			if ( _chdir( ( string( pCur ) + "/" ).c_str() ) )
				return false;			
			pCur = pCurDir + 1;
		}
	}