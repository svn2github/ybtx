#include "stdafx.h"
#include "CFilePack.h"
#include "PkgCommon.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "DebugHelper.h"



int32 file_size( const TCHAR* szFileName )
{
	FILE* f = fopen( szFileName, "rb" );
	if( !f )
		return -1;

	fseek( f, 0, SEEK_END );
	int32 len = ftell( f );
	fclose( f );

	return len;
}

bool file_exist( const TCHAR* szFileName )
{
	FILE *f = fopen( szFileName, "rb" );
	if( !f )
		return false;

	fclose(f);
	return true;
}



void XorBuf( TCHAR* buf )
{
	const uint8* key=GetPackKey();
	
	for( int i = 0; i < 1024; i+= 16 )
		for( int j = 0; j < 16; j++ )
			buf[i+j] = buf[i+j]^key[j];
}

CFilePack::CFilePack( const TCHAR* szPackName, bool bXor ) 
: m_sPackName( szPackName )
{ 
	m_FileHeader.bXor = bXor;
	m_sPackName += ".pkg";

	// 为包头结构生成虚拟文件信息节点。当紧邻包头结构的文件被删除时，可以将删除后的空间挂接到这个节点上
	FILE_INFO Info;
	Info.offset = 0;
	Info.space	= 0;
	Info.name	= "";
	Info.uid	= 0;
	Info.size	= sizeof(PackageHead);
	m_vecNewFiles.push_back(Info);
}

FILE* CFilePack::_ReadOldData()
{
	FILE* pFile = fopen( m_sPackName.c_str(), "rb+" );
	if( !pFile )
		return NULL;

	FILE_INFO old_head_info;
	bool bXor = m_FileHeader.bXor;
	if ( !ReadPackageHeadInfo(m_FileHeader, old_head_info, pFile) )
		return NULL;
	m_FileHeader.bXor = bXor;

	m_vecOldFiles.resize( m_FileHeader.number );
	SeekToFileIndexList(pFile, m_FileHeader);
	for( int32 i = 0; i < m_FileHeader.number; i++ )
		ReadPackageFileIndex(m_vecOldFiles[i], m_FileHeader, pFile);

	int32 nSizeDif = sizeof(PackageHead) - old_head_info.size;
	if ( nSizeDif > 0 )
	{
		if ( m_vecOldFiles[0].space < nSizeDif )
		{
			for ( int32 i=1; i<m_vecOldFiles.size(); ++i )
			{
				if ( m_vecOldFiles[i].offset == old_head_info.offset + old_head_info.size + old_head_info.space )
				{
					_DeleteFile(pFile, i);
					break;
				}
			}
		}

		m_vecOldFiles[0].space -= nSizeDif;
	}

	return pFile;
}

int32 CFilePack::_FindFile( uint32 id ) const
{
	for ( int32 i = 0; i < (int32)m_vecNewFiles.size(); i++ )
		if ( id == m_vecNewFiles[i].uid )
			return i;

	return -1;
}

void CFilePack::_DeleteFile( FILE *f, int32 n )
 {
	for( size_t i = 0; i < m_vecOldFiles.size(); i++ )
	{
		// i == 0 的解释：
		// m_vecOldFiles[0]为包头结构对应的虚拟文件，用于防止紧邻包头结构的文件被删除后的空间不被利用
		if ( m_vecOldFiles[i].uid != 0 || i == 0 ) 
		{
			if( m_vecOldFiles[i].offset + m_vecOldFiles[i].size + m_vecOldFiles[i].space == m_vecOldFiles[n].offset )
			{
				m_vecOldFiles[i].space += m_vecOldFiles[n].size + m_vecOldFiles[n].space;
				int32 n_pre = _FindFile(m_vecOldFiles[i].uid);
				if ( n_pre != -1 )
					m_vecNewFiles[n_pre].space  = m_vecOldFiles[i].space;
				
				int32 n_cur = _FindFile(m_vecOldFiles[n].uid);
				if ( n_cur != -1 )
					m_vecNewFiles[n_cur].space = 0;

				break;
			}
		}
	}

	fseek( f, m_vecOldFiles[n].offset, SEEK_SET );
	for( int32 i = 0; i < m_vecOldFiles[n].size; i++ )
		putc( 0xff, f );
	//fseek( f, m_vecOldFiles[n].offset + m_vecOldFiles[n].size, SEEK_SET );

	m_vecOldFiles[n].uid = 0;
}

