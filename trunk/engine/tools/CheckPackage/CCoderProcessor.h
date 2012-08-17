#pragma once
#include <string>
#include "iresprocessormgr.h"
#include "GraphicBase.h"
#include "CFileFormatMgr.h"
/*********************************************************
// 只适合收集字符串类型的文件路径
**********************************************************/
class CCoderProcessor 
	: public CBaseResProcessor
	, public CFileFormatMgr
{
public:
	CCoderProcessor(FourCC ccType, uint16 NewVer,CFileFormatMgr::CHeadCoder* pCoder = NULL);
	virtual ~CCoderProcessor(void);

	virtual void			RegProcessTrunk(FourCC fourcc);
	virtual bool			RunProcess( const TCHAR* szFilename );
	virtual size_t			GetCountCanBeProcess() = 0;
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ) = 0;

protected:
	virtual void			_AutoDecode(const	CDataChunk& inData, void* handle);
protected:
	set<FourCC>				m_CltType;
	// 收集到的文件路径名
	set<string>				m_fileName;
	// 保存的资源的相对路径resbin
	string					m_filePath;	
	// 该processor收集的的文件路径名
	FourCC					m_fileType;
};