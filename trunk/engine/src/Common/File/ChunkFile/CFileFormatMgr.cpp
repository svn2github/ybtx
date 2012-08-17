#include "stdafx.h"
#include "CFileFormatMgr.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include "CPkgFile.h"

CFileFormatMgr::CFileFormatMgr(FourCC ccType, uint16 NewVer,CFileFormatMgr::CHeadCoder* pCoder)
	: m_FileType(ccType)
	, m_FileVer(NewVer)
{
	if(NULL!=pCoder)
	{
		pCoder->m_pParentFormat = this;
		RegisterCoder(pCoder);
	}
}

CFileFormatMgr::~CFileFormatMgr()
{
}

void	CFileFormatMgr::_OnReadAsyn( void* handle, CRefBufFile& File )
{
	CodeFileInf *pFInf = (CodeFileInf *)handle;
	SQR_TRY	
	{
		pFInf->pFormatMgr->_Load(pFInf->pObj, File);
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("CFileFormatMgr解码失败");
		LogExp(exp);
	}
	SQR_TRY_END;
	SafeDelete(pFInf);
}

void  CFileFormatMgr::_Load(void* handle, CRefBufFile& File)
{
	CDataChunk	inData;
	File.read(&inData.GetHdr(),sizeof(CDataChunk::ChunkHdr));

	if(inData.GetName()==m_FileType)
	{
		ReadChunk(File, inData);
		_AutoDecode(inData,handle);	
	}
}

bool	CFileFormatMgr::Load( const string& filename,OBJ_HANDLE handle )
{
	CodeFileInf *pFInf = new CodeFileInf(this,handle);
	SQR_TRY	
	{
		int32 n = CPkgFile::ReadWholeFile( L"", filename.c_str(), pFInf, _OnReadAsyn );

		if ( n < 0 )
		{	
			SafeDelete(pFInf);
			ostringstream strm;
			strm<<"CFileFormatMgr 打开文件错误"<<filename;
			ArtErr(strm.str());
			return false;
		}
		return true;;
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("CFileFormatMgr创建异常");
		LogExp(exp);
		SafeDelete(pFInf);
	}
	SQR_TRY_END;
	return false;
}

bool CFileFormatMgr::Load(const wchar_t* szAliasPath, const string& filename,OBJ_HANDLE handle)
{
	CodeFileInf *pFInf = new CodeFileInf(this,handle);
	SQR_TRY	
	{
		int32 n = CPkgFile::ReadWholeFile( szAliasPath, filename.c_str(), pFInf, _OnReadAsyn );

		if ( n < 0 )
		{	
			SafeDelete(pFInf);
			ostringstream strm;
			strm<<"CFileFormatMgr 打开文件错误"<<filename;
			ArtErr(strm.str());
			return false;
		}
		return true;;
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("CFileFormatMgr创建异常");
		LogExp(exp);
		SafeDelete(pFInf);
	}
	SQR_TRY_END;
	return false;
}

bool CFileFormatMgr::Save(const string& filename,OBJ_HANDLE handle)
{
	FILE* pFile;
	SetFileAttributes( filename.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
#ifdef _WIN32 
	pFile = _wfopen( gbk_to_utf16(filename).c_str(), L"wb" );
#else
	pFile  = fopen( utf16_to_utf8(filename).c_str(), "wb" );
#endif

	if( !pFile )
		return false;
	WriteDataInf	wdi(pFile);
	wdi.BeginWrite(m_FileType, m_FileVer, CDataChunk::DCT_AUTO);
	_AutoEncode(wdi,handle);	
	wdi.EndWrite();
	fclose( pFile );
	pFile = NULL;
	return true;
}
