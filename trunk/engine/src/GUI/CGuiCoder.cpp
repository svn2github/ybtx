#include "stdafx.h"
#include "CGuiCoder.h"
#include "CCoderNameHelper.h"
#include "CRefCoder.h"
#include "CGuiBaseCoder.h"
#include "CTexRefCoder.h"
#include "SQRWnd.h"
#include "GUIBase.h"
#include "StringHelper.h"
#include "Memory.h"
#include "CFontCfg.h"

#define string_size_check(sz, maxsz) \
	if( (sz) > (maxsz) ) \
		return false;


namespace sqr
{
	// ---- CGuiFormat ----------------------------------------------------------------------------
	CGuiFormat::CGuiFormat(void)
		: CFileFormatMgr(CCoderNameHelper::GUI_CODER, 0)
	{
		RegisterCoder(new CGUIResCoder);
	}

	CGuiFormat::~CGuiFormat(void)
	{
	}


	//---------------------------------------------------------------------------------------------
	class CSoundRefCoder : public CBaseStringCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_SOUND_REF_CODER; };
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderSoundInfo* prs = static_cast<CoderSoundInfo*>(handle);
			string tmp = prs->sf.c_str();
			_Encode(wdi, &tmp);

			return true;
		}

	private:
		bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle )
		{
			CoderSoundInfo* prs = static_cast<CoderSoundInfo*>(handle);
			prs->sf = refName.c_str();

			return true;
		}
	};

	// --------------------------------------------------------------------------------------------

	class CRsTypeCoder : public CBaseIntCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_RS_TYPE_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderSoundInfo* prs = static_cast<CoderSoundInfo*>(handle);
			int tmp = prs->tp;
			_Encode(wdi, &tmp);

			return true;
		}
		
	private:
		bool _DecodeImpl( const int& refName, PRIVATE_HANDLE handle )
		{
			CoderSoundInfo* prs = static_cast<CoderSoundInfo*>(handle);
			prs->tp = refName;

			return true;
		}
		
	};

	// ---- CGUIPerSoundProCoder ----------------------------------------------------------------------
	CGUIPerSoundProCoder::CGUIPerSoundProCoder()
	{
		// 需要解析子窗体CODER
		// 类形CODER
		RegisterCoder(new CRsTypeCoder);
		// 声音值CODER
		RegisterCoder(new CSoundRefCoder);
	}

	FourCC CGUIPerSoundProCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_PER_SOUND_CODER;
	}

	bool CGUIPerSoundProCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderSoundInfo* si = static_cast<CoderSoundInfo*>(handle);
		_Encode(wdi, si);
		return true;
	}

	bool CGUIPerSoundProCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIPerSoundProCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		// 声音属性
		WndResParam* pm = static_cast<WndResParam*>(handle);
		CoderSoundInfo si;
		_AutoDecode(inData, &si);

		if (si.tp < eMS_Count)
			pm->m_MsgSound[si.tp] = si.sf.c_str();
		
		return true;
	}

	BEGIN_VERSION_MAP(CGUIPerSoundProCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUISoundProCoder ----------------------------------------------------------------------
	CGUISoundProCoder::CGUISoundProCoder()
	{
		RegisterCoder(new CGUIPerSoundProCoder);
	}

	FourCC CGUISoundProCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_SOUND_CODER;
	}

	bool CGUISoundProCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
		// 声音属性，怎么ENCODE
		for( int i = 0; i < eMS_Count; ++i )
		{
			ESoundMsg eSoundType = static_cast<ESoundMsg>(i);
			string szSoundInfo = pWnd->GetMsgSound(eSoundType);
			if (szSoundInfo.length() > 0)
			{
				CoderSoundInfo si;
				si.tp = i;	si.sf = szSoundInfo;

				_Encode(wdi, &si);
			}
		}
		return true;
	}

	bool CGUISoundProCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUISoundProCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		// 声音属性
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUISoundProCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	//---------------------------------------------------------------------------------------------
	class CFlashRefCoder : public CBaseStringCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_FLASH_REF_CODER; };
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderFlashInfo* prs = static_cast<CoderFlashInfo*>(handle);
			string tmp = prs->fi.c_str();
			_Encode(wdi, &tmp);

			return true;
		}

	private:
		bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle )
		{
			CoderFlashInfo* prs = static_cast<CoderFlashInfo*>(handle);
			prs->fi = refName.c_str();

			return true;
		}
	};

	// --------------------------------------------------------------------------------------------
	class CFlashTypeCoder  : public CBaseIntCoder 
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_FLASH_TP_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderFlashInfo* prs = static_cast<CoderFlashInfo*>(handle);
			int tmp = prs->tp;
			_Encode(wdi, &tmp);

			return true;
		}

	private:
		bool _DecodeImpl( const int& refName, PRIVATE_HANDLE handle )
		{
			CoderFlashInfo* prs = static_cast<CoderFlashInfo*>(handle);
			prs->tp = refName;

			return true;
		}
	};


	class CSQRWndRectCoder : public CBaseCFRectCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_RECT_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			CFRect rt;
			pWnd->GetWndRect( rt );
			pWnd->GetParent()->ScreenToWnd( rt );
			_Encode(wdi, &rt);

			return true;
		}
	private:	
		bool _DecodeImpl( const CFRect& refName, PRIVATE_HANDLE handle )
		{
			CFRect rt = refName;
			WndResParam* pm = static_cast<WndResParam*>(handle);

			pm->m_x = (int)(rt.left > 0.0f ? rt.left + 0.5f : rt.left - 0.5f);
			pm->m_y = (int)(rt.top > 0.0f ? rt.top + 0.5f : rt.top - 0.5f);
			pm->m_Width = (int)(rt.Width() > 0.0f ? rt.Width() + 0.5f : rt.Width() - 0.5f);
			pm->m_Height = (int)(rt.Height() > 0.0f ? rt.Height() + 0.5f : rt.Height() - 0.5f);
			return true;
		}
	};


	class CLockFlagCoder : public CBaseUnsignedCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_LOCK_FLAG_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			unsigned  uLockFlag = pWnd->GetLockFlag();
			_Encode(wdi, &uLockFlag);

			return true;
		}

	private:
		bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle )
		{
			ChildWndInfo* info = static_cast<ChildWndInfo*>(handle);
			info->m_uLockFlag = refName;
			return true;
		}
	};


	class CWndNameCoder : public CBaseStringCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_NAME_CODER; };
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			string szWindowName = pWnd->GetszWindowName();
			_Encode(wdi, &szWindowName);

			return true;
		}

	private:
		bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle )
		{
			WndResParam* pm = static_cast<WndResParam*>(handle);

			char*   szCurBuf = NULL;
			int32 nMaxLen				= refName.length() + 1;
			szCurBuf					= (char*)SqrNew(nMaxLen, SQRGuiMallocObject::GetPool());
			if (!szCurBuf)
				return false;
			memcpy( szCurBuf, refName.c_str(), nMaxLen );
			pm->m_WindowName = szCurBuf;

			return true;
		}
	};

	
	class CStyleIndexCoder : public CBaseDWordCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_STYLE_IDX_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			DWORD StyleIndex		= pWnd->GetStyleIndex();
			_Encode(wdi, &StyleIndex);

			return true;
		}

	private:
		bool _DecodeImpl( const DWORD& refName, PRIVATE_HANDLE handle )
		{
			ChildWndInfo* info = static_cast<ChildWndInfo*>(handle);
			info->m_uStyleIndex = refName;
			return true;
		}
	};

	class CTransTexRefCoder : public CTexRefCoder
	{
	public:
		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			string transtex = pWnd->GetWndHandle()->m_strTransName.c_str();
			_Encode(wdi, &transtex);
			return true;
		}

	private:
		bool _DecodeImpl( const string& inData, OBJ_HANDLE handle )
		{
			WndResParam* pm = static_cast<WndResParam*>(handle);

			char*   szCurBuf;
			int32 nMaxLen				= inData.length() + 1;
			szCurBuf					= (char*)SqrNew(nMaxLen, SQRGuiMallocObject::GetPool());
			if (!szCurBuf)
				return false;
			memcpy( szCurBuf, inData.c_str(), nMaxLen );
			pm->m_TransTexture = szCurBuf;

			return true;
		}
	};



	// ---- CGUIPerFlashInfoCoder ----------------------------------------------------------------------
	CGUIPerFlashInfoCoder::CGUIPerFlashInfoCoder()
	{
		// 需要解析子窗体CODER
		// 类形CODER
		RegisterCoder(new CFlashTypeCoder);
		// 声音值CODER
		RegisterCoder(new CFlashRefCoder);
	}

	FourCC CGUIPerFlashInfoCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_PER_FLASH_CODER;
	}

	bool CGUIPerFlashInfoCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderFlashInfo* si = static_cast<CoderFlashInfo*>(handle);
		_Encode(wdi, si);
		return true;
	}

	bool CGUIPerFlashInfoCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIPerFlashInfoCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		// 声音属性
		WndResParam* pm = static_cast<WndResParam*>(handle);
		CoderFlashInfo si;
		_AutoDecode(inData, &si);
		EventStateMask eStFls = (EventStateMask)(si.tp);
		if (si.tp < IP_COUNT)
		{
			UIString flsname = si.fi.c_str();
			pm->m_FlashWndStr.insert(pair<EventStateMask, UIString>(eStFls, flsname));
		}
		return true;
	}

	BEGIN_VERSION_MAP(CGUIPerFlashInfoCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()



	// ---- CGUIFlashInfoCoder ----------------------------------------------------------------------
	CGUIFlashInfoCoder::CGUIFlashInfoCoder()
	{
		RegisterCoder(new CGUIPerFlashInfoCoder);
	}

	FourCC CGUIFlashInfoCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_FLASH_CODER;
	}

	bool CGUIFlashInfoCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
		///save flash property
		UIMap<EventStateMask, FlashWndInfo*> flashInfo = pWnd->GetFlashInfoMap();
		// 清空无用数据
		UIMap<EventStateMask, FlashWndInfo*>::iterator flsitr = flashInfo.begin();
		UIMap<EventStateMask, FlashWndInfo*>::iterator flsitrend = flashInfo.end();
		while (flsitr != flsitrend)
		{
			if (!(flsitr->second))
			{
				flashInfo.erase(flsitr);
				flsitr = flashInfo.begin();
				flsitrend = flashInfo.end();
				continue;
			}
			++flsitr;
		}
		// coder
		flsitr = flashInfo.begin();
		flsitrend = flashInfo.end();
		while (flsitr != flsitrend)
		{
			int st = flsitr->first;
			string szFlsInfo = flsitr->second->m_FlashName.c_str();

			CoderFlashInfo si;
			si.tp = st;	si.fi = szFlsInfo.c_str();
			_Encode(wdi, &si);

			++flsitr;
		}
		return true;
	}

	bool CGUIFlashInfoCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIFlashInfoCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIFlashInfoCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()



	// ---- CGUIPerImageProCoder ----------------------------------------------------------------------
	CGUIPerImageProCoder::CGUIPerImageProCoder()
	{
		RegisterCoder(new CGUIImgProMskOneCoder);
		RegisterCoder(new CGUIImgProMskOtherCoder);
	}

	FourCC CGUIPerImageProCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_PER_IMAGE_PRO_CODER;
	}

	bool CGUIPerImageProCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderImagePro* si = static_cast<CoderImagePro*>(handle);
		_Encode(wdi, si);
		return true;
	}

	bool CGUIPerImageProCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIPerImageProCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		ChildWndInfo Info;
		_AutoDecode(inData, &Info);

		WndResParam* pm = static_cast<WndResParam*>(handle);
		pm->m_ImageList.push_back(Info);

		return true;
	}

	BEGIN_VERSION_MAP(CGUIPerImageProCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()



	// ---- CGUIImageProCoder ----------------------------------------------------------------------
	CGUIImageProCoder::CGUIImageProCoder()
	{
		// 需要解析子窗体CODER
		RegisterCoder(new CGUIPerImageProCoder);
	}

	FourCC CGUIImageProCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_IMAGE_PRO_CODER;
	}

	bool CGUIImageProCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		SQRWnd* pWnd = static_cast<SQRWnd*>(handle);

		for( int nCtrlMask = 1; ; nCtrlMask++ )
		{
			SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
			if( !pChild )
				break;
			CoderImagePro cip;
			cip.cm = nCtrlMask;		cip.wnd = pChild;
			_Encode(wdi, &cip);
		}
		return true;
	}

	bool CGUIImageProCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIImageProCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIImageProCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUIImgProMskOneCoder ----------------------------------------------------------------------
	CGUIImgProMskOneCoder::CGUIImgProMskOneCoder()
	{
		// 需要解析子窗体CODER
		RegisterCoder(new CGUIImgProStateCoder);
	}

	FourCC CGUIImgProMskOneCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_IMAGE_PRO_ONE_CODER;
	}

	bool CGUIImgProMskOneCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderImagePro* pCWndStruct = static_cast<CoderImagePro*>(handle);
		if (pCWndStruct->cm == 1)
		{
			// 不包含缩定方式大小的 CODER
			_Encode(wdi, pCWndStruct->wnd);
		}
		return true;
	}

	bool CGUIImgProMskOneCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIImgProMskOneCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIImgProMskOneCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	class CChildRectCoder : public CBaseCFRectCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_CHILD_WND_RECT_CODER; }
		uint16	GetNewVer(void)		{ return 1; }
		
		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
			CFRect rt;
			pWnd->GetWndRect( rt );
			_Encode(wdi, &rt);

			return true;
		}
	private:	
		bool _DecodeImpl( const CFRect& refName, PRIVATE_HANDLE handle )
		{
			ChildWndInfo* info = static_cast<ChildWndInfo*>(handle);
			info->rtWnd = refName;
			return true;
		}
	};

	// ---- CGUIImgProMskOtherCoder ----------------------------------------------------------------------
	CGUIImgProMskOtherCoder::CGUIImgProMskOtherCoder()
	{
		RegisterCoder(new CChildRectCoder);
		RegisterCoder(new CStyleIndexCoder);
		RegisterCoder(new CLockFlagCoder);

		RegisterCoder(new CGUIImgProStateCoder);
	}

	FourCC CGUIImgProMskOtherCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_IMAGE_PRO_OTHER_CODER;
	}

	bool CGUIImgProMskOtherCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderImagePro* pCWndStruct = static_cast<CoderImagePro*>(handle);
		if (pCWndStruct->cm > 1)
		{
			_Encode(wdi, pCWndStruct->wnd);
		}
		return true;
	}

	bool CGUIImgProMskOtherCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIImgProMskOtherCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIImgProMskOtherCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	class CStateColorCoder : public CBaseUnsignedCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_STATE_COLOR_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderImageState* pCIS = static_cast<CoderImageState*>(handle);
			unsigned nStateColor = pCIS->wnd->GetWndTextColor( pCIS->ip );
			_Encode(wdi, &nStateColor);

			return true;
		}

	private:
		bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle )
		{
			ChildImageList* info = static_cast<ChildImageList*>(handle);
			info->uTextColor = refName;
			return true;
		}
	};

	class CRimColorCoder : public CBaseUnsignedCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_RIM_COLOR_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderImageState* pCIS = static_cast<CoderImageState*>(handle);
			unsigned uRimColor = pCIS->wnd->GetWndTextBcColor( pCIS->ip );
			_Encode(wdi, &uRimColor);

			return true;
		}

	private:
		bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle )
		{
			ChildImageList* info = static_cast<ChildImageList*>(handle);
			info->uRimColor = refName;
			return true;
		}
	};

	class CStateMaskCoder : public CBaseIntCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_STATE_MASK_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CoderImageState* pCIS = static_cast<CoderImageState*>(handle);
			int nStateMask = pCIS->ip.StateMask;
			_Encode(wdi, &nStateMask);

			return true;
		}

	private:
		bool _DecodeImpl( const int& refName, PRIVATE_HANDLE handle )
		{
			ChildImageList* info = static_cast<ChildImageList*>(handle);
			info->nStateMask = refName;
			return true;
		}
	};


	// ---- CGUIImgProStateCoder ----------------------------------------------------------------------
	CGUIImgProStateCoder::CGUIImgProStateCoder()
	{
		// 需要解析子窗体CODER
		RegisterCoder(new CStateMaskCoder);
		RegisterCoder(new CStateColorCoder);
		RegisterCoder(new CRimColorCoder);
		
		RegisterCoder(new CGUIImgTextureCoder);
	}

	FourCC CGUIImgProStateCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_IMAGE_PRO_State_CODER;
	}

	bool CGUIImgProStateCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		SQRWnd* pChild = static_cast<SQRWnd*>(handle);

		IMAGE_PARAM IP ( 1, 0 );
		for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
		{
			WND_IMAGE_LIST* pWI = pChild->GetWndBkImage( IP );
			if( pWI )
			{
				CoderImageState cis;
				cis.ip = IP;	cis.wil = pWI;	cis.wnd = pChild;
				_Encode(wdi, &cis);
			}
		}
		return true;
	}

	bool CGUIImgProStateCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIImgProStateCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		ChildWndInfo* info = static_cast<ChildWndInfo*>(handle);
		
		ChildImageList ImgList;
		_AutoDecode(inData, &ImgList);
		info->ChildBkList.push_back(ImgList);

		return true;
	}

	BEGIN_VERSION_MAP(CGUIImgProStateCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	class CImageTexRefCoder : public CTexRefCoder
	{
	public:
		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			IMAGE* Img = static_cast<IMAGE*>(handle);
			string	TextName  = gbk_to_utf8(Img->GetTexName());
			slash_to_backslash(TextName);
			_Encode(wdi, &TextName);
			return true;
		}

	private:
		bool _DecodeImpl( const string& inData, OBJ_HANDLE handle )
		{
			WindowImage* Img = static_cast<WindowImage*>(handle);
			Img->szTexture = inData.c_str();

			return true;
		}
	};

	class CTextureRectCoder : public CBaseCFRectCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_TEXTURE_RECT_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			IMAGE* Img = static_cast<IMAGE*>(handle);
			_Encode(wdi, &Img->rtTexture);

			return true;
		}

	private:
		bool _DecodeImpl( const CFRect& refName, PRIVATE_HANDLE handle )
		{
			WindowImage* Img = static_cast<WindowImage*>(handle);
			Img->rtScale = refName;

			return true;
		}
	};

	class CTexturePosCoder : public CBaseCFPosCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_TEXTURE_POS_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			IMAGE* Img = static_cast<IMAGE*>(handle);
			_Encode(wdi, &Img->posWnd);

			return true;
		}

	private:
		bool _DecodeImpl( const CFPos& refName, PRIVATE_HANDLE handle )
		{
			WindowImage* Img = static_cast<WindowImage*>(handle);
			Img->posWnd = refName;

			return true;
		}
	};

	class CTextureDWColorCoder : public CBaseUnsignedCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_TEXTURE_DWCOLOR_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			IMAGE* Img = static_cast<IMAGE*>(handle);
			_Encode(wdi, &Img->dwColor);

			return true;
		}

	private:
		bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle )
		{
			WindowImage* Img = static_cast<WindowImage*>(handle);
			Img->dwColor = refName;

			return true;
		}
	};

	class CTextureDWLockFlagCoder : public CBaseUnsignedCoder
	{
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::GUI_WND_TEXTURE_DWLOCKFLAG_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			IMAGE* Img = static_cast<IMAGE*>(handle);
			_Encode(wdi, &Img->dwLockFlag);

			return true;
		}

	private:
		bool _DecodeImpl( const unsigned& refName, PRIVATE_HANDLE handle )
		{
			// 等待实现
			WindowImage* Img = static_cast<WindowImage*>(handle);
			Img->dwLockFlage = refName;

			return true;
		}
	};


	// ---- CGUIImgTextureCoder ----------------------------------------------------------------------
	CGUIImgTextureCoder::CGUIImgTextureCoder()
	{
		// 需要解析子窗体CODER
		RegisterCoder(new CTextureRectCoder);
		RegisterCoder(new CTexturePosCoder);
		RegisterCoder(new CTextureDWColorCoder);
		RegisterCoder(new CTextureDWLockFlagCoder);
		RegisterCoder(new CImageTexRefCoder);
	}

	FourCC CGUIImgTextureCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_IMAGE_TEXTURE_CODER;
	}

	bool CGUIImgTextureCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CoderImageState* pcis = static_cast<CoderImageState*>(handle);
		int nImageCount = pcis->wil->GetImageCount();
		for ( int i = 0; i < nImageCount; i++ )
		{
			IMAGE& Image = pcis->wil->GetImage(i);
			_Encode(wdi, &Image);
		}

		return true;
	}

	bool CGUIImgTextureCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIImgTextureCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		ChildImageList* ImageList = static_cast<ChildImageList*>(handle);

		WindowImage wImg;
		_AutoDecode(inData, &wImg);

		ImageList->ImageList.push_back(wImg);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIImgTextureCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUIChildWndCoder ----------------------------------------------------------------------
	CGUIChildWndCoder::CGUIChildWndCoder()
	{
		// 需要解析子窗体CODER
		RegisterCoder(new CWndNameCoder);
		RegisterCoder(new CGUISoundProCoder);
		RegisterCoder(new CGUIFlashInfoCoder);

		RegisterCoder(new CSQRWndRectCoder);
		RegisterCoder(new CGUIWndsDefProCoder);

		RegisterCoder(new CTransTexRefCoder);
		RegisterCoder(new CGUIImageProCoder);
	}

	FourCC CGUIChildWndCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_CHILD_CODER;
	}

	bool CGUIChildWndCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		// 子窗体的MAP
		SQRWnd* pParentWnd = static_cast<SQRWnd*>(handle);
		// 遍历一遍
		_Encode(wdi, handle);
		return true;
	}

	bool CGUIChildWndCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIChildWndCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIChildWndCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()

	// ---- CGUIWndsMapCoder ----------------------------------------------------------------------
	CGUIWndsMapCoder::CGUIWndsMapCoder()
	{
		// 解析子窗体
		RegisterCoder(new CGUIChildWndCoder);
	}

	FourCC CGUIWndsMapCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_ITEM_MAP_CODER;
	}

	bool CGUIWndsMapCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		// 子窗体的MAP
		SQRWnd* pParentWnd = static_cast<SQRWnd*>(handle);
		// 遍历一遍
		SQRWnd* ChildWnd = pParentWnd->GetWndHandle()->m_LastChild;
		while ( ChildWnd )
		{
			_Encode(wdi, ChildWnd);
			ChildWnd = ChildWnd->GetWndHandle()->m_PreBrother;
		}
		return true;
	}

	bool CGUIWndsMapCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIWndsMapCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		WndResParam* MainWndParam = static_cast<WndResParam*>(handle);

		WndResParam* ChildWndParam = new WndResParam;
		_AutoDecode(inData, ChildWndParam);

		// 根据名字去填冲
		UIMap<UIString, WndResParam*>::iterator it = MainWndParam->m_Child.find(ChildWndParam->m_WindowName);
		if (it!=MainWndParam->m_Child.end())
			delete it->second;
		MainWndParam->m_Child[ChildWndParam->m_WindowName] = ChildWndParam;

		return true;
	}

	BEGIN_VERSION_MAP(CGUIWndsMapCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUIWndsDefProCoder ----------------------------------------------------------------------
	CGUIWndsDefProCoder::CGUIWndsDefProCoder()
	{
	}

	FourCC CGUIWndsDefProCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_DEF_PRO_CODER;
	}

	bool CGUIWndsDefProCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		// 子窗体的MAP
		SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
		if ( pWnd )
			_Encode(wdi, pWnd);
		return true;
	}

	bool CGUIWndsDefProCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		SQRWnd* pWnd = static_cast<SQRWnd*>(handle);
		// 编码
		int		ClassLen		= (int)strlen( pWnd->GetClassName() );
		LPCSTR  szClass			= pWnd->GetClassName();
		int		WndTextLen		= (int)strlen( pWnd->GetWndText() );
		LPCSTR	szWndText		= pWnd->GetWndText();
		DWORD	wsStyle			= pWnd->GetStyle();

		DWORD	GroupID			= pWnd->GetGroupID();
		int32	nFontID			= pWnd->GetFontID();
		int		nFontSize		= (int)(pWnd->GetFontSize()/CFontCfg::GetInst()->GetFontZoom() + 0.5f);
		bool	bBold			= pWnd->IsFontBold();
		unsigned  uLockFlag     = pWnd->GetLockFlag();
		int     nDescLen		= (int)strlen( pWnd->GetMouseOverDesc() );
		LPCSTR  szDescInfo		= pWnd->GetMouseOverDesc();

		string  use_class_utf8	= pWnd->GetszClassName();
		int     nUseClassLen	= (int)use_class_utf8.length();
		LPCSTR  szUseClass		= use_class_utf8.c_str();

		string  use_info_utf8	= pWnd->GetszCreateInfo();
		int     nUseInfLen		= (int)use_info_utf8.length();
		LPCSTR  szUseInf		= use_info_utf8.c_str();

		int     nClickFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Click) );
		LPCSTR  szClickFxInfo	= pWnd->GetMsgFx(eMF_Click);
		int     nOpenFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Open) );
		LPCSTR  szOpenFxInfo	= pWnd->GetMsgFx(eMF_Open);
		int     nCloseFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Close) );
		LPCSTR  szCloseFxInfo	= pWnd->GetMsgFx(eMF_Close);
		int		SourceLen		= (int)strlen( pWnd->GetszSourceName() );
		LPCSTR  szSource		= pWnd->GetszSourceName();//add by guo

		DWORD fontGradualColor	= pWnd->GetFontGradualColor();
		DWORD fontEffectMask	= pWnd->GetFontEffectMask();
		bool  bShow				= pWnd->IsDrawBackground();
		DWORD StyleIndex		= pWnd->GetStyleIndex();

		wdi.Write( (char*)&ClassLen, sizeof(int) );
		wdi.Write( (char*)szClass, ClassLen );
		wdi.Write( (char*)&WndTextLen, sizeof(int) );
		wdi.Write( (char*)szWndText, WndTextLen );
		wdi.Write( (char*)&wsStyle, sizeof(DWORD) );
		wdi.Write( (char*)&GroupID, sizeof(DWORD) );

		wdi.Write( (char*)&nFontID, sizeof(int32) );
		wdi.Write( (char*)&bBold, sizeof(bool) );
		wdi.Write( (char*)&nFontSize, sizeof(int) );
		wdi.Write(	(char*)&fontGradualColor, sizeof(DWORD));		 
		wdi.Write(	(char*)&fontEffectMask, sizeof(DWORD));

		wdi.Write(	(char*)&bShow, sizeof(bool));
		wdi.Write( (char*)&StyleIndex, sizeof(DWORD) );
		wdi.Write( (char*)&uLockFlag, sizeof(unsigned) );

		wdi.Write( (char*)&nDescLen, sizeof(int) );
		wdi.Write( (char*)szDescInfo, nDescLen );
		wdi.Write( (char*)&nUseClassLen, sizeof(int) );
		wdi.Write( (char*)szUseClass, nUseClassLen );
		wdi.Write( (char*)&nUseInfLen, sizeof(int) );
		wdi.Write( (char*)szUseInf, nUseInfLen );

		wdi.Write( (char*)&nClickFxLen, sizeof(int) );
		wdi.Write( (char*)szClickFxInfo, nClickFxLen );
		wdi.Write( (char*)&nOpenFxLen, sizeof(int) );
		wdi.Write( (char*)szOpenFxInfo, nOpenFxLen );
		wdi.Write( (char*)&nCloseFxLen, sizeof(int) );
		wdi.Write( (char*)szCloseFxInfo, nCloseFxLen );
		wdi.Write( (char*)&SourceLen, sizeof(int) );//add by guo
		wdi.Write( (char*)szSource, SourceLen );
		return true;
	}

	bool CGUIWndsDefProCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		WndResParam* WndParam = static_cast<WndResParam*>(handle);

		char	szBuf[4096];	char*   szCurBuf = szBuf;	int32	ClassLen;
		int32	WndTextLen;		int32   nDescLen;			int32	SourceLen = 0;//add by guo
		int32   nUseClassLen;	int32   nUseInfLen;			int32   nClickFxLen;
		int32   nOpenFxLen;		int32   nCloseFxLen;		uint32	wsStyle;
		uint32	GroupID;		int32	nFontID;			bool	bBoldFont = false;
		int32	nFontSize;		uint32  uLockFlag;			uint32	fontGradualColor;
		uint32	fontEffectMask; bool	bisShowBack;		uint32	StyleIndex;
				

		memset( szBuf, 0, 4096 );
		int32	remainLength = 4096;

		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		bufFile.SafeRead( (char*)&ClassLen, sizeof(int32) );
		string_size_check(ClassLen, remainLength);
		remainLength -=ClassLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, ClassLen );
		szCurBuf += ClassLen + 1;
		bufFile.SafeRead( (char*)&WndTextLen, sizeof(int32) );
		string_size_check(WndTextLen, remainLength);
		remainLength -=WndTextLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, WndTextLen );
		szCurBuf += WndTextLen + 1;
		bufFile.SafeRead( (char*)&wsStyle, sizeof(uint32) );

		bufFile.SafeRead( (char*)&GroupID, sizeof(uint32) );

		bufFile.SafeRead( (char*)&nFontID, sizeof(int32) );
		bufFile.SafeRead( (char*)&bBoldFont, sizeof(bool) );
		bufFile.SafeRead( (char*)&nFontSize, sizeof(int32) );

		bufFile.SafeRead( (char*)&fontGradualColor, sizeof(uint32)); 
		bufFile.SafeRead( (char*)&fontEffectMask, sizeof(uint32));
		bufFile.SafeRead( (char*)&bisShowBack, sizeof(bool));
		bufFile.SafeRead( (char*)&StyleIndex, sizeof(uint32));


		bufFile.SafeRead( (char*)&uLockFlag, sizeof(uint32) );

		bufFile.SafeRead( (char*)&nDescLen, sizeof(int32) );
		string_size_check(nDescLen, remainLength);
		remainLength -=nDescLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nDescLen );
		szCurBuf += nDescLen + 1;
		bufFile.SafeRead( (char*)&nUseClassLen, sizeof(int32) );
		string_size_check(nUseClassLen, remainLength);
		remainLength -=nUseClassLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nUseClassLen );
		szCurBuf += nUseClassLen + 1;
		bufFile.SafeRead( (char*)&nUseInfLen, sizeof(int32) );
		string_size_check(nUseInfLen, remainLength);
		remainLength -=nUseInfLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nUseInfLen );
		szCurBuf += nUseInfLen + 1;
		bufFile.SafeRead( (char*)&nClickFxLen, sizeof(int32) );
		string_size_check(nClickFxLen, remainLength);
		remainLength -=nClickFxLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nClickFxLen );
		szCurBuf += nClickFxLen + 1;
		bufFile.SafeRead( (char*)&nOpenFxLen, sizeof(int32) );
		string_size_check(nOpenFxLen, remainLength);
		remainLength -=nOpenFxLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nOpenFxLen );
		szCurBuf += nOpenFxLen + 1;
		bufFile.SafeRead( (char*)&nCloseFxLen, sizeof(int32) );
		string_size_check(nCloseFxLen, remainLength);
		remainLength -=nCloseFxLen + 1;

		bufFile.SafeRead( (char*)szCurBuf, nCloseFxLen );
		szCurBuf += nCloseFxLen + 1;
		bufFile.SafeRead( (char*)&SourceLen, sizeof(int32) );//add by guo
		string_size_check(SourceLen, 1024);
		bufFile.SafeRead( (char*)szCurBuf, SourceLen );
		szCurBuf += SourceLen + 1;

		int32 nMaxLen				= szCurBuf - szBuf;
		szCurBuf					= (char*)SqrNew(nMaxLen, SQRGuiMallocObject::GetPool());
		memcpy( szCurBuf, szBuf, nMaxLen );

		WndParam->m_uFontGradualColor	= fontGradualColor;
		WndParam->m_uFontEffectMask = fontEffectMask;

		WndParam->m_ClassName		= szCurBuf;
		szCurBuf += ClassLen + 1;
		WndParam->m_WindowTitle		= szCurBuf;
		szCurBuf += WndTextLen + 1;
		WndParam->m_WndStyle			= wsStyle;
		WndParam->m_GroupID			= GroupID;

		WndParam->m_nFontID			= nFontID;
		WndParam->m_bBold			= bBoldFont;
		WndParam->m_FontSize			= nFontSize;

		WndParam->m_uLockFlage		= uLockFlag;
		WndParam->m_isShowBackground = bisShowBack;
		WndParam->m_uStyleIndex		= StyleIndex;

		WndParam->m_szDesc			= szCurBuf;
		szCurBuf += nDescLen + 1;
		WndParam->m_UseClassName		= szCurBuf;
		szCurBuf += nUseClassLen + 1;
		WndParam->m_UseInfo			= szCurBuf;
		szCurBuf += nUseInfLen + 1;
		WndParam->m_MsgFx[eMF_Click]	= szCurBuf;
		szCurBuf += nClickFxLen + 1;
		WndParam->m_MsgFx[eMF_Open]	= szCurBuf;
		szCurBuf += nOpenFxLen + 1;
		WndParam->m_MsgFx[eMF_Close]	= szCurBuf;
		szCurBuf += nCloseFxLen + 1;
		WndParam->m_SourceName = szCurBuf;//add by guo
		szCurBuf += SourceLen + 1;

		return true;
	}

	BEGIN_VERSION_MAP(CGUIWndsDefProCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUIWndItemCoder ----------------------------------------------------------------------
	CGUIWndItemCoder::CGUIWndItemCoder()
	{
		// 一些属性的CODER
		RegisterCoder(new CWndNameCoder);
		RegisterCoder(new CGUISoundProCoder);
		RegisterCoder(new CGUIFlashInfoCoder);

		RegisterCoder(new CSQRWndRectCoder);
		RegisterCoder(new CGUIWndsDefProCoder);

		RegisterCoder(new CTransTexRefCoder);
		RegisterCoder(new CGUIImageProCoder);

		// 子窗体MAP的CODER
		RegisterCoder(new CGUIWndsMapCoder);
	}

	FourCC CGUIWndItemCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_WND_ITEM_CODER;
	}

	bool CGUIWndItemCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		_Encode(wdi, handle);
		return true;
	}

	bool CGUIWndItemCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIWndItemCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIWndItemCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()


	// ---- CGUIResCoder ----------------------------------------------------------------------
	// 貌似写的多包了一层
	CGUIResCoder::CGUIResCoder()
	{
		RegisterCoder(new CGUIWndItemCoder);
	}

	FourCC CGUIResCoder::GetChunkName()
	{
		return CCoderNameHelper::GUI_RES_CODER;
	}

	bool CGUIResCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		_Encode(wdi, handle);
		return true;
	}

	bool CGUIResCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CGUIResCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CGUIResCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()
}
