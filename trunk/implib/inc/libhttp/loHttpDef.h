#ifndef __lo_HTTPDEF_H__
#define __lo_HTTPDEF_H__

#include <common/lo/lodefine.h>

BEGIN_EXTERN_C

#if (defined(WIN32) || defined(_WIN32) ) && !defined(HTTP_STATICLIB)

#if defined(LIBHTTP_DLL)
#define loHTTP_API  __declspec(dllexport)
#else
#define loHTTP_API  __declspec(dllimport)
#endif

#else
// 其他平台的，或者是静态链接
#define loHTTP_API 
#endif

#if defined(WIN32) || defined(_WIN32)

#else

#include <time.h>

#endif //defined(WIN32) || defined(_WIN32)


namespace lohttp
{

#if 1
	// 根据编译环境来定义
	// 
#ifndef BUILD_CHARACTER_SET

	typedef char HTTPChar;
	#define HTTP_T(s) s

#else

#if (defined(UNICDOE) || defined(_UNICODE))
	typedef wchar_t HTTPChar;
    #define HTTP_T(s) L##s
#else
	typedef char HTTPChar;
    #define HTTP_T(s) s
#endif

#endif //BUILD_CHARACTER_SET

#else

	// 只提供 ， 多字节方式
	typedef char      HTTPChar;
	#define HTTP_T(s) s

#endif


#pragma pack(push)
#pragma pack(4)

	enum enHttpStatus
	{
		E_HTTPSTATUS_CLOSED = 0, ///< 下载器关闭
		E_HTTPSTATUS_START,      ///< 下载器开始下载
		E_HTTPSTATUS_STOP,       ///< 下载器结束下载，但整个过程还没有完成
		E_HTTPSTATUS_CANCEL,     ///< 下载器已取消
		E_HTTPSTATUS_COMPLETE,   ///< 下载器下载完成，表示整个过程完成（注 ： 这个消息，不带表 “成功”，可能是“失败”，只告诉是一种状态)
		                         ///< 成攻，失败要看 错误码 == 0 ,表成功
	};

	/** 下载方法 */
	enum enHttpMethod
	{
		E_HTTP_GET_METHOD, ///< GET 
		E_HTTP_DOWNLOAD_METHOD,///< download to buffer OR download to file
		E_HTTP_POST_METHOD, ///< POST
//		E_HTTP_POST_BINARY_METHOD,//POST二进制数据
		E_HTTP_UPLOAD_METHOD, ///< 上传内容，或以与文本混排,需要结合 fun_formadd_callback 方式
//		E_HTTP_UPLOAD_BINARY_TEXT_METHOD,//POST二进制数据与文本混排
		E_HTTP_DELETE_METHOD,///<DELETE
		E_HTTP_PUT_METHOD,///PUT
	};

	enum ParamAttr
	{
		ParamNormal			= 0x00000000,		//!< The parameter is a normal parameter.
		//!  This means that the parameter's value contains the file path
		//!  and the file specified by the file path is uploaded when the HTTP UPLOAD is started.
		ParamEncodedName  	= 0x00000002,		//!< The parameter's name needed to a URL-Encoded string.,表明这个值，需要转化成URLCode
		//!  This means that the parameter's name is not encoded using URL-encoding
		//!  before sending to the web server when the HTTP GET or POST is started.
		ParamEncodedValue	= 0x00000004,		//!< The parameter's value needed to a URL-Encoded string.表明这个值，需要转化成URLCode
		//!  This means that the parameter's value is not encoded using URL-encoding
		//!  before sending to the web server when the HTTP GET or POST is started.
		ParamEncoded 		= (ParamEncodedName| ParamEncodedValue),	//!< The parameter's name and value are URL-Encoded strings.
		//!  This means that the parameter's name and value are not encoded using URL-encoding
		//!  before sending to the web server when the HTTP GET or POST is started.
		ParamUTF8Name       = 0x0000008, ///< 这个参数需要 UTF8编码

		ParamUTF8Value      = 0x0000010, ///< 
	} ;

