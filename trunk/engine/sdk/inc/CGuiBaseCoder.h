#pragma once
#include "CCoder.h"
#include "CRectangle.h"

namespace sqr
{
	class CBaseStringCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};


	class CBaseIntCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const int& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CBaseDWordCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const DWORD& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CBaseBoolCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const bool& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CBaseUnsignedCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CBaseCFRectCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const CFRect& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CBaseCFPosCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		uint16	GetChunkType() { return CDataChunk::DCT_DEPEND; }
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		virtual bool _DecodeImpl( const CFPos& refName, PRIVATE_HANDLE handle ) = 0;
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};
}