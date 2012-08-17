#include "stdafx.h"
#include "ScriptFileComm.h"
#include "CPkgFile.h"
#include "ScriptCommon.h"
#include "CScriptData.h"
#include "CodeCvs.h"
#include "StringHelper.h"
#include "CScriptData.h"

namespace sqr
{
	int ScriptSeek( CPkgFile* file, long pos, int flag )
	{
		return file->Seek( pos, flag );
	}

	size_t ScriptRead( void* buf, size_t size, CPkgFile* file )
	{
		return file->Read( buf, size );
	}

	int ScriptClose( CPkgFile* file )
	{
		delete file;	return 0;
	}

	long ScriptTell( CPkgFile* file )
	{
		return file->Tell();
	}

	long ScriptSize( CPkgFile* file )
	{
		return file->Size();
	}

	CPkgFile* ScriptOpen( const char* file )
	{
		CPkgFile* p = new CPkgFile( L"", file );
		return p->IsValid() ? p : (CPkgFile*)((size_t)ScriptClose(p));
	}

	int ScriptEOF( CPkgFile* file )
	{
		return ScriptTell( file ) == ScriptSize(file);
	}

	int ScriptGetC( CPkgFile* file )
	{
		char c;
		if (ScriptRead( &c, 1, file ))
			return c;
		return EOF;
	}

	int ScriptUnGetC( int c, CPkgFile* file )
	{
		ScriptSeek( file, -1, SEEK_CUR );
		return c;
	}

#define MAX_READ_BUFFER 1024

	struct _LoadF
	{
		CPkgFile* f;
		char buff[MAX_READ_BUFFER];
	};

	static const char *_ScriptFileReader (lua_State *L, void *ud, size_t *size) 
	{
		_LoadF *lf = (_LoadF*)ud;
		(void)L;

		if (ScriptTell(lf->f) == ScriptSize(lf->f))
			return NULL;

		*size = ScriptRead(lf->buff, MAX_READ_BUFFER, lf->f);
		return (*size > 0) ? lf->buff : NULL;
	}

	typedef basic_string<char, char_traits<char>, CScriptAllocator<char> > String_t;

	static int _ScriptLoadFile(lua_State* pState, const char* szAliasPath, const char* szFileName)
	{
		if (!szFileName)
		{
			luaL_error(pState, "没有提供文件名");
			return 0; 
		}

		String_t sFileName(szFileName);
		sFileName.append(".lua");

		wstring sAliasPath = szAliasPath ? utf8_to_utf16(szAliasPath) : L"";

		CPkgFile* p = new CPkgFile(sAliasPath.c_str(), sFileName.c_str() );
		if( !p->IsValid() )
		{
			ScriptClose(p);
			lua_pushfstring( pState, "没找到文件%s\n", szFileName );
			return LUA_ERRFILE; 
		}

		_LoadF lf;
		lf.f = p;

		String_t chunkname;
		chunkname.append("@").append(sFileName);
		int ret = lua_load(pState, _ScriptFileReader, &lf, chunkname.c_str());
		ScriptClose(p);
		return ret;
	}

	typedef map<String_t, String_t, less<String_t>, 
		CScriptAllocator<pair<String_t, String_t> > > MapAliasPath_t;


	int ASLoadFile( lua_State*L, const char* szFileName )
	{
		int nBase = lua_gettop( L );
		int r = 0;

		r = _ScriptLoadFile( L, NULL, szFileName );
		if( r != LUA_ERRFILE )
			return r;


		CScriptData* pData=CScript::Inst()->GetData();
		MapAliasPath_t::iterator iter = pData->m_mapAliasToPath.begin();
		while (iter != pData->m_mapAliasToPath.end())
		{
			lua_settop( L, nBase );				//清除堆栈中的错误信息
			
			String_t sPath = iter->second;
			sPath.append(szFileName);
			
			r = _ScriptLoadFile( L, iter->first.c_str(), sPath.c_str() );
			if( r != LUA_ERRFILE )
				return r;
			++iter;
		}


		//没找到文件
		lua_settop( L, nBase );				//清除堆栈中的错误信息
		lua_pushfstring( L, "没找到文件%s\n", szFileName );
		return LUA_ERRFILE;
	}


	int ASLoadFileExport( lua_State* L )
	{
		const TCHAR* szFileName = lua_tostring( L, 1 );
		lua_pop( L, 1 );
		if( ASLoadFile( L, szFileName) != 0 )
		{
			lua_pushnil( L );
			lua_insert( L, -2 );
			return 2;
		}

		return 1;		//return function
	}

	int ASDoFile( lua_State* L )
	{
		const TCHAR* szFileName = lua_tostring( L, 1 );
		lua_settop( L, 0 );						//清空堆栈，为了后面得到 dofile的返回值个数

		lua_pushcfunction( L, &CScriptData::ErrorHandler );
		int nErrIdx = lua_gettop( L );

		if( ASLoadFile( L, szFileName) != 0 )
		{
			const TCHAR* szError = lua_tostring( L, -1 );
			luaL_error( L, "%s\n", szError );
		}

		lua_pcall( L, 0, LUA_MULTRET, nErrIdx );

		lua_remove( L, nErrIdx );

		return lua_gettop( L );
	}


	static int _AliasLoadFile(lua_State* pState, const char* szAliasPath, const char* szFileName)
	{
		if(!szFileName)
		{
			luaL_error(pState, "文件名为空");
			return 0;
		}

		String_t sAliasPath = szAliasPath ? szAliasPath : "";

		CScriptData* pData=CScript::Inst()->GetData();

		String_t sPath = pData->m_mapAliasToPath[sAliasPath];
		sPath.append(szFileName);

		int r = _ScriptLoadFile( pState, sAliasPath.c_str(), sPath.c_str());

		return r;
	}

	int AliasLoadFile(lua_State* pState)
	{
		const char* szAliasPath = lua_tostring(pState, -2);
		const char* szFileName = lua_tostring(pState, -1);

		lua_pop(pState, 2);

		if( _AliasLoadFile(pState, szAliasPath, szFileName) != 0 )
		{
			lua_pushnil( pState );
			lua_insert( pState, -2 );
			return 2;
		}

		return 1;		
	}

	int AliasDoFile(lua_State* pState)
	{
		const char* szAliasPath = lua_tostring(pState, -2);
		const char* szFileName = lua_tostring(pState, -1);

		lua_pop(pState, 2);		//清空堆栈，为了后面得到 dofile的返回值个数

		lua_pushcfunction( pState, &CScriptData::ErrorHandler );
		int nErrIdx = lua_gettop( pState );

		if( _AliasLoadFile(pState, szAliasPath, szFileName) != 0 )
		{
			const char* szError = lua_tostring( pState, -1 );
			luaL_error( pState, "%s\n", szError );
		}

		lua_pcall( pState, 0, LUA_MULTRET, nErrIdx );

		lua_remove( pState, nErrIdx );

		return lua_gettop( pState );
	}
}