	/** 返回错误(主要为了有效时间的处理) */
	enum enHTTPCode
	{
		HTTPE_OK = 0,
		HTTPE_UNSUPPORTED_PROTOCOL,    /* 1 */
		HTTPE_FAILED_INIT,             /* 2 */
		HTTPE_URL_MALFORMAT,           /* 3 */
		HTTPE_OBSOLETE4,               /* 4 - NOT USED */
		HTTPE_COULDNT_RESOLVE_PROXY,   /* 5 */
		HTTPE_COULDNT_RESOLVE_HOST,    /* 6 */
		HTTPE_COULDNT_CONNECT,         /* 7 */
		HTTPE_FTP_WEIRD_SERVER_REPLY,  /* 8 */
		HTTPE_REMOTE_ACCESS_DENIED,    /* 9 a service was denied by the server due to lack of access - when login fails this is not returned. */
		HTTPE_OBSOLETE10,              /* 10 - NOT USED */
		HTTPE_FTP_WEIRD_PASS_REPLY,    /* 11 */
		HTTPE_OBSOLETE12,              /* 12 - NOT USED */
		HTTPE_FTP_WEIRD_PASV_REPLY,    /* 13 */
		HTTPE_FTP_WEIRD_227_FORMAT,    /* 14 */
		HTTPE_FTP_CANT_GET_HOST,       /* 15 */
		HTTPE_OBSOLETE16,              /* 16 - NOT USED */
		HTTPE_FTP_COULDNT_SET_TYPE,    /* 17 */
		HTTPE_PARTIAL_FILE,            /* 18 */
		HTTPE_FTP_COULDNT_RETR_FILE,   /* 19 */
		HTTPE_OBSOLETE20,              /* 20 - NOT USED */
		HTTPE_QUOTE_ERROR,             /* 21 - quote command failure */
		HTTPE_HTTP_RETURNED_ERROR,     /* 22 */
		HTTPE_WRITE_ERROR,             /* 23 */
		HTTPE_OBSOLETE24,              /* 24 - NOT USED */
		HTTPE_UPLOAD_FAILED,           /* 25 - failed upload "command" */
		HTTPE_READ_ERROR,              /* 26 - couldn't open/read from file */
		HTTPE_OUT_OF_MEMORY,           /* 27  Note: HTTPE_OUT_OF_MEMORY may sometimes indicate a conversion error instead of a memory allocation error if CURL_DOES_CONVERSIONS is defined */
		HTTPE_OPERATION_TIMEDOUT,      /* 28 - the timeout time was reached */
		HTTPE_OBSOLETE29,              /* 29 - NOT USED */
		HTTPE_FTP_PORT_FAILED,         /* 30 - FTP PORT operation failed */
		HTTPE_FTP_COULDNT_USE_REST,    /* 31 - the REST command failed */
		HTTPE_OBSOLETE32,              /* 32 - NOT USED */
		HTTPE_RANGE_ERROR,             /* 33 - RANGE "command" didn't work */
		HTTPE_HTTP_POST_ERROR,         /* 34 */
		HTTPE_SSL_CONNECT_ERROR,       /* 35 - wrong when connecting with SSL */
		HTTPE_BAD_DOWNLOAD_RESUME,     /* 36 - couldn't resume download */
		HTTPE_FILE_COULDNT_READ_FILE,  /* 37 */
		HTTPE_LDAP_CANNOT_BIND,        /* 38 */
		HTTPE_LDAP_SEARCH_FAILED,      /* 39 */
		HTTPE_OBSOLETE40,              /* 40 - NOT USED */
		HTTPE_FUNCTION_NOT_FOUND,      /* 41 */
		HTTPE_ABORTED_BY_CALLBACK,     /* 42 */
		HTTPE_BAD_FUNCTION_ARGUMENT,   /* 43 */
		HTTPE_OBSOLETE44,              /* 44 - NOT USED */
		HTTPE_INTERFACE_FAILED,        /* 45 - CURLOPT_INTERFACE failed */
		HTTPE_OBSOLETE46,              /* 46 - NOT USED */
		HTTPE_TOO_MANY_REDIRECTS ,     /* 47 - catch endless re-direct loops */
		HTTPE_UNKNOWN_TELNET_OPTION,   /* 48 - User specified an unknown option */
		HTTPE_TELNET_OPTION_SYNTAX ,   /* 49 - Malformed telnet option */
		HTTPE_OBSOLETE50,              /* 50 - NOT USED */
		HTTPE_PEER_FAILED_VERIFICATION,/* 51 - peer's certificate or fingerprint wasn't verified fine */
		HTTPE_GOT_NOTHING,             /* 52 - when this is a specific error */
		HTTPE_SSL_ENGINE_NOTFOUND,     /* 53 - SSL crypto engine not found */
		HTTPE_SSL_ENGINE_SETFAILED,    /* 54 - can not set SSL crypto engine as default */
		HTTPE_SEND_ERROR,              /* 55 - failed sending network data */
		HTTPE_RECV_ERROR,              /* 56 - failure in receiving network data */
		HTTPE_OBSOLETE57,              /* 57 - NOT IN USE */
		HTTPE_SSL_CERTPROBLEM,         /* 58 - problem with the local certificate */
		HTTPE_SSL_CIPHER,              /* 59 - couldn't use specified cipher */
		HTTPE_SSL_CACERT,              /* 60 - problem with the CA cert (path?) */
		HTTPE_BAD_CONTENT_ENCODING,    /* 61 - Unrecognized transfer encoding */
		HTTPE_LDAP_INVALID_URL,        /* 62 - Invalid LDAP URL */
		HTTPE_FILESIZE_EXCEEDED,       /* 63 - Maximum file size exceeded */
		HTTPE_USE_SSL_FAILED,          /* 64 - Requested FTP SSL level failed */
		HTTPE_SEND_FAIL_REWIND,        /* 65 - Sending the data requires a rewind that failed */
		HTTPE_SSL_ENGINE_INITFAILED,   /* 66 - failed to initialise ENGINE */
		HTTPE_LOGIN_DENIED,            /* 67 - user, password or similar was not accepted and we failed to login */
		HTTPE_TFTP_NOTFOUND,           /* 68 - file not found on server */
		HTTPE_TFTP_PERM,               /* 69 - permission problem on server */
		HTTPE_REMOTE_DISK_FULL,        /* 70 - out of disk space on server */
		HTTPE_TFTP_ILLEGAL,            /* 71 - Illegal TFTP operation */
		HTTPE_TFTP_UNKNOWNID,          /* 72 - Unknown transfer ID */
		HTTPE_REMOTE_FILE_EXISTS,      /* 73 - File already exists */
		HTTPE_TFTP_NOSUCHUSER,         /* 74 - No such user */
		HTTPE_CONV_FAILED,             /* 75 - conversion failed */
		HTTPE_CONV_REQD,               /* 76 - caller must register conversion callbacks using curl_easy_setopt options CURLOPT_CONV_FROM_NETWORK_FUNCTION, CURLOPT_CONV_TO_NETWORK_FUNCTION, and CURLOPT_CONV_FROM_UTF8_FUNCTION */
		HTTPE_SSL_CACERT_BADFILE,      /* 77 - could not load CACERT file, missing or wrong format */
		HTTPE_REMOTE_FILE_NOT_FOUND,   /* 78 - remote file not found */
		HTTPE_SSH,                     /* 79 - error from the SSH layer, somewhat generic so the error message will be of interest when this has happened */
		HTTPE_SSL_SHUTDOWN_FAILED,     /* 80 - Failed to shut down the SSL connection */
		HTTPE_AGAIN,                   /* 81 - socket is not ready for send/recv,wait till it's ready and try again (Added in 7.18.2) */
		HTTPE_SSL_CRL_BADFILE,         /* 82 - could not load CRL file, missing or wrong format (Added in 7.19.0) */
		HTTPE_SSL_ISSUER_ERROR,        /* 83 - Issuer check failed.  (Added in 7.19.0) */

