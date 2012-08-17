#pragma once
#include <vector>
#include <hash_map>

using namespace std;
using namespace stdext;

struct TABLEFILE_HANDLE
{
	vector<char>				m_vecBuf;
	vector<vector<size_t> >		m_nOffsetByIndex;
	hash_map<string, size_t>	m_nOffsetByName;
};

TABLEFILE_HANDLE& GetTableFileHandle( void* hIniFile );
	
class CTxtTableFile
{
public:
	CTxtTableFile();
	~CTxtTableFile();
	// 载入制表符分隔的txt文件
	bool		Load( const char* szFileName );
	// 清空
	void		Clear();
	// 得到行数
	int			GetHeight();
	// 得到列数
	int			GetWidth();
	// 根据列号得到某行某列
	const char* GetString( int nRow, int nColumn );
	// 根据列名得到某行某列
	const char* GetString( int nRow, const char* szColumnName );

private:
	void* m_hFileHandle;

};
