#pragma once
#include "CFileFormatMgr.h"
#include "SQRWnd.h"

namespace sqr
{
	class CGuiFormat : public CFileFormatMgr
	{
	public:
		CGuiFormat(void);
		~CGuiFormat(void);
	};
	
	struct CoderSoundInfo
	{
		int		tp;
		string	sf;
	};

	struct CoderFlashInfo
	{
		int		tp;
		string	fi;
	};

	struct CoderImagePro
	{
		int		cm;
		SQRWnd* wnd;
	};

	struct CoderImageState
	{
		IMAGE_PARAM		ip;
		WND_IMAGE_LIST* wil;
		SQRWnd*			wnd;
	};

	class CGUIPerSoundProCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIPerSoundProCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};
	// 一些属性的CODER
	// GUI 声音
	class CGUISoundProCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUISoundProCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};


	class CGUIPerFlashInfoCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIPerFlashInfoCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// flash info 指示信息
	class CGUIFlashInfoCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIFlashInfoCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};


	class CGUIPerImageProCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIPerImageProCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// image pro coder
	class CGUIImageProCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIImageProCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// image pro coder
	class CGUIImgProMskOneCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIImgProMskOneCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// image pro coder
	class CGUIImgProMskOtherCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIImgProMskOtherCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// image pro coder
	class CGUIImgProStateCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIImgProStateCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// image pro coder
	class CGUIImgTextureCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIImgTextureCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};


	// 单个CHILD WND的CODER
	class CGUIChildWndCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIChildWndCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// 子WND MAP的CODER
	class CGUIWndsMapCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIWndsMapCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};


	class CGUIWndsDefProCoder
		: public CCoder
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIWndsDefProCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// 单个WND的CODER
	class CGUIWndItemCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIWndItemCoder();
		FourCC	GetChunkName(void);							// coder chunk name
		uint16	GetNewVer(void)		{ return 1; }			// coder version
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

	// 整体GUI资源CODER
	class CGUIResCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CGUIResCoder();
		FourCC	GetChunkName(void);						// Coder区块名
		uint16	GetNewVer(void)		{ return 1; }		// Coder版本号, 增加信息使用后加法不用提升版本号，Chunk格式大换血的时候提升版本号
		uint16	GetChunkType()		{ return CDataChunk::DCT_AUTO; }

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );
		bool	Decode1( const CDataChunk& inData , OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};
}
