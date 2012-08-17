#include "stdafx.h"
#include "StringHelper.h"
#include "PkgCommon.h"
#include "DebugHelper.h"
using namespace sqr;

enum EErrorType
{
	ET_USAGE_WRONG = 1
};

const string SOUND_CHECK_FILE_NAME = "sound.crc";

typedef map<uint32, uint32>	CheckMap;
typedef vector<string>		FileList;

CheckMap g_check_map;
FileList g_file_list;

void	GenFileList( const string& path )
{
	const string	CURR_PATH	= ".";
	const string	PARENT_PATH	= "..";
	
	WIN32_FIND_DATA	fd;
	string cur_file;
	string szFullPath(path + "/*");
	HANDLE h	= FindFirstFile( szFullPath.c_str(), &fd );
	if ( h != INVALID_HANDLE_VALUE )
	{
		BOOL	bMoreFile	= 1;
		while ( bMoreFile )
		{
			if ( CURR_PATH != fd.cFileName && PARENT_PATH != fd.cFileName )
			{
				cur_file = path + "/" + fd.cFileName;
				if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if ( fd.cFileName[0] != '.' )
						GenFileList(cur_file);
				}
				else
				{
					backslash_to_slash(cur_file);
					cur_file = cur_file.substr(cur_file.find("sound"), cur_file.length());
					g_file_list.push_back(cur_file);
				}
			}
			bMoreFile	= FindNextFile( h, &fd ); 
		}
		FindClose(h);
	}

}

void GenCheckCodes( const string& path )
{
	string cur_file;
	string res_path = path.substr(0, path.find("sound"));

	FileList::iterator it	= g_file_list.begin();
	FileList::iterator eit	= g_file_list.end();
	for ( ; it != eit; ++it )
	{	
		cur_file = res_path + *it;
		uint32 file_id = FileName2Id((*it).c_str());
		uint32 crc = 0;
		CCrc32::GetInst()->InitCrc32(crc);

		FILE* fp = 0;
		if ( fopen_s(&fp, cur_file.c_str(), "rb") != 0 )
		{
			cerr << "open " << cur_file << "failed." << endl;
			return;
		}

		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* date_buf = new char[size];
		if ( !date_buf )
		{
			cerr << "alloc data buf failed." << endl;
			return;
		}
		fread_s(date_buf, size, size, 1, fp);
		
		CCrc32::GetInst()->MakeCrc32Sub(crc, (uint8*)date_buf, (uint32)size);
		CCrc32::GetInst()->MakeCrc32End(crc);
		g_check_map.insert(make_pair(file_id, crc));

		delete[] date_buf;
		fclose(fp);
	}
}

void	WriteCheckFile(const string& path)
{ 
	string check_file = path + SOUND_CHECK_FILE_NAME;

	FILE* fp = 0;
	if ( fopen_s(&fp, check_file.c_str(), "wb") != 0 )
	{
		cerr << "write " << SOUND_CHECK_FILE_NAME << " failed." << endl;
		return;
	}

	uint32 file_count = (uint32)g_check_map.size();
	fwrite(&file_count, sizeof(uint32), 1, fp);
	
	CheckMap::iterator it	= g_check_map.begin();
	CheckMap::iterator eit	= g_check_map.end();
	for ( ; it != eit; ++it )
	{
		uint32 file_id	= it->first;
		uint32 crc		= it->second;
		fwrite(&file_id, sizeof(uint32), 1, fp);
		fwrite(&crc, sizeof(uint32), 1, fp);
	}

	fclose(fp);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc != 2 )
	{	
		cerr << "Usage: " << argv[0] << " sound_path";
		return ET_USAGE_WRONG;
	}

	string path = argv[1];
	backslash_to_slash(path);
	if ( path[path.size()-1] != '/' )
		path += "/";

	GenFileList(path);
	GenCheckCodes(path);
	WriteCheckFile(path);

	cout << "generate " << SOUND_CHECK_FILE_NAME << " succeeded.";

	return 0;
}

