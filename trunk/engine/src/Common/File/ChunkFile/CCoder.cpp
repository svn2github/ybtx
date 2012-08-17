#include "stdafx.h"
#include "CCoder.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"

namespace sqr
{

	bool WriteDataInf::Write( const CDataChunk& outData )
	{
		BeginWrite(outData.GetName(), outData.GetVersion(), outData.GetType());
		Write(outData.GetBuffer(),outData.GetSize());
		EndWrite();
		return true;
	}

	bool WriteDataInf::BeginWrite(const FourCC& type, uint16 ver, uint16 chunk_type)
	{	
		if( wrStack.size() != 0 )
		{
			OffInf& Inf = wrStack.top();
			if(Inf.Hdr.dwName==type && Inf.Hdr.stSize == 0 && Inf.Hdr.ver == ver && Inf.Hdr.type == chunk_type)
				return true;
		}

		OffInf nInf;
		nInf.Hdr.dwName = type;
		nInf.Hdr.ver	= ver;
		nInf.Hdr.type	= chunk_type;
		nInf.Hdr.stSize = 0;
		nInf.OffSet = ftell(pFile);

		fwrite(&nInf.Hdr, sizeof(CDataChunk::ChunkHdr),1,pFile);
		wrStack.push(nInf);
		return true;
	}

	bool WriteDataInf::Write(const void* pBuf, size_t uSize)
	{
		OffInf& Inf = wrStack.top();
		Inf.Hdr.stSize += uSize;
		fwrite(pBuf,uSize,1,pFile);
		return true;
	}

	void WriteDataInf::EndWrite()
	{
		OffInf Inf = wrStack.top();
		wrStack.pop();
		long org = ftell(pFile);
		fseek(pFile,Inf.OffSet,SEEK_SET);
		fwrite(&Inf.Hdr, sizeof(CDataChunk::ChunkHdr),1,pFile);
		fseek(pFile, org, SEEK_SET);
		if(0 != wrStack.size())
		{
			wrStack.top().Hdr.stSize += sizeof(CDataChunk::ChunkHdr);
			wrStack.top().Hdr.stSize += Inf.Hdr.stSize;
		}
	}

	void sqr::ReadChunk( CBufFile& file, CDataChunk& chunk )
	{
		if ( int(file.Tell() + chunk.GetSize()) > file.GetBufSize() )
			GenErr("文件损坏或代码错误，读文件越界！");
		chunk.SetBuffer(const_cast<char*>(file.GetBuf()+file.Tell()),chunk.GetSize());
		file.Seek( long(chunk.GetSize()), SEEK_CUR );
		return;
	}

	// ---- CCoder -----------------------------------------------------------------------------------

	uint16 CCoder::GetChunkType()
	{
		return 0;
	}

	FourCC CCoder::GetChunkName(void)
	{
		return 'CODE';
	}

	void CCoder::_BeginEncode( WriteDataInf& wdi )
	{
		wdi.BeginWrite(GetChunkName(), GetNewVer(), GetChunkType());
	}

	void CCoder::_EndEncode( WriteDataInf& wdi )
	{
		wdi.EndWrite();
	}

	bool CCoder::_Encode( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_BeginEncode(wdi);
		bool ret = _EncodeImpl(wdi, handle);
		_EndEncode(wdi);

		return ret;
	}

	bool CCoder::DeCode( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		PFN_DECODE pFn = GetDeCoder(inData.GetVersion());
		if(NULL!=pFn)
			return (this->*pFn)(inData,handle);
		return false;
	};

	//-------------------------------------
	CCoderHelper::~CCoderHelper()
	{
		CoderMap::iterator it = m_ChunkCoder.begin();
		CoderMap::iterator eit = m_ChunkCoder.end();
		for( ;it!=eit;++it)
			SafeRelease(it->second);
	}

	void CCoderHelper::RegisterCoder(CCoder* pCoder)
	{
		Ast(pCoder!=NULL);
		CoderMap::iterator it = m_ChunkCoder.find(pCoder->GetChunkName());
		if(it!=m_ChunkCoder.end())
		{
			if(it->second!=pCoder)
			{
				SafeRelease(it->second);
				it->second = pCoder;
				pCoder->AddRef();
			}
		}
		else
		{
			m_ChunkCoder.insert(CoderMap::value_type(pCoder->GetChunkName(),pCoder));
			pCoder->AddRef();
		}
	}

	void CCoderHelper::RemoveCoder( FourCC coderName )
	{
		CoderMap::iterator it = m_ChunkCoder.find(coderName);
		if ( it != m_ChunkCoder.end() )
		{
			CCoder* coder = it->second;
			m_ChunkCoder.erase(it);
			SafeRelease(coder);
		}
	}

	void CCoderHelper::CleanCoder()
	{
		CoderMap::iterator it  = m_ChunkCoder.begin();
		CoderMap::iterator eit = m_ChunkCoder.end();

		for ( ; it != eit; ++it )
		{
			SafeRelease(it->second);
		}
		m_ChunkCoder.clear();
	}

	CCoder* CCoderHelper::GetSubCoder( FourCC coderName )
	{
		CoderMap::iterator it = m_ChunkCoder.find(coderName);
		if ( it != m_ChunkCoder.end() )
		{
			return it->second;
		}

		return 0;
	}

	bool CCoderHelper::_DoDecode(char* &pBuffer, OBJ_HANDLE handle)
	{
		CDataChunk tpDC;
		tpDC.GetHdr() = *((CDataChunk::ChunkHdr*)pBuffer);
		pBuffer += sizeof(CDataChunk::ChunkHdr);
		tpDC.SetBuffer(pBuffer,tpDC.GetSize());
		pBuffer+=tpDC.GetSize();
		return _DoDecode(tpDC,handle);

	}

	bool CCoderHelper::_DoDecode( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		CoderMap::iterator it = m_ChunkCoder.find(inData.GetName());
		if(it!=m_ChunkCoder.end())
		{
			SQR_TRY	
			{
				it->second->DeCode(inData, handle);
				return true;
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg(inData.GetName().AsString() + "CFileFormatMgr解码失败");
				LogExp(exp);
			}
			SQR_TRY_END;
		}
		return false;
	}


	bool CCoderHelper::_DecodeAsOneChunk( CBufFile& bufFile, OBJ_HANDLE handle )
	{
		CDataChunk	tChunk;
		bufFile.SafeRead(&tChunk.GetHdr(), sizeof(CDataChunk::ChunkHdr));
		ReadChunk(bufFile, tChunk);
		return _DoDecode(tChunk, handle);
	}

	bool CCoderHelper::_AutoEncode(WriteDataInf& wdi, CCoder::PRIVATE_HANDLE handle)
	{
		CoderMap::iterator it = m_ChunkCoder.begin();
		CoderMap::iterator ite = m_ChunkCoder.end();
		for(;it!=ite;++it)
		{
			it->second->Encode(wdi,handle);
		}
		return true;
	}

	void CCoderHelper::_AutoDecode(const	CDataChunk& inData, CCoder::PRIVATE_HANDLE handle)
	{
		CDataChunk	FileData;
		CBufFile File(inData.GetBuffer(), inData.GetSize() );	
		
		// 数据处理开始
		size_t Size = inData.GetSize();
		while(Size > 0)
		{
			File.read(&FileData.GetHdr(),sizeof(CDataChunk::ChunkHdr));
			Size -= sizeof(CDataChunk::ChunkHdr);
			if(Size>=FileData.GetSize())
			{
				ReadChunk(File, FileData);
				_DoDecode(FileData,handle);
				Size -= (uint32)FileData.GetSize();
			}
			else
				break;
		}
	}

}