		// add by loach
		HTTPE_FILE_NAME_NULL, ///< 文件名空
		HTTPE_CREATE_FILE, ///< 创建失败
		HTTPE_OPENT_FILE,

		HTTPE_LAST /* never use! */
	};


	/**
	* 代理类型
	*/
	typedef enum 
	{
		E_PROXYTYPE_NONE = 0,				// 没有代理
		E_PROXYTYPE_HTTP,				// HTTP代理
		E_PROXYTYPE_SOCKS4,			// socks4代理
		E_PROXYTYPE_SOCKS5,			// socks5代理
	}EHttpProxyType;

	const int HTTP_BUFFER_STREAM_LENGTH  = 1024*10;
	const int HTTP_URL_LENGTH = 2048 ;

	/** 下载数据缓冲区 */
	struct  THttpBuffer
	{
		unsigned char  byBuffer[HTTP_BUFFER_STREAM_LENGTH];
		unsigned long  dwLength;
	};

	/** http 传输时间记录 */
	struct THttpTime
	{
		HTTPChar szURL[HTTP_URL_LENGTH];
		time_t	 tRequest;
		time_t	 tResponse;
	};

	/** http form 方式 （上传文件... ) */
	typedef enum
	{
		HTTP_FORMTYPE_UNK,
		HTTP_FORMTYPE_COPYNAME,
		HTTP_FORMTYPE_PTRNAME,
		HTTP_FORMTYPE_NAMELENGTH,
		HTTP_FORMTYPE_COPYCONTENTS,
		HTTP_FORMTYPE_PTRCONTENTS,
		HTTP_FORMTYPE_CONTENTSLENGTH,
		HTTP_FORMTYPE_FILECONTENT,
		HTTP_FORMTYPE_ARRAY,
		HTTP_FORMTYPE_OBSOLETE,
		HTTP_FORMTYPE_FILE,
		HTTP_FORMTYPE_BUFFER,
		HTTP_FORMTYPE_BUFFERPTR,
		HTTP_FORMTYPE_BUFFERLENGTH,
		HTTP_FORMTYPE_CONTENTTYPE,
		HTTP_FORMTYPE_CONTENTHEADER,
		HTTP_FORMTYPE_FILENAME,
		HTTP_FORMTYPE_END,
		HTTP_FORMTYPE_OBSOLETE2,
		HTTP_FORMTYPE_STREAM,
		HTTP_FORMTYPE_LASTENTRY
	}HTTPFormType;

