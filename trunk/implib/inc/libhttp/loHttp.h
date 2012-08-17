#ifndef __lo_HTTP_H__
#define __lo_HTTP_H__

#include <libhttp/loHttpDef.h>

namespace lohttp
{	
	class CloHttpResponse;
	class CloHttpAssoc;
	class CloHttpResponseAssoc;

	/** 一个下载任务 线程阻塞式的
	 * @author loach
	*/
	class loHTTP_API CloHttp
	{
	public:
		CloHttp();
		~CloHttp(void);	
	public:		
		/*下载大文件(内存缓冲区) 回调接口［边下载中］ */
		void SetCallbackOfBuffer(fun_httpbuffer_callback fbuffer_cb);
		
		/* 进度状态 */
		void SetCallbackOfProgress(fun_httpprogress_callback fprogress_cb);

		/** 下载状态
		*
		* @param state 状态(成功，失败）
		* @param errCode 失败码
		*/
		void SetCallbackOfCode(fun_httpcode_callback fcode_cb);

		void SetCallbackOfRespone(fun_httprespone_callback fresp_cb);

		/** CloHttp 资源释放掉 */
		void SetCallbackOfRelease(fun_httprelease_callback frelease_cb);

		/** 设置自定义数据 */
		void SetUserData(void* puserdata);

		// 设置url
		void SetURL( const HTTPChar* szURL );

		// 添加参数值
		void AddParam(const HTTPChar* szName , const HTTPChar* szValue , unsigned long dwParamAttr = lohttp::ParamNormal);
		void ClearParams();

		// 添加头信息
		void AddHeader(const HTTPChar* szName, const HTTPChar* szValue);
		void ClearHeader();

		// 添加二进制数据
		void AddBinaryData(const char* szBinaryData,long lLen);
		void ClearBinaryData(void);

		// 文件名（下载保存，或者上传文件）
		void SetFileName(const HTTPChar* szPath , const HTTPChar* szFile);

		// 代理
		void SetProxySrvAddr(int proxyType, const HTTPChar* szProxySrv, unsigned short nProxySrvPort);
		void SetProxyAccount(const HTTPChar* szUserName = NULL, const HTTPChar* szPassword = NULL);
		void ClearProxy();

		//
		void EnableFormData(bool bForm = false);
//		void SetUploadParam(const HTTPChar* lpszParamName, const HTTPChar* lpszFile);

		void SetHttpForm(const struct THttpForm* pform);
		//
		void SetUserAgent(const HTTPChar* szAgent);

		// 请求(同步请求，在一个线程完成，会阻塞线程）
		int  Request(const HTTPChar* szUrl , int method);

		// 异步请求（启动线程）
		int  RequestAsyn(const HTTPChar* szUrl , int method);
		void Wait();

		// 断开
		bool Cancel(unsigned long msTimeout = 1000 );

		// 获取到回应包
		CloHttpResponse* GetRespone(void) const;

		// 分离 回应包
		CloHttpResponse* DetachRespone(void);

		// 请求时间（不精确 )
		THttpTime* GetHttpTime() const;

		// http method
	private:
		void InitURL( const HTTPChar* szUrl,int method );
		//int  Split_url_copy_keyval(HTTPChar* val , const HTTPChar* start , const HTTPChar* end);
		//int  Split_url_param(const HTTPChar*& next , const HTTPChar*& key , const HTTPChar*& qe , const HTTPChar*& split ,const char endflag );

		/** donwload to header AND body */
		int	 HttpMethodGet();

		/** 下载到buffer 或 文件中 */
		int	 HttpMethodDownload();

		/** delete http */
		int	 HttpMethodDelete();

		/** put http */
		int	 HttpMethodPut();

		/** post 一般的数据 */
		int	 HttpMethodPost();

		/** 上传内容，可以：文件，数据 或 文字混排 */
		int	 HttpMethodUpload();

		/** */
//		int	 HttpMethodPostBinary();
//		int	 HttpMethodPostBinaryText();

		int  HttpMethod(int method);

		/** 设置代理信息 */
		void SetHttpProxyCfg();

		// 创建一个 respone包
		CloHttpResponse* CreateHttpResponse(void);

		// 异步（线程开始)
		unsigned long StartAsyn();
	protected:
		CloHttpAssoc* m_pAssoc;
		friend class CloHttpAssoc;
	};

	/** 请求后，应答包
	 *
	 * @author loach
	*/
	class loHTTP_API CloHttpResponse
	{
	public:
		CloHttpResponse(void);
		~CloHttpResponse(void);
	public:
		// 头信息长度
		bool GetHeaderLength(unsigned long& len) const;

		// 头信息内容
		unsigned long ReadHeader(char * pbyBuff, unsigned long cbBuff , unsigned long& offsize) const;
		bool ReadHeaderCallback(fun_http_resp_header_callback cb , void* pUsrData) const;

		/** body length */
		bool GetBodyLength(unsigned long & dwBodyLen) const;

		/** 移位读取，它会偏移 ，直到把整个读取完成 
		 *
		 * @offsize 读取的偏移量,offsize >= 整体大小时 ，return 0;
		 * @return 返回实际读取的大小， = 0 ，表示读取完成
		*/
		unsigned long ReadBody(char * pbyBuff, unsigned long cbBuff , unsigned long& offsize) const;
	    bool ReadBodyCallback(fun_http_resp_body_callback cb , void* pUsrData) const;

		// 200 , 401 , 504 ...
		long GetHttpCode(void) const;

	private:
		CloHttpResponseAssoc* m_pAssoc;
		friend class CloHttp;
	};

	/** http header 信息 */
	class loHTTP_API CloHttpHeader
	{
	public:
		CloHttpHeader();
		~CloHttpHeader();

		/** */
		void Parse(const char* header , unsigned long len);

		/** http header information */
		int GetHeadercount(const HTTPChar* szName) const;
		const HTTPChar*	GetHeader(const HTTPChar* szName, unsigned long nIdx = 0) const;

		/** 读取类似于 " 200 OK 这样的信息 "*/
		const HTTPChar*	GetHeaderState() const;

        bool GetContentLength (unsigned long & cbContLen) const;
	private:
		void* m_pAssoc;

		// 200 OK ,
		HTTPChar m_httpState[64];
	};

}//namespace lohttp

#endif //__lo_HTTP_H__