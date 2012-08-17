#pragma once

struct lua_State;

namespace sqr
{
	class CPkgFile;

	int ScriptSeek( CPkgFile* file, long pos, int flag );
	size_t ScriptRead( void* buf, size_t size, CPkgFile* file );
	int ScriptClose( CPkgFile* file );
	long ScriptTell( CPkgFile* file );
	long ScriptSize( CPkgFile* file );
	CPkgFile* ScriptOpen( const char* file );
	int ScriptEOF( CPkgFile* file );
	int ScriptGetC( CPkgFile* file );
	int ScriptUnGetC( int c, CPkgFile* file );

	int ASLoadFileExport( lua_State* L );
	int ASDoFile( lua_State* L );

	int AliasLoadFile(lua_State* pState);
	int AliasDoFile(lua_State* pState);
	
}