	/** LOFromValue 有点特别，请参考例子 */
	typedef void (*fun_formdata_free)(void* pfromdata);
	typedef int  (*fun_formadd_write_callback)(void** httpost , void** last , ...);
	typedef void (*fun_formadd_callback)(void** httpost , void** last , fun_formadd_write_callback cb , void* pformdata);

	struct THttpForm
	{
		// add by loach for http form 方式
		fun_formadd_callback fformadd_cb;
		fun_formdata_free ffromdata_free_cb;
		void* pformdata;
	};

	typedef void LOHttp;
	typedef void LOHttpResponse;
	typedef void LOHttpHeader;

	/*下载大文件(内存缓冲区) 回调接口［边下载中 ，与OnHttpBody有所不同 ］ */
	typedef  int (*fun_httpbuffer_callback)(unsigned char* pBuffer , long lBuffer, void* pUsrData );

	/* 进度状态 */
	typedef  int (*fun_httpprogress_callback)(long lCurBytes , long lTotalBytes, void* pUsrData );

	/** 下载状态
	*
	* @param state 状态(成功，失败）
	* @param errCode 失败码
	* @param pTime  请求时间
	*/
	typedef void (*fun_httpcode_callback)(int state , int errCode ,const THttpTime* pTime , void* pUsrData );

	/** 产生回应包 */
	typedef void (*fun_httprespone_callback)(const LOHttpResponse* resp , void* pUsrData);

	/** CloHttp 资源释放掉 */
	typedef void (*fun_httprelease_callback)(void* pUsrData);

	/** 回应包的 */
	typedef void (*fun_http_resp_header_callback)(unsigned long httpcode,const char* header , unsigned long len , void* pUsrData);
	typedef void (*fun_http_resp_body_callback)(unsigned long httpcode , const char* body , unsigned long len , void* pUsrData);

#pragma pack(pop)

} // namespace lohttp


END_EXTERN_C

#endif // __lo_HTTPDEF_H__
