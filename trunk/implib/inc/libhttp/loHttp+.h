#ifndef __lo__HTTP_C_H__
#define __lo__HTTP_C_H__

/**
*
* C接口
*
* @author loach
*
* @date 2010-06-07
*/

#include <libhttp/loHttpDef.h>

BEGIN_EXTERN_C

namespace lohttp
{
	//--------------------------------------------------------------------------------------------------//
	// http 支持线程

	// 创建
	loHTTP_API LOHttp* Http_createobject(void);

	// 设置一些操作
	typedef enum 
	{
		LOHTTP_OPT_UNK = 0,
		LOHTTP_OPT_callbackOfBuffer , ///< 参考 lohttpdef.h  fun_httpbuffer_callback
		LOHTTP_OPT_callbackOfProgress ,///< 参考 lohttpdef.h  fun_httpprogress_callback
		LOHTTP_OPT_callbackOfCode ,///< 参考 lohttpdef.h  fun_httpcode_callback
		LOHTTP_OPT_callbackOfRespone,
		LOHTTP_OPT_callbackOfRelease ,///< 参考 lohttpdef.h  fun_httprelease_callback
		LOHTTP_OPT_userdata,
		LOHTTP_OPT_setmethod,
		LOHTTP_OPT_addparam, ///<  const HTTPChar* szName , const HTTPChar* szValue , unsigned long dwParamAttr
		LOHTTP_OPT_clearparam,
		LOHTTP_OPT_addheader,///< const HTTPChar* szName, const HTTPChar* szValue
		LOHTTP_OPT_clearheader,
		LOHTTP_OPT_addbinarydata, ///< const char* szBinaryData , long lLen
		LOHTTP_OPT_clearbinarydata,
		LOHTTP_OPT_setfile, ///<  const HTTPChar* szPath , const HTTPChar* szFile
		LOHTTP_OPT_setproxysrvaddr,///< int proxyType, const HTTPChar* szProxySrv, unsigned short nProxySrvPort
		LOHTTP_OPT_setproxyaccount,///< const HTTPChar* szUserName, const HTTPChar* szPassword
		LOHTTP_OPT_setuseragent,
		LOHTTP_OPT_setform,
		LOHTTP_OPT_LAST,
	}LOHttpoption;

	loHTTP_API int Http_setopt(LOHttp* phttp, LOHttpoption option , ...);
	/**
	* @brief 设置下载文件URL
	* @param szUrl 要下载的文件Url
	* @param szSavePath 另存为的本地路径，可以是目录或者具体文件路径。
	* @param lpszExt 扩展名
	* @return true表示成功初始化下载工作，false表示错误
	*/
	loHTTP_API void Http_seturl(LOHttp* phttp, const HTTPChar* szUrl);

	/*
	* @brief 开始下载(异步下载,内部有线程）
	* @param method 参考 enHttpMethod
	*/
	loHTTP_API int  Http_start_asyn(LOHttp* phttp);
	loHTTP_API void Http_wait(LOHttp* phttp);

	/** 同步执行 ，阻塞线程
	*
	* @param method 参考 enHttpMethod
	*/
	loHTTP_API int Http_start(LOHttp* phttp);

	/**
	* @brief 取消下载操作
	* @param msTimeout 等待的超时时间
	* @return 1 表示已经成功取消， 0 表示下载未开始。-1 表示等待超时。
	* @remark 当返回 1 或 -1 时，下载器在成功取消后都会
	*/
	loHTTP_API int Http_cancel(LOHttp* phttp ,  unsigned long msTimeout = 1000/*INFINITE*/ );

	// 请求完后，可能获取 回应包
	loHTTP_API LOHttpResponse* Http_getRespone(LOHttp* phttp);

	// 请求完后，分离 回应包
	loHTTP_API LOHttpResponse* Http_detachRespone(LOHttp* phttp);

	// 消毁
	loHTTP_API void Http_destroyobject(LOHttp* phttp);

	//--------------------------------------------------------------------------------------------//
	// LOHttpRespone ,需要结合 LOHttp 

	/** 获取http码（200，503，401。。。）*/
	loHTTP_API long Http_resp_code(const LOHttpResponse* resp);

	/** "http 头*/
	loHTTP_API bool Http_resp_getheaderlength(const LOHttpResponse* resp, unsigned long& dwHeaderLen );
	loHTTP_API bool Http_resp_readheader(const LOHttpResponse* resp , fun_http_resp_header_callback cb , void* pUsrData );

	/** 读取body */
	loHTTP_API bool Http_resp_getbodylength(const LOHttpResponse* resp , unsigned long& dwBodyLen);
	loHTTP_API bool Http_resp_readbody(const LOHttpResponse* resp , fun_http_resp_body_callback cb , void* pUsrData );

	/** 注：只有 Http_detachRespone 结合使用，否则会想不到的后果 */
	loHTTP_API void Http_resp_destroy(LOHttpResponse* resp);


	//--------------------------------------------------------------------------------------------//
	// LOHttpHeader

	loHTTP_API LOHttpHeader* Http_header_create(void);

	/** 分析 */
	loHTTP_API void Http_header_parse(LOHttpHeader* header , const char* context , unsigned long len);

	/** "Content-Length" 字段的长度
	* @return 存在 “Content-Length”，返回true
	*/
	loHTTP_API bool Http_header_getcontentlength (const LOHttpHeader* resp,unsigned long & cbContLen);

	/** http header information */
	loHTTP_API int Http_header_count(const LOHttpHeader* header , const HTTPChar* szName);
	loHTTP_API const HTTPChar*	Http_header_name(const LOHttpHeader* header , const HTTPChar* szName, unsigned long nIdx);

	/** 读取类似于 " 200 OK 这样的信息 "*/
	loHTTP_API const HTTPChar*	Http_header_state(const LOHttpHeader* header);

	/** 消毁 */
	loHTTP_API void Http_header_destroy(LOHttpHeader* header );

} // namespace lohttp

END_EXTERN_C

#endif /* __lo__HTTP_C_H__ */