bool CFilePack::_WriteFile( FILE *f, FILE_INFO& WrtFile, bool findspace )
{
	FILE *g;
	static TCHAR buffer[1024];
	size_t count;
	if( WrtFile.offset ) 
		return false;

	long pos = ftell(f);
	int space = -1;
	int space_n = 0;

	g = fopen( WrtFile.name.c_str(), "rb" );
	if( !g ) 
		return false;

	fseek( g, 0, SEEK_END );
	size_t size = ftell( g );
	fseek( g, 0, SEEK_SET );

	if( findspace ) 
	{
		for( int32 i = 0; i< (int32)m_vecNewFiles.size(); i++ ) 
		{
			// i == 0 的解释:
			// m_vecOldFiles[0]为包头结构对应的虚拟文件，用于防止紧邻包头结构的文件被删除后的空间不被利用
			if ( m_vecNewFiles[i].offset || i==0 ) 
			{
				if ( m_vecNewFiles[i].space >= size ) 
				{
					if ( space < 0 || ( space > 0 && (size_t)space > m_vecNewFiles[i].space ) ) 
					{
						space_n = i;
						space = m_vecNewFiles[i].space;
					}
				}
			}
		}

		if ( space > 0 )
		{
			WrtFile.offset = m_vecNewFiles[space_n].offset + m_vecNewFiles[space_n].size;
			fseek( f, WrtFile.offset, SEEK_SET );
			WrtFile.space = (unsigned int)space - size;
			m_vecNewFiles[space_n].space = 0;
		}
	}

	if ( space < 0 )
		WrtFile.offset = pos;

	CCrc32::GetInst()->InitCrc32(WrtFile.check_code);
	do
	{
		count = fread( buffer, 1, 1024, g );
		CCrc32::GetInst()->MakeCrc32Sub(WrtFile.check_code, (const uint8*)buffer, count);
		if( m_FileHeader.bXor ) // 异或加密
			XorBuf(buffer);
		fwrite( buffer, 1, count, f );
	} while( count == 1024 ); // 读取整个文件
	
	CCrc32::GetInst()->MakeCrc32End(WrtFile.check_code);
	fclose( g );
	if ( space >= 0 )
		fseek( f, pos, SEEK_SET );

	return true;
}

void CFilePack::_WriteList( FILE *f )
{
	map<uint32,FILE_INFO*> mapUseID;

	for( size_t i = 0; i < m_vecNewFiles.size(); i++ )
	{
		map<uint32,FILE_INFO*>::iterator it = mapUseID.find( m_vecNewFiles[i].uid );
		if( it == mapUseID.end() )
			mapUseID[m_vecNewFiles[i].uid] = &m_vecNewFiles[i];
		else
			//cout << m_vecNewFiles[i].uid << " ";
			cerr<< "[pack] err 文件" << m_vecNewFiles[i].name << "与文件" << it->second->name << "具有相同的hash值\n";
	}

	for( map<uint32,FILE_INFO*>::iterator it = mapUseID.begin(); it != mapUseID.end(); it++ )
	{
		WriteFileIndex(f, *it->second, PACKAGE_CUR_VER);
	}
}

// 注, 这个函数不检查长度是否相同
bool CFilePack::_NeedUpdate( FILE *f, FILE_INFO& NewFile, FILE_INFO& OldFile )
{
	static TCHAR buffer1[1024], buffer2[1024];
	FILE *g;
	fseek( f, OldFile.offset, SEEK_SET );
	size_t count;
	g = fopen( NewFile.name.c_str(), "rb" );
	if ( g == 0 )
		return false;

	if ( m_FileHeader.ver == 0 )
		CCrc32::GetInst()->InitCrc32(NewFile.check_code);
	
	do 
	{
		count = fread( buffer1, 1, 1024, g );

		if ( m_FileHeader.ver == 0 )
			CCrc32::GetInst()->MakeCrc32Sub(NewFile.check_code, (const uint8*)buffer1, count);
		
		if( m_FileHeader.bXor )
			XorBuf( buffer1 );
		if( count == 0 )
			break;
		fread( buffer2, 1, 1024, f );
		if( memcmp( buffer1, buffer2, count ) )
		{
			if ( m_FileHeader.ver == 0 )
				NewFile.check_code = 0;

			fclose(g);
			return true;
		}
	} while( count == 1024 );
	fclose( g );

	if ( m_FileHeader.ver == 0 )
		CCrc32::GetInst()->MakeCrc32End(NewFile.check_code);

	NewFile.offset	= OldFile.offset;
	NewFile.space	= OldFile.space;

	return false;
}

void CFilePack::_UpdateFile( FILE *f, FILE_INFO& NewFile, FILE_INFO& OldFile )
{
	fseek( f, OldFile.offset, SEEK_SET );
	_WriteFile( f, NewFile );
	NewFile.space	= OldFile.size + OldFile.space - NewFile.size;
	OldFile.size	= NewFile.size;
	OldFile.space	= NewFile.space;

	for( int32 i = 0; i < NewFile.space; i++ )
		putc( 0, f );
}

void CFilePack::_WriteHeader( FILE *f )
{
	WritePkgHead(f, m_FileHeader, PACKAGE_CUR_VER);
}

