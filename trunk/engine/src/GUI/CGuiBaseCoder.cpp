#include "stdafx.h"
#include "CGuiBaseCoder.h"
#include "Algorithm.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CBaseStringCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseStringCoder::Decode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		string refName;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		int32 size = bufFile.GetBufSize();
		vector<char> temp( size + 1, 0);
		bufFile.read(&temp[0], size);
		refName = &temp[0];

		return _DecodeImpl(refName, handle);
	}

	bool CBaseStringCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		string* refName = static_cast<string*>(handle);
		wdi.Write((const char*)refName->c_str(), refName->size());

		return true;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseIntCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseIntCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		int prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(int) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseIntCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		int* tp = static_cast<int*>(handle);
		wdi.Write( (const char*)tp, sizeof(int) );

		return true;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseDWordCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseDWordCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		DWORD prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(DWORD) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseDWordCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		DWORD* tp = static_cast<DWORD*>(handle);
		wdi.Write( (const char*)tp, sizeof(DWORD) );

		return true;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseBoolCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseBoolCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		bool prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(bool) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseBoolCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		bool* tp = static_cast<bool*>(handle);
		wdi.Write( (const char*)tp, sizeof(bool) );

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseUnsignedCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseUnsignedCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		unsigned prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(unsigned) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseUnsignedCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		unsigned* tp = static_cast<unsigned*>(handle);
		wdi.Write( (const char*)tp, sizeof(unsigned) );

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseCFRectCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseCFRectCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CFRect prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(CFRect) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseCFRectCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		CFRect* tp = static_cast<CFRect*>(handle);
		wdi.Write( (const char*)tp, sizeof(CFRect) );

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	BEGIN_VERSION_MAP(CBaseCFPosCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CBaseCFPosCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CFPos prs;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&prs, sizeof(CFPos) );

		return _DecodeImpl(prs, handle);
	}

	bool CBaseCFPosCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		CFPos* tp = static_cast<CFPos*>(handle);
		wdi.Write( (const char*)tp, sizeof(CFPos) );

		return true;
	}
}