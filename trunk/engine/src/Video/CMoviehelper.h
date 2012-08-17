#pragma once
 
namespace sqr
{
	//#include <dshow.h>
	// <hack>
	// 'qedit.h'中定义的接口

	EXTERN_C const CLSID CLSID_SampleGrabber;
	class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
		SampleGrabber;

	EXTERN_C const CLSID CLSID_NullRenderer;
	class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
		NullRenderer;

	EXTERN_C const IID IID_ISampleGrabberCB;
	MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
	ISampleGrabberCB : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SampleCB( 
			double SampleTime,
			IMediaSample *pSample) = 0;

		virtual HRESULT STDMETHODCALLTYPE BufferCB( 
			double SampleTime,
			BYTE *pBuffer,
			long BufferLen) = 0;

	};

	EXTERN_C const IID IID_ISampleGrabber;
	MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
	ISampleGrabber : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetOneShot( 
			BOOL OneShot) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
			const AM_MEDIA_TYPE *pType) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
			AM_MEDIA_TYPE *pType) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( 
			BOOL BufferThem) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( 
			/* [out][in] */ long *pBufferSize,
			/* [out] */ long *pBuffer) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( 
			/* [retval][out] */ IMediaSample **ppSample) = 0;

		virtual HRESULT STDMETHODCALLTYPE SetCallback( 
			ISampleGrabberCB *pCallback,
			long WhichMethodToCallback) = 0;

	};
    struct DirectShowData
    {
        /// filter管理器
        IGraphBuilder *pGraph;
        /// 媒体控制器
        IMediaControl *pControl;
        /// 媒体事件
        IMediaEvent *pEvent;
        /// 媒体搜寻
        IMediaSeeking *pSeeking;
        /// 媒体窗口
        IVideoWindow *pWindow;
        /// avi分割器
		IBaseFilter *pAviSpliter;
		/// 媒体信息抓取
		IBaseFilter *pGrabberF;
		/// sourcefitler
		IBaseFilter *pSrcFilter;
		/// 媒体信息抓取
		ISampleGrabber *pGrabber;
        /// 视频宽度
        int videoWidth;
        /// 视频高度
        int videoHeight;
    };
}