void CFilePack::_CreateData()
{
	FILE* pFile = fopen( m_sPackName.c_str(), "wb" );
	if( !pFile )
		return;
	
	_WriteHeader( pFile ); // 写入整个pkg文件的头文件
	m_FileHeader.number = (int32)m_vecNewFiles.size();

	// 以下for循环 i 从1开始是因为 m_vecNewFiles[0] 是包头结构对应的虚拟文件信息节点
	for( size_t i = 0; i < m_vecNewFiles.size(); i++ )
		_WriteFile( pFile, m_vecNewFiles[i] ); // 写入各文件内容

	m_FileHeader.offset = ftell( pFile );
	_WriteList( pFile );   // 
	_WriteHeader( pFile ); // 写入各文件的头文件(所有头文件聚集在一起，通过offset跳转到头文件的聚集起始)
	fclose( pFile );
}

void CFilePack::_UpdateData()
{
	FILE* pFile = _ReadOldData();
	if( !pFile )
		return;

	m_FileHeader.number = (int32)m_vecNewFiles.size();

	// 为包头结构空间字段赋值
	m_vecNewFiles[0].space = m_vecOldFiles[0].space;
	
	// 一下i从1开始的for循环，是因为m_vecOldFiles和m_vecNewFiles的第0个节点是
	// 包头结构对应的虚拟文件信息节点，不要参与更新检测
	for( int32 i = 1; i < (int32)m_vecOldFiles.size(); i++ )
		if( _FindFile( m_vecOldFiles[i].uid ) == -1 )
			_DeleteFile( pFile, i );

	for( int32 i = 1; i < (int32)m_vecOldFiles.size(); i++ )
	{
		if( m_vecOldFiles[i].uid == 0 ) 
			continue;
		int32 n = _FindFile( m_vecOldFiles[i].uid );
		int32 space = m_vecOldFiles[i].size + m_vecOldFiles[i].space;
		int32 size = file_size( m_vecNewFiles[n].name.c_str() );
		if ( size > space )
			_DeleteFile( pFile, i );

		else if( size == m_vecOldFiles[i].size )
		{
			if( _NeedUpdate( pFile, m_vecNewFiles[n], m_vecOldFiles[i] ) )
				_UpdateFile( pFile, m_vecNewFiles[n], m_vecOldFiles[i] );
		}
		else if( size <= space ) 
			_UpdateFile( pFile, m_vecNewFiles[n], m_vecOldFiles[i] );
	}

	// 寻找文件尾
	for( int32 i = 0; i < (int32)m_vecOldFiles.size(); i++ )
	{
		if( m_vecOldFiles[i].uid == 0 ) 
			continue;
		if( m_vecOldFiles[i].offset + m_vecOldFiles[i].size + m_vecOldFiles[i].space == m_FileHeader.offset ) 
		{
			m_FileHeader.offset = m_vecOldFiles[i].offset + m_vecOldFiles[i].size;
			m_vecOldFiles[i].space = 0;
			m_vecNewFiles[_FindFile( m_vecOldFiles[i].uid )].space = 0;
			break;
		}
	}

	fseek( pFile, m_FileHeader.offset, SEEK_SET );
	for( int32 i = 1; i < (int32)m_vecNewFiles.size(); i++ )
		_WriteFile( pFile, m_vecNewFiles[i], true );	// 添加文件(见缝插针)

	m_FileHeader.offset = ftell( pFile );
	_WriteList( pFile );
	_WriteHeader( pFile );
	fclose( pFile );
}

void CFilePack::AddFile( string szFileName )
{
	FILE_INFO Info;
	Info.name	= szFileName;
	Info.size	= file_size( szFileName.c_str() );

	// 从源文件路径中去掉包含包名的路径，使用相对路径计算uid
	string temp = m_sPackName;
	temp = temp.substr( 0, temp.size() - 4 ) + "/";		
	
	//int32  pos  = szFileName.find(temp) + temp.size();
	
	string fullfolder = "/" + temp;
	int32 pos = szFileName.rfind(fullfolder);
	if(pos == string::npos)
	{
		if(szFileName.substr(0, temp.size()) == temp)
		{
			pos = temp.size();
		}
		else
		{
			cerr << (string("路径") + szFileName + "中不存在" + fullfolder);
		}
	}
	else
	{
		pos += fullfolder.size();
	}

	szFileName	= szFileName.substr( pos, szFileName.size() );
	Info.uid	= FileName2Id( szFileName.c_str() );
	
	
	//TCHAR CurPath[200];
	//::GetCurrentDirectory(200, CurPath);
	if (Info.size == -1)
	{
		cerr << (string("文件") + szFileName + "无法打开");
	}
	m_vecNewFiles.push_back( Info );
 	if(!m_setFileID.insert(Info.uid).second)
	{
		cerr<<"id值冲突\n";
	}
}

void CFilePack::MakePackage()
{	
	if( file_exist( m_sPackName.c_str() ) )
		_UpdateData();
	else
		_CreateData();
}

