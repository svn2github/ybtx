#include "stdafx.h"
#include "CDemoCoder.h"
#include <string>
//----------------------------------------------------------
class ChunkCoder1 : public CCoder
{
public:
	DECLARE_VERSION_MAP();
	FourCC	GetChunkName(void)	{ return 'CKC1'; }	//Coder区块名
	uint16	GetNewVer(void)		{ return 1; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

	bool Code(WriteDataInf& wdi , OBJ_HANDLE handle )
	{
		string str = "Hello World!";
		size_t len = str.size();
		wdi.Write(&len,sizeof(size_t));
		wdi.Write(str.c_str(),str.size());
		return true;
	}

	bool	DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		size_t	len;
		string  str;
		tData.SafeRead( &len, sizeof(size_t) );
		str.resize(len);
		tData.SafeRead( &str[0], len );// 如果读取失败, SafeRead不会改变原来的值
		cout<<str;
		return true;
	}
};

BEGIN_VERSION_MAP(ChunkCoder1)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

//-----------------------------------------------------------

class ChunkCoder2 
	: public CCoder
	, public CCoderHelper
{
public:
	DECLARE_VERSION_MAP();
	FourCC	GetChunkName(void)	{ return 'CKC2'; }	//Coder区块名
	uint16	GetNewVer(void)		{ return 2; }		//Coder版本号,增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号

	bool Code(WriteDataInf& wdi , OBJ_HANDLE handle )
	{
		string str = "我后面跟着一个Chunk";
		size_t len = str.size();
		wdi.Write(&len,sizeof(size_t));
		str.resize(len);
		wdi.Write(str.c_str(),str.size());
		_AutoEncode(wdi,handle); //保存这个地方顺序出现的Chunk
		str = "我前面有一个Chunk";
		len = str.size();
		wdi.Write(&len,sizeof(size_t));
		str.resize(len);
		wdi.Write(str.c_str(),str.size());
		return true;
	}

	bool	DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData,handle); //如果次Chunk中的所有数据也都是Chunk可使用这个函数界自动解码 
		return true;
	}

	bool	DeCode_2( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		size_t	len;
		string  str;
		tData.SafeRead( &len, sizeof(size_t) );
		tData.SafeRead( &str[0], len );
		cout<<str;

		//读取后面的Chunk
		CDataChunk tChunk;
		tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
		ReadChunk(tData,tChunk);
		_DoDecode(tChunk,handle);//读取这个地方顺序出现的Chunk


		tData.SafeRead( &len, sizeof(size_t) );
		tData.SafeRead( &str[0], len );
		cout<<str;
		return true;
	}
};

BEGIN_VERSION_MAP(ChunkCoder2)
	ON_VERSION( 1 , DeCode_1 )
	ON_VERSION( 2 , DeCode_2 )
END_VERSION_MAP()

//-----------------------------------------------------------

class ChuckCoder2inner
	: public CCoder
{
public:
	DECLARE_VERSION_MAP();
	FourCC	GetChunkName(void)	{ return 'CK2i'; }	
	uint16	GetNewVer(void)		{ return 1; }

	bool Code(WriteDataInf& wdi , OBJ_HANDLE handle )
	{
		string str = "传进来的指针地址是:";
		size_t len = str.size();
		wdi.Write(&len,sizeof(size_t));
		wdi.Write(str.c_str(),str.size());

		wdi.Write(&handle,sizeof(OBJ_HANDLE));
		return true;
	}

	bool	DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		size_t	len;
		string  str;
		tData.SafeRead( &len, sizeof(size_t) );
		tData.SafeRead( &str[0], len );// 如果读取失败, SafeRead不会改变原来的值
		cout<<str;
		void* pObj;
		tData.SafeRead( &pObj, sizeof(void*) );
		if( pObj == (void*)(0xeefefeff) )
			cout<< "传入指针正确";
		return true;
	}
};

BEGIN_VERSION_MAP(ChuckCoder2inner)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

//-----------------------------------------------------------

CDemoFormat::CDemoFormat(void)
: CFileFormatMgr( 'Demo'/*文件格式标示*/, 0 /*文件最新版本号*/ )
{
}

CDemoFormat::~CDemoFormat(void)
{ /*do nothing~~~*/ }

class CAnyObject;

void CDemoFormat::TestMe(void)
{
	CDemoFormat TestFormat;
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	
	TestFormat.RegisterCoder(new ChunkCoder1);

	ChunkCoder2* pCoder = new ChunkCoder2;
	pCoder->RegisterCoder(new ChuckCoder2inner);
	TestFormat.RegisterCoder(pCoder);

	TestFormat.Save( "C:/Demo.data", pObj ); //可以用 UE 打开看看
	TestFormat.Load( "C:/Demo.data", pObj );
}