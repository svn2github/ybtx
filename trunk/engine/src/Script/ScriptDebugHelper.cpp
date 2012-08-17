#include "stdafx.h"
#include "ScriptDebugHelper.h"
#include <algorithm>
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CScriptData.h"
#include "CPkgFile.h"
#include "ScriptFileComm.h"

namespace sqr
{

	string GetFileNameFromPathName(const char* szPathName)
	{
		string sFileName=szPathName;
		replace(sFileName.begin(),sFileName.end(),'\\','/');
		int32 nPos=(int)sFileName.find_last_of('/');
		if(nPos!=-1)
			sFileName.erase(0,nPos+1);
		return sFileName;
	}


	//-----------------------------------------------------
	// 显示脚本某一行
	//-----------------------------------------------------

	bool DebugShowLine( const char* szFilePathName, int line, bool bIsCurLine )
	{
		CPkgFile* fp = ScriptOpen( szFilePathName );
		if( !fp )
		{
			printf( "Source not available.\n" );
			return false;
		}
		else
		{
			int c = ScriptGetC( fp );
			if ( c == '#' || c == LUA_SIGNATURE[0] )
				printf( "%s\t%d(can not support binary file)\n", szFilePathName, line );
			else
			{
				ScriptSeek( fp, 0, SEEK_END );
				long size = ScriptTell( fp );
				ScriptSeek( fp, 0, SEEK_SET );
				char* szBuf = new char[ size + 1 ];
				ScriptRead( szBuf, size, fp );
				szBuf[size] = 0;

				char* pCur = szBuf;
				for( int i = 0; i < line - 1; pCur++ )
				{
					if( *pCur == 0 )
					{
						string sFileName=GetFileNameFromPathName(szFilePathName);
						printf("Line number %d out of range; %s has %d lines.\n",i+2,sFileName.c_str(),i+1);
						delete szBuf;
						return false;
					}
					if( *pCur == '\n' )
						i++;
				}

				for( int i = 0; pCur[i]; i++ )
				{
					if( pCur[i] == '\r' || pCur[i] == '\n' )
					{
						pCur[i] = 0;
						break;
					}
				}
				printf( "%d ", line );
				printf( bIsCurLine?">>":"  " );
				printf( "\t%s\n", pCur );
				delete szBuf;
			}
			ScriptClose( fp );
		}
		return true;
	}

	uint32 GetRunStackDepth(lua_State* pState)
	{
		lua_Debug ld;
		uint32 uDepth=0;
		for(;lua_getstack ( pState, uDepth, &ld );++uDepth);
		return uDepth;
	}

	bool PrintFrame( lua_State *pState, uint32 uLevel, ostream& str )
	{
		lua_Debug ld;
		if(!lua_getstack ( pState, uLevel, &ld ))
			return false;		
		
		str << "#" << uLevel;
		lua_getinfo ( pState, "n", &ld );
		if( ld.name )
			str << ld.name;
		else
			str << "(trunk)";

		lua_getinfo ( pState, "S", &ld );
		if( (ld.source[0] == '=') || (ld.source[0] == '@') )
			str << ld.source;

		if( ld.source[0] != '=' )
		{
			lua_getinfo ( pState, "l", &ld );
			str << ":" << ld.currentline;
		}
		
		str << endl;

		return true;
	}

	int GetCurLine( lua_State *pState, uint32 uLevel )
	{
		lua_Debug ld;
		lua_getstack ( pState, uLevel, &ld );
		lua_getinfo ( pState, "l", &ld );
		return ld.currentline;
	}


	//-----------------------------------------------------
	// 打印lua的pL堆栈中的最顶数值
	//-----------------------------------------------------
	void Print_lua( lua_State *pL, const list<string>& listField )
	{
		lua_checkstack(pL, 10);

		lua_getglobal( pL, "DebugPrint" );//stack：..., luaobj, DebugPrint
		lua_insert( pL, -2 );//stack: ..., DebugPrint, luaobj
		for( list<string>::const_iterator it = listField.begin(); it != listField.end(); it++ )
		{
			const char* szStr = it->c_str();
			if( szStr[0] >= '0' && szStr[0] <= '9' )
				lua_pushnumber( pL, atof( szStr ) );
			else
			{
				//有可能是 "aaa" 这样类型的字符串
				string strBuffer = *it;
				size_t stSize = strBuffer.size();
				if (strBuffer[0] == '"' && strBuffer[stSize - 1] == '"' )
				{
					strBuffer[stSize - 1] = '\0';
					strBuffer.erase(0, 1);
				}
								
				lua_pushstring( pL, strBuffer.c_str());
			}
		}
		lua_pcall( pL, 1 + (int32)listField.size(), 0, 0 );
	}


	void GetStackInfo(lua_State* pState, string& sStackInfo)
	{
		ostringstream strm;
		const int nMaxFrameLevel = 30;

		for(int i = 0; i < nMaxFrameLevel; i++)
		{
			bool bRet = PrintFrame(pState, i, strm);
			if (!bRet)
			{
				break;
			}
		}

		sStackInfo = strm.str().c_str();
	}
}

void sbt()
{
	CScript* pScript = CScript::Inst();
	if(!pScript)
		return;

	ostringstream strm;

	pScript->GetStack(strm);

#ifdef _WIN32
	OutputDebugString(strm.str().c_str());
#else
	cout << strm.str().c_str() << endl;
#endif
}

