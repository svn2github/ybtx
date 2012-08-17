#ifndef _CFileFormat_Mgr_H_
#define _CFileFormat_Mgr_H_

#include "CCoder.h"
#include "CDataChunk.h"

namespace sqr
{
class CFileFormatMgr;

struct CodeFileInf
{
	CodeFileInf(CFileFormatMgr* pMgr, OBJ_HANDLE pO)
		: pFormatMgr(pMgr), pObj(pO) {}
	CFileFormatMgr*		pFormatMgr;
	OBJ_HANDLE	pObj;
};

class CFileFormatMgr : public CCoderHelper
{
protected:
	class CHeadCoder : public CCoder
	{
	public:
		CHeadCoder():m_pParentFormat(NULL){};
		FourCC		GetChunkName(void)	{ return 0; }; //排在Coder第一位
	protected:
		friend class	CFileFormatMgr;
		CFileFormatMgr* m_pParentFormat;
	};
	friend class CHeadCoder;

public:
	CFileFormatMgr(FourCC ccType, uint16 NewVer,CFileFormatMgr::CHeadCoder* pCoder = NULL);
	virtual ~CFileFormatMgr();

	virtual bool Save(const string& filename,OBJ_HANDLE handle);
	virtual bool Load(const string& filename,OBJ_HANDLE handle);
	virtual bool Load(const wchar_t* szAliasPath, const string& filename,OBJ_HANDLE handle);
	virtual	void _Load(OBJ_HANDLE handle, CRefBufFile& File);

protected:
	static  void		_OnReadAsyn( void* handle, CRefBufFile& File );
	

	const		FourCC		m_FileType;
	const		uint16		m_FileVer;
};

}

#endif //_CFileFormat_Mar_H_
