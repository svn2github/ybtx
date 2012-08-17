#pragma once
#include "CFileFormatMgr.h"
#include "CSkeletalMesh.h"
#include "CStaticMesh.h"
#include "CCPUSkeletalMesh.h"
namespace sqr
{
	class CVertexAniMeshCoder 
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		FourCC	GetChunkName(void);
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号
		
		bool	Encode(WriteDataInf& wdi , OBJ_HANDLE handle );
		bool	DeCode_VertexAniMesh( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CStaticMeshCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CStaticMeshCoder();
		FourCC	GetChunkName(void);
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号
		
		bool	Encode( WriteDataInf& wdi , OBJ_HANDLE handle );
		bool	DeCode_StaticMesh( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	class CSkeletalMeshCoder 
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CSkeletalMeshCoder();
		FourCC	GetChunkName(void);
		uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号
		
		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	DeCode_SkeletalMesh( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
		bool	_DecodeCpuSkeMesh( const CDataChunk& inData , OBJ_HANDLE handle );
		bool	_DecodeGpuSkeMesh( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CCommonMeshCoder: public CCoder
	{
		DECLARE_VERSION_MAP();
	public:
		FourCC	GetChunkName(void);
		uint16	GetNewVer(void)		{return 1;}

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	DeCode_CommonMesh(const CDataChunk& inData, OBJ_HANDLE handle);

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

}
