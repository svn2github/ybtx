#pragma once
#include "CDataChunk.h"
#include "CRefObject.h"
#include "CBufFile.h"
#include <stack>
#include "stdio.h"

namespace sqr
{
	extern void	ReadChunk( CBufFile& file, CDataChunk& chunk );

	class WriteDataInf
	{
		struct OffInf
		{
			CDataChunk::ChunkHdr Hdr;
			long	 OffSet;
		};

		FourCC				dcType;
		DWORD				dcVer;
		stack< OffInf >		wrStack;
		FILE*				pFile;
	public:
		WriteDataInf(FILE* pfile):pFile(pfile){};

		bool		Write( const CDataChunk& outData );
		bool		BeginWrite(const FourCC& type, uint16 ver, uint16 chunk_type);
		bool		Write(const void* pBuf, size_t uSize);		
		void		EndWrite();

		template<class T>
		void		WriteMap( const T& data )
		{
			size_t size = data.size();
			Write(&size);

			for ( typename T::const_iterator it = data.begin(); it != data.end(); ++it )
			{
				Write( &it->first );
				Write( &it->second );
			}
		}

		template<class T>
		void		Write( const T* data )
		{
			Write( static_cast<const void*>(data), sizeof(T) );
		}

		template<class T>
		void		operator << ( const T& data )
		{
			Write( static_cast<const void*>(&data), sizeof(T) );
		}

		void		operator << ( const char* data )
		{
			size_t len = strlen(data);
			Write( &len );
			Write( static_cast<const void*>(data), len );
		}
	};

	typedef void*	OBJ_HANDLE;
	class CCoder : public CRefObject
	{	
		friend class CCoderHelper;
	public:
		typedef bool (CCoder::*PFN_DECODE)( const CDataChunk& inData ,OBJ_HANDLE handle);

		CCoder(){};
		virtual ~CCoder() {};
		virtual FourCC	GetChunkName(void);
		virtual uint16	GetNewVer(void) { return 0; }
		virtual uint16	GetChunkType();
		
		virtual bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
		{
			return false; 
		};
		
		bool	DeCode( const CDataChunk& inData ,OBJ_HANDLE handle );

	protected:
		typedef void* PRIVATE_HANDLE;
		bool	_Encode( WriteDataInf& wdi, PRIVATE_HANDLE handle );
		
		virtual	PFN_DECODE	GetDeCoder(uint16 version) { return NULL; };

	private:
		virtual bool _EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			return false;
		};

		void	_BeginEncode( WriteDataInf& wdi );
		void	_EndEncode( WriteDataInf& wdi );
	};

	//_CODERMAP的标识位
	enum CfnSig
	{
		CfnSig_end = 0, // marks end of _CODERMAP map
		CfnSig_vv,
	};

	struct CODERMAP
	{
		DWORD				dwVersion;	//版本号
		CCoder::PFN_DECODE	pfnDecodefn;//解码的函数指针
		UINT				nSig;		//标识是否为最后一个节点
	};

	//代码版本静态管理宏
	#define DECLARE_VERSION_MAP() \
		static CODERMAP st_arCodermap[]; \
		CCoder::PFN_DECODE	GetDeCoder(uint16 version); 

	#define BEGIN_VERSION_MAP( class_name ) \
		CCoder::PFN_DECODE class_name::GetDeCoder( uint16 version ) \
		{CODERMAP* arCodermap = st_arCodermap;for(;arCodermap->nSig!=CfnSig_end;++arCodermap) \
		if(arCodermap->dwVersion == version){ return arCodermap->pfnDecodefn;} \
		return NULL;} \
		CODERMAP class_name::st_arCodermap[] = \
		{

	#define ON_VERSION( version, pfn_decode ) \
		{ (uint16)version,static_cast<CCoder::PFN_DECODE>(&pfn_decode), CfnSig_vv },

	#define END_VERSION_MAP() \
		{ 0,NULL,CfnSig_end } \
		}; 

	class CCoderHelper
	{
	public:
		virtual			~CCoderHelper();
		void			RegisterCoder(CCoder* pCoder);
		void			RemoveCoder( FourCC coderName );
		void			CleanCoder();
		CCoder*			GetSubCoder( FourCC coderName );

	protected:
		typedef			map<FourCC,CCoder*>		CoderMap;
		
		virtual void	_AutoDecode(const	CDataChunk& inData, CCoder::PRIVATE_HANDLE handle);//顺序解码inData中的数据
		bool			_DoDecode(const	CDataChunk& inData, OBJ_HANDLE handle);//解码 当前inData
		bool			_DoDecode(char*	&pBuffer, OBJ_HANDLE handle);
		bool			_DecodeAsOneChunk( CBufFile& bufFile, OBJ_HANDLE handle );

		bool			_AutoEncode(WriteDataInf& wdi, CCoder::PRIVATE_HANDLE handle);
		
		CoderMap		m_ChunkCoder;
	};

}
