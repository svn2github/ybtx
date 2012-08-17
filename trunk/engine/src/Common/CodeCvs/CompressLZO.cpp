#include "stdafx.h"
#include "CompressLZO.h"
#include "ExpHelper.h"
#include "minilzo.h"
#include "CMemData.inl"
#include "ThreadHelper.h"

namespace sqr
{

	class CLzoWorkBuffer
	{
	public:
		static CLzoWorkBuffer	ms_Inst;

		char* GetBuffer();
	private:
		CLzoWorkBuffer();
		~CLzoWorkBuffer();
	private:
		stack<char*>	m_stkBuffer;
		HSPINLOCK		m_hLock;
		HTLSKEY			m_tlsKey;
	};

	CLzoWorkBuffer CLzoWorkBuffer::ms_Inst;
	
	char* CLzoWorkBuffer::GetBuffer()
	{
		char* pMem=reinterpret_cast<char*>( TLS_GetValue( m_tlsKey ) );

		if( !pMem )
		{
			//为什么要new这么多内存，参考testmini.c中的例子，这个文件是minilzo附带的.
			pMem=reinterpret_cast<char*>
				(new lzo_align_t[((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t)]);

			SpinLock( &m_hLock );
			m_stkBuffer.push( pMem );
			SpinUnlock( &m_hLock );

			TLS_SetValue( m_tlsKey, pMem );
		}
		return pMem;
	}

	CLzoWorkBuffer::CLzoWorkBuffer()
	{
		TLS_CreateKey( &m_tlsKey );
		CreateSpinLock( &m_hLock );
	}

	CLzoWorkBuffer::~CLzoWorkBuffer()
	{
		SpinLock( &m_hLock );
		while( !m_stkBuffer.empty() )
		{
			delete[] reinterpret_cast<lzo_align_t*>( m_stkBuffer.top() );
			m_stkBuffer.pop();
		}
		SpinUnlock( &m_hLock );

		TLS_DestroyKey( m_tlsKey );
		DestroySpinLock( &m_hLock );
	}

	void CompressBufferLZO( CMemData& bufOut,const char* szData,size_t stSize )
	{
		//计算保险长度的方法看testmini.c中的例子，这个文件是minilzo附带的
		lzo_uint uOutSize=lzo_uint(  stSize + stSize / 16 + 64 + 3 );

		const size_t stOrgOutSize = bufOut.Size();

		bufOut.ReSize( uOutSize + stOrgOutSize );
		
		if( LZO_E_OK != lzo1x_1_compress ( reinterpret_cast<const lzo_bytep>(szData), lzo_uint(stSize)
			, bufOut.AsPtr<lzo_byte>( stOrgOutSize ),&uOutSize,CLzoWorkBuffer::ms_Inst.GetBuffer() ) )
		{
			GenErr("lzo compress failed!");
		}

		bufOut.ReSize( uOutSize + stOrgOutSize );


		//CMemData test(bufOut);
		//test.ReSize(stSize);
		//DecompressBufferLZO(test.Data(),stSize,bufOut.AsPtr<const char>(4),bufOut.Size()-4);

		//printf("%d %d %0.2f\n",int(bufOut.Size()),int(stSize),bufOut.Size()/(float)stSize );

		//FILE* fp=fopen("compress.txt","a");
		//fwrite( &bufOut[ stOrgOutSize ],1,bufOut.Size()-stOrgOutSize,fp);
		//fclose(fp);
	}

	bool DecompressBufferLZO( char* szOutBuffer,size_t stOrgDataSize,const char* szData,size_t stSize )
	{	
		//FILE* fp=fopen("decompress.txt","a");
		//fwrite(szData,1,stSize,fp);
		//fclose(fp);
		
		lzo_uint uOutSize=lzo_uint(stOrgDataSize);

		bool bResult=( LZO_E_OK == lzo1x_decompress( reinterpret_cast<const lzo_bytep>( szData )  , lzo_uint( stSize )
			,reinterpret_cast<lzo_bytep>( szOutBuffer ), &uOutSize,NULL ) );

		Ast( stOrgDataSize >= uOutSize );

		return bResult;
	}

}
