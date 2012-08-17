#include "stdafx.h"
#include "CGraphic.h"
#include "CWinPkgFile.h"
#include <streams.h>
#include "Casyncio.h"
#include "Casyncrdr.h"
#include "Cmemdef.h"
#include "CMoviehelper.h"
#include "CBaseMovieImp.h"
#include "CDShowmovieImp.h"
#include "BaseHelper.h"


namespace sqr
{
	//得到Filter未连接的Pin接口
	HRESULT GetUnconnectedPin(   
		IBaseFilter *pFilter,   // filter指针   
		PIN_DIRECTION PinDir,   // Pin的属性，类型为Output或Input  
		IPin **ppPin)           // 返回的pin接口   
	{   
		*ppPin = 0;   
		IEnumPins *pEnum = 0;   
		IPin *pPin = 0;   
		HRESULT hr = pFilter->EnumPins(&pEnum);   
		if (FAILED(hr))   
		{   
			return hr;   
		}   
		while (pEnum->Next(1, &pPin, NULL) == S_OK)   
		{   
			PIN_DIRECTION ThisPinDir;   
			pPin->QueryDirection(&ThisPinDir);   
			if (ThisPinDir == PinDir)   
			{   
				IPin *pTmp = 0;   
				hr = pPin->ConnectedTo(&pTmp);   
				if (SUCCEEDED(hr))  // 已经连接过的，不是我们想要的pin  
				{   
					pTmp->Release();   
				}   
				else  // 没有连接，是我们想要的 
				{   
					pEnum->Release();   
					*ppPin = pPin;   
					return S_OK;   
				}   
			}   
			pPin->Release();   
		}   
		pEnum->Release();   
		// 没有找到匹配的pin  
		return E_FAIL;   
	}   

	// 断开filter的所有连接  
	HRESULT DisconnectPins(IBaseFilter *pFilter)   
	{   
		IEnumPins *pEnum = 0;   
		IPin *pPin = 0;   
		HRESULT hr = pFilter->EnumPins(&pEnum);   
		if (FAILED(hr))   
		{   
			return hr;   
		}   

		while (pEnum->Next(1, &pPin, NULL) == S_OK)   
		{   
			pPin->Disconnect();   
			pPin->Release();   
		}   
		pEnum->Release();   

		// 没有找到匹配的pin  
		return S_OK;   
	}   

	//指定一个outputPin和目标filter InputPin连接
	HRESULT ConnectFilters(   
		IGraphBuilder *pGraph, // Filter管理器   
		IPin *pOut,            // OutputPin  
		IBaseFilter *pDest)    // 目标Filter   
	{   
		if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))   
		{   
			return E_POINTER;   
		}   
#ifdef debug   
		PIN_DIRECTION PinDir;   
		pOut->QueryDirection(&PinDir);   
		_ASSERTE(PinDir == PINDIR_OUTPUT);   
