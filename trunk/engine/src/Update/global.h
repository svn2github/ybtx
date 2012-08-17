#pragma once
#include <vector>
#include <string>
#include "HeadDataType.h"
using namespace std;


NAMESPACE_BEGIN(sqr)


//更新包文件头
//struct UpdateFileHead
//{
//	DWORD flag;	//标志 'XYUD' 标记XYBZ表示用bzip2
//	DWORD ver;	//版本号
//	DWORD decompress_len;	//下面数据解压后的长度
//};

////资源更新节
//struct UpdateSec
//{
//	DWORD flag;
//	DWORD file_len;
//	DWORD pathname_len;
//	TCHAR pathname[1];
//};

//string get_setting(const string& file, const string& strName, const string& strDefault ,const string& dst_path );
//void get_url_info(const string& url, string& server, short& port, string& file);
//void get_url_info(const string& url, string& server, string& port, string& file);

//string get_filename_from_url(const string& url);

//typedef enum {
//	LAST = 0,
//	EXILITY,
//	RANDOM
//} seed_style;

void CreatePatchList(PatchList& pl,const string& buf);


NAMESPACE_END

