#pragma once
#include "IGraphic.h"
#include "CStaticObject.h"

using namespace std;

/*
//  每个Block是正方形的
*/
struct tagACFHeader
{
	uint32  dwFlag;            //文件标志
	uint32  dwSize;            //文件头大小
	uint32  dwFormat;          //文件格式
	uint32  dwWidth;           //图片宽度
	uint32  dwHeight;          //图片高度
	uint32  dwLineOffsetCount; //行偏移数量,每行一个偏移
	uint8   byRev[40];         //保留
};

struct MapUnit
{
	CPos		Pos;
	ITexture*	pTexture;

	CPos        PosName;
	std::string strName;
};
typedef vector< MapUnit > MapUnits;

class CCommpressFile : public virtual CStaticObject
{
	int32				m_nWidth;
	int32				m_nHeight;
	vector<TCHAR>		m_CommpressData;
public:
	~CCommpressFile()	{ CleanData(); }
	bool				ReadFile( const TCHAR* szInFile, uint8 uSceneID, IGraphic* pGraphic  );
	const vector<TCHAR>&	GetMapData()const{ return m_CommpressData; };
	void				CleanData();
	int32				GetWidth()const{ return m_nWidth; }
	int32				GetHeight()const{ return m_nHeight; }
};