#endif   

		// 找到下一级Filter的InputPin.   
		IPin *pIn = 0;   
		HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);   
		if (FAILED(hr))   
		{   
			return hr;   
		}   
		// 试着连接它们   
		hr = pGraph->Connect(pOut, pIn);   
		pIn->Release();   
		return hr;   
	}   



	// 连接两个filter  
	HRESULT ConnectFilters(   
		IGraphBuilder *pGraph,    
		IBaseFilter *pSrc,    
		IBaseFilter *pDest)   
	{   
		if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))   
		{   
			return E_POINTER;   
		}   

		// 找到源Filter的OutputPin   
		IPin *pOut = 0;   
		HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);   
		if (FAILED(hr))    
		{   
			return hr;   
		}   
		hr = ConnectFilters(pGraph, pOut, pDest);   
		pOut->Release();   
		return hr;   
	} 

	WCHAR* util_convertCStringToWString(const char* string)
	{
		const int MAX_STRINGZ=500;
		static WCHAR wtext[MAX_STRINGZ+2];

		if (strlen(string)>MAX_STRINGZ)
			return 0;

		// convert text to wchar
		if (MultiByteToWideChar(
			CP_ACP,// ansi code page
			0,// flags
			string,// orig string
			-1,// calculate len
			wtext,// where to put the string
			MAX_STRINGZ)// maximum allowed path
			==0)
		{
			throw("[DSHOW] convertCStringToWString failed with no extra error info");
		}

		return wtext;
	}

	CDShowMovieImp::CDShowMovieImp()
		:CBaseMovieImp()
	{
		// 1) 创建 DSDATA
		dsdata=new DirectShowData;

		// 2) 初始化 DIRECT SHOW
		dsdata->pGraph=0;
		dsdata->pControl=0;
		dsdata->pEvent=0;
		dsdata->pSeeking=0;
		dsdata->pWindow=0;
		dsdata->pAviSpliter=0;
		dsdata->pSrcFilter=0;
		dsdata->pGrabberF=0;
		dsdata->pGrabber=0;

		// 3)初始化com
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		m_pMemStream = NULL;
		m_strPreName = "";
		mHorizontalMirroring = false;
		m_pTexture = NULL;
	}

	CDShowMovieImp::~CDShowMovieImp()
	{
		SQR_TRY
		{
			// 1) 卸载视频
			UnloadMovie();
			//  结束com
			CoUninitialize();
			// 3) 删除 DSDATA
			delete dsdata;
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("CDShowMovieImp析构异常！", true);
			GfkLogExp(exp);
		}
		SQR_TRY_END
	}

	HRESULT CDShowMovieImp::LoadMemMovie( const char* name )
	{
		HRESULT hr = S_OK;
		
		std::string _name = name;

		if (m_strPreName == _name)
		{
			return S_OK;
		}
		else
		{
			m_strPreName = _name;
		}
;
		// 销毁上一次播放的视频
		UnloadMovie();
		CleanTextureContents();

		if (!_name.length())
		{
			return S_FALSE;
		}

		CMediaType mt;
		mt.majortype = MEDIATYPE_Stream;
		mt.subtype = MEDIASUBTYPE_Avi;
		mt.formattype = FORMAT_VideoInfo;

		m_pMemStream = new CMemStream(_name, INFINITE);

		CMemReader *rdr = new CMemReader(m_pMemStream, &mt, &hr);
		if(FAILED(hr) || rdr == NULL)
		{ 
			delete rdr;
			_tprintf(_T("Could not create filter - HRESULT 0x%8.8X\n"), hr);
			CoUninitialize();
			return S_FALSE;
		}

		// 源码级别的com初始化
		rdr->AddRef();

		dsdata->pSrcFilter = rdr;

		IBaseFilter* render;

		//Filter管理器
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IFilterGraph, (void**) &dsdata->pGraph);
		if(FAILED(hr))
		{
			return hr;
		}
		//avi分割器
		hr = CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC, 
			IID_IBaseFilter, (void**) &dsdata->pAviSpliter);
		if(FAILED(hr))
		{
			return hr;
		}
		//初始化视频信息抓取器
		hr=CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void**)&dsdata->pGrabberF);
		if(FAILED(hr))
		{
			return hr;
		}
		//创建渲染器
		hr=CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void**)&render);
		if(FAILED(hr))
		{
			return hr;
		}
		//得到一个接口
		dsdata->pGrabberF->QueryInterface(IID_ISampleGrabber,
			(void**)&dsdata->pGrabber);

		AM_MEDIA_TYPE mt1;
		ZeroMemory(&mt1, sizeof(AM_MEDIA_TYPE));
		mt1.majortype = MEDIATYPE_Video;
		mt1.subtype = MEDIASUBTYPE_RGB24;
		mt1.formattype = FORMAT_VideoInfo;

		hr=dsdata->pGrabber->SetMediaType(&mt1);
		if(FAILED(hr))
		{
			return hr;
		}
		//添加Filter
		hr = dsdata->pGraph->AddFilter(dsdata->pSrcFilter, L"Source, Filter");
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(dsdata->pAviSpliter, L"Avi Spliter");
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(dsdata->pGrabberF, L"Sample Grabber");
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(render, L"Render");
		if(FAILED(hr))
		{
			return hr;
		}

		IPin *OutPin = NULL;
		//MemFilter连接到AviSpliter
		hr = ConnectFilters(dsdata->pGraph, dsdata->pSrcFilter, dsdata->pAviSpliter);
		if(FAILED(hr))
		{
			return hr;
		}
		//AviSpliter连接到pGrabberF
		hr = ConnectFilters(dsdata->pGraph, dsdata->pAviSpliter, dsdata->pGrabberF); 
		if(FAILED(hr))
		{
			return hr;
		}
		//得到pGrabberF的OutPin
		hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, render);
		if(FAILED(hr))
		{
			return hr;
		}
		//得到pAviSpliter的OutPin
		hr = GetUnconnectedPin(dsdata->pAviSpliter, PINDIR_OUTPUT, &OutPin);
		//if(FAILED(hr))//不存在音频也可以正常播放
		//{
		//	return hr;
		//}
		//渲染音频线路，pGraph自动处理后续连接
		hr = dsdata->pGraph->Render(OutPin);
		//得到托管窗口接口
		hr=dsdata->pGraph->QueryInterface(IID_IVideoWindow, (void**) &dsdata->pWindow);
		if(FAILED(hr))
		{
			return hr;
		}
		//隐藏窗口
		dsdata->pWindow->put_AutoShow(OAFALSE);
		//得到媒体控制器
		hr = dsdata->pGraph->QueryInterface(IID_IMediaControl, (void **)&dsdata->pControl);
		if(FAILED(hr))
		{
			dsdata->pControl->Release();
			return hr;
		}
		//得到媒体事件
		hr = dsdata->pGraph->QueryInterface(IID_IMediaEvent, (void **)&dsdata->pEvent);
		if(FAILED(hr))
		{
			dsdata->pEvent->Release();
			return hr;
		}

		hr = dsdata->pGraph->QueryInterface(IID_IMediaSeeking, (void **)&dsdata->pSeeking);
		if(FAILED(hr))
		{
			dsdata->pEvent->Release();
			return hr;
		}

		AM_MEDIA_TYPE mtt;
		hr=dsdata->pGrabber->GetConnectedMediaType(&mtt);
		if(FAILED(hr))
		{
			return hr;
		}
		//得到视频的宽度、高度	
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*) mtt.pbFormat;
		dsdata->videoWidth=vih->bmiHeader.biWidth;
		dsdata->videoHeight=vih->bmiHeader.biHeight;
		// microsoft's help version of free media type
		if (mtt.cbFormat != 0)
		{
			CoTaskMemFree((PVOID)mtt.pbFormat);
			mtt.cbFormat = 0;
			mtt.pbFormat = NULL;
		}
		if (mtt.pUnk != NULL)
		{
			mtt.pUnk->Release();
			mtt.pUnk = NULL;
		}

		dsdata->pGrabber->SetOneShot(FALSE);
		dsdata->pGrabber->SetBufferSamples(TRUE);

		//根据视频大小自动计算ITexture大小
		int twoSquared;
		mTexWidth=0; mTexHeight=0;
		for (twoSquared=2; mTexWidth==0 || mTexHeight==0; twoSquared*=2)
		{
			if (mTexWidth==0 && twoSquared>=dsdata->videoWidth)
				mTexWidth=twoSquared;
			if (mTexHeight==0 && twoSquared>=dsdata->videoHeight)
				mTexHeight=twoSquared;
		}

		//创建纹理
		CGraphic::GetInst()->CreateEmptyTexture( mTexWidth,mTexHeight,1, TF_ARGB32,&m_pTexture );

		return S_OK;
	}

	HRESULT CDShowMovieImp::LoadUrlMovie( const char* name )
	{
		HRESULT hr = S_OK;

		std::string _name = name;

		if (m_strPreName == _name)
		{
			return S_OK;
		}
		else
		{
			m_strPreName = _name;
		}

		// 销毁上一次播放的视频
		UnloadMovie();

		if (!_name.length())
		{
			return S_FALSE;
		}

		IBaseFilter* render;

		//Filter管理器
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IFilterGraph, (void**) &dsdata->pGraph);
		if(FAILED(hr))
		{
			return hr;
		}
		//avi分割器
		hr = CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC, 
			IID_IBaseFilter, (void**) &dsdata->pAviSpliter);
		if(FAILED(hr))
		{
			return hr;
		}
		//初始化视频信息抓取器
		hr=CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void**)&dsdata->pGrabberF);
		if(FAILED(hr))
		{
			return hr;
		}
		//创建渲染器
		hr=CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void**)&render);
		if(FAILED(hr))
		{
			return hr;
		}

		//得到一个接口
		dsdata->pGrabberF->QueryInterface(IID_ISampleGrabber,
			(void**)&dsdata->pGrabber);

		AM_MEDIA_TYPE mt1;
		ZeroMemory(&mt1, sizeof(AM_MEDIA_TYPE));
		mt1.majortype = MEDIATYPE_Video;
		mt1.subtype = MEDIASUBTYPE_RGB24;
		mt1.formattype = FORMAT_VideoInfo;

		hr=dsdata->pGrabber->SetMediaType(&mt1);
		if(FAILED(hr))
		{
			return hr;
		}
		//添加Filter
		WCHAR* wname = util_convertCStringToWString(_name.c_str());
		hr = dsdata->pGraph->AddSourceFilter(wname, L"Source, Filter", &dsdata->pSrcFilter);
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(dsdata->pAviSpliter, L"Avi Spliter");
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(dsdata->pGrabberF, L"Sample Grabber");
		if(FAILED(hr))
		{
			return hr;
		}

		hr = dsdata->pGraph->AddFilter(render, L"Render");
		if(FAILED(hr))
		{
			return hr;
		}

		IPin *OutPin = NULL;
		//MemFilter连接到AviSpliter
		hr = ConnectFilters(dsdata->pGraph, dsdata->pSrcFilter, dsdata->pAviSpliter);
		if(FAILED(hr))
		{
			return hr;
		}
		//AviSpliter连接到pGrabberF
		hr = ConnectFilters(dsdata->pGraph, dsdata->pAviSpliter, dsdata->pGrabberF); 
		if(FAILED(hr))
		{
			return hr;
		}
		//pGrabberF连接到render
		hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, render); 
		if(FAILED(hr))
		{
			return hr;
		}
		//得到pGrabberF的OutPin
		//hr = GetUnconnectedPin(dsdata->pGrabberF, PINDIR_OUTPUT, &OutPin);
		//if(FAILED(hr))
		//{
		//	return hr;
		//}
		//渲染视频线路，pGraph自动处理后续连接
		//hr = dsdata->pGraph->Render(OutPin);
		//if(FAILED(hr))
		//{
		//	return hr;
		//}
		//得到pAviSpliter的OutPin
		hr = GetUnconnectedPin(dsdata->pAviSpliter, PINDIR_OUTPUT, &OutPin);
		if(FAILED(hr))
		{
			return hr;
		}
		//渲染音频线路，pGraph自动处理后续连接
		hr = dsdata->pGraph->Render(OutPin);
		if(FAILED(hr))
		{
			return hr;
		}
		//得到托管窗口接口
		hr=dsdata->pGraph->QueryInterface(IID_IVideoWindow, (void**) &dsdata->pWindow);
		if(FAILED(hr))
		{
			return hr;
		}
		//隐藏窗口
		dsdata->pWindow->put_AutoShow(OAFALSE);
		//得到媒体控制器
		hr = dsdata->pGraph->QueryInterface(IID_IMediaControl, (void **)&dsdata->pControl);
		if(FAILED(hr))
		{
			dsdata->pControl->Release();
			return hr;
		}
		//得到媒体事件
		hr = dsdata->pGraph->QueryInterface(IID_IMediaEvent, (void **)&dsdata->pEvent);
		if(FAILED(hr))
		{
			dsdata->pEvent->Release();
			return hr;
		}

		hr = dsdata->pGraph->QueryInterface(IID_IMediaSeeking, (void **)&dsdata->pSeeking);
		if(FAILED(hr))
		{
			dsdata->pEvent->Release();
			return hr;
		}

		AM_MEDIA_TYPE mtt;
		hr=dsdata->pGrabber->GetConnectedMediaType(&mtt);
		if(FAILED(hr))
		{
			return hr;
		}
		//得到视频的宽度、高度	
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*) mtt.pbFormat;
		dsdata->videoWidth=vih->bmiHeader.biWidth;
		dsdata->videoHeight=vih->bmiHeader.biHeight;
		// microsoft's help version of free media type
		if (mtt.cbFormat != 0)
		{
			CoTaskMemFree((PVOID)mtt.pbFormat);
			mtt.cbFormat = 0;
			mtt.pbFormat = NULL;
		}
		if (mtt.pUnk != NULL)
		{
			mtt.pUnk->Release();
			mtt.pUnk = NULL;
		}

		dsdata->pGrabber->SetOneShot(FALSE);
		dsdata->pGrabber->SetBufferSamples(TRUE);

		//根据视频大小自动计算ITexture大小
		int twoSquared;
		mTexWidth=0; mTexHeight=0;
		for (twoSquared=2; mTexWidth==0 || mTexHeight==0; twoSquared*=2)
		{
			if (mTexWidth==0 && twoSquared>=dsdata->videoWidth)
				mTexWidth=twoSquared;
			if (mTexHeight==0 && twoSquared>=dsdata->videoHeight)
				mTexHeight=twoSquared;
		}

		//创建纹理
		CGraphic::GetInst()->CreateEmptyTexture( mTexWidth,mTexHeight,1, TF_ARGB32,&m_pTexture );

		return hr;
	}

	void CDShowMovieImp::GetMovieDimensions(float &width, float &height)
	{
		width = (float)dsdata->videoWidth;
		height = (float)dsdata->videoHeight;
	}

	float CDShowMovieImp::GetMovieWidth()
	{
		return (float)dsdata->videoWidth;
	}

	float CDShowMovieImp::GetMovieHeight()
	{
		return (float)dsdata->videoHeight;
	}

	void CDShowMovieImp::UnloadMovie()
	{
		std::string errLine;
		SQR_TRY
		{
			if (dsdata->pGraph==0)
				return;

			errLine = "pGrabber";
			if (dsdata->pGrabber!=0)
			{
				dsdata->pGrabber->Release();
				dsdata->pGrabber=0;
			}
			errLine = "pGrabberF";
			if (dsdata->pGrabberF!=0)
			{
				dsdata->pGrabberF->Release();
				dsdata->pGrabberF=0;
			}
			errLine = "pWindow";
			if (dsdata->pWindow!=0)
			{
				dsdata->pWindow->Release();
				dsdata->pWindow=0;
			}
			errLine = "pSeeking";
			if (dsdata->pSeeking!=0)
			{
				dsdata->pSeeking->Release();
				dsdata->pSeeking=0;
			}
			errLine = "pControl";
			if (dsdata->pControl!=0)
			{
				dsdata->pControl->Release();
				dsdata->pControl=0;
			}
			errLine = "pEvent";
			if (dsdata->pEvent!=0)
			{
				dsdata->pEvent->Release();
				dsdata->pEvent=0;
			}
			errLine = "pAviSpliter";
			if (dsdata->pAviSpliter!=0)
			{
				dsdata->pAviSpliter->Release();
				dsdata->pAviSpliter=0;
			}
			errLine = "pSrcFilter";
			if (dsdata->pSrcFilter!=0)
			{
				dsdata->pSrcFilter->Release();
				dsdata->pSrcFilter=0;
			}
			errLine = "pGraph";
			if (dsdata->pGraph!=0)
			{
				dsdata->pGraph->Release();
				dsdata->pGraph=0;
			}
			errLine = "m_pMemStream";
			if (m_pMemStream!=0)
			{
				delete m_pMemStream;
				m_pMemStream=0;
			}
			errLine = "m_pTexture";
			SafeRelease(m_pTexture);
		}
		SQR_CATCH(exp)
		{
			std::string msg = "UnloadMovie异常！";
			msg.append(errLine);
			exp.AppendType(msg, true);
			GfkLogExp(exp);
		}
		SQR_TRY_END
	}

	void CDShowMovieImp::Pause()
	{
		if (!m_strPreName.length())
		{
			return;
		}

		if (dsdata->pControl)
			dsdata->pControl->Pause();
	}

	void CDShowMovieImp::Play()
	{
		if (!m_strPreName.length())
		{
			return;
		}
		
		if (dsdata->pControl)
		{
			HRESULT hr = dsdata->pControl->Run();
			while (hr == S_FALSE)
			{
				hr = dsdata->pControl->Run();
			}
		}
	}

	void CDShowMovieImp::Rewind()
	{
		if (!m_strPreName.length())
		{
			return;
		}

		if (!dsdata->pSeeking) return;

		LONGLONG p1=0;
		LONGLONG p2=0;

		dsdata->pSeeking->SetPositions(
			&p1, AM_SEEKING_AbsolutePositioning, &p2, AM_SEEKING_NoPositioning);

		Play();
	}

	void CDShowMovieImp::Stop()
	{
		if (dsdata->pControl)
			dsdata->pControl->Stop();
	}

	bool CDShowMovieImp::IsPlayingMovie()
	{
		OAFilterState pfs;
		HRESULT hr;

		if (dsdata->pEvent!=NULL){
			long ev, p1, p2;

			while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0)){
				if (ev==EC_COMPLETE)
				{
					Pause();
					return false;
				}

				hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
				if (FAILED(hr))
				{
					Pause();
					return false;
				}
			}
		}

		if (dsdata->pControl!=NULL)
		{
			hr=dsdata->pControl->GetState(0, &pfs);
			if (FAILED(hr))
			{
				Pause();
				return false;
			}

			return pfs==State_Running;
		}

		return false;
	}

	bool CDShowMovieImp::IsPlayCompleted( void )
	{
		if (dsdata->pEvent!=NULL){
			long ev, p1, p2;

			while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0)){
				if (ev==EC_COMPLETE)
					return true;
				else
					return false;
			}
		}
		return false;
	}

	void CDShowMovieImp::UpdateMovieTexture()
	{
		HRESULT hr;
		unsigned int i, idx;
		int x, y;
		BYTE* bmpTmp;

		if (!dsdata->pGraph || !m_pTexture)
			return;

		long cbBuffer = 0;
		hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
		if (cbBuffer<=0)
		{
			return;
		}

		char *pBuffer = new char[cbBuffer];
		if (!pBuffer) 
		{
			throw("[DSHOW] Out of memory or empty buffer");
		}
		//得到每一帧的视频信息
		hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
		if (hr==E_INVALIDARG || hr==VFW_E_NOT_CONNECTED || hr==VFW_E_WRONG_STATE)
		{
			delete[] pBuffer;
			return;
		}
		if (FAILED(hr)) throw("[DSHOW] Failed at GetCurrentBuffer!");

		bmpTmp=(BYTE*)pBuffer;

		TEXLOCKED_RECT rect;
		m_pTexture->Lock(0, &rect, NULL, 0,LOCK_DISCARD);

		BYTE* pDest = (BYTE*)rect.pBits;

		bool shouldBeMirrored=mHorizontalMirroring;
		if (shouldBeMirrored){
			x=dsdata->videoWidth-1; y=dsdata->videoHeight-1;
		}else{
			x=0; y=dsdata->videoHeight-1;
		}

		//开始拷贝
		for (i=0; i<(unsigned int)(dsdata->videoWidth*dsdata->videoHeight*3); i+=3){
			idx=(x*4)+y*rect.Pitch;

			pDest[idx]=bmpTmp[i];//b
			pDest[idx+1]=bmpTmp[i+1];//g
			pDest[idx+2]=bmpTmp[i+2];//r
			pDest[idx+3]=255;//a

			if (shouldBeMirrored){
				x--;
				if (x<0){
					x=dsdata->videoWidth-1;
					y--; if (y<0) y=0;
				}
			}else{
				x++;
				if (x>=dsdata->videoWidth){
					x=0;
					y--; if (y<0) y=0;
				}
			}
		}

		m_pTexture->Unlock(0);

		// bye
		delete[] pBuffer;
	}

	void CDShowMovieImp::CleanTextureContents()
	{
		if (!m_pTexture)
		{
			return;
		}

		TEXLOCKED_RECT rect;
		m_pTexture->Lock(0, &rect, NULL, 0,LOCK_DISCARD);

		DWORD *pDest = (DWORD*)rect.pBits;

		for(int i = 0; i < (int)m_pTexture->GetHeight(); i++)
		{
			for(int j = 0; j < (int)m_pTexture->GetWidth(); j++)
			{
				int index = i * rect.Pitch / 4 + j;
				pDest[index] = 0x00000000;
			}
		}

		m_pTexture->Unlock(0);
	}

	float CDShowMovieImp::GetProgress( void )
	{
		if(!dsdata->pSeeking)
			return 0.0;

		LONGLONG curPos;
		dsdata->pSeeking->GetCurrentPosition(&curPos);

		LONGLONG length;
		dsdata->pSeeking->GetDuration(&length);

		return float(curPos) / length;
	}
}
