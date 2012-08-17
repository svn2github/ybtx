#ifndef __WBPARSER_H__
#define __WBPARSER_H__

#ifdef  __cplusplus
extern "C" {
#endif

//#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)
//
//#if defined(BUILDING_LIBWBPARSER)
//#define WBPARSER_API  __declspec(dllexport)
//#else
//#define WBPARSER_API  __declspec(dllimport)
//#endif
//
//#else
//	// 其他平台的，或者是静态链接
#define WBPARSER_API
//#endif

#define	_USE_XML_PARSER

	namespace wbParserNS
	{
		/*
		*
		* 针对新浪的开放平台的回调的字符串解析
		* 方便开发者调用
		*
		* 版权属于 新浪中国
		*
		* 作者：welbon
		*
		* 使用例程： 
		* 
		REQOBJ *pObjRoot = Parse_data_JSON(data);
		REQOBJ *pChild   = GetObject_Key_JSON(key,pObjRoot);
		if( pChild ){
		long l = GetLONG_Key_JSON(pChild);
		}
		*
		*
		* 时间：2010-07-01
		*/
		typedef void REQOBJ;

#if defined(_USE_JSON_PARSER)

		/** 解析JSON,data format : UTF8 */
		WBPARSER_API REQOBJ* Parse_data_JSON(const char* data,char *err_out = 0,const int len = 0);

		/** 释放 */
		WBPARSER_API void Parse_free_JSON(REQOBJ *obj);

		/** 从请求对象中获取值 */

		// object
		WBPARSER_API int     GetObject_JSON_SIZE(REQOBJ *obj);
		WBPARSER_API REQOBJ *GetObject_Key_JSON(const char *key ,REQOBJ *obj);
		WBPARSER_API REQOBJ *GetObject_Idx_JSON(const int idx ,REQOBJ *&obj );
		// long
		WBPARSER_API long long GetLONG_Key_JSON(const char *key,REQOBJ *obj);
		WBPARSER_API long long GetLONG_Idx_JSON(const int idx ,REQOBJ *obj);
		// string 
		WBPARSER_API void GetCHAR_Key_JSON(const char *key,REQOBJ *obj,char *out,const int len);
		WBPARSER_API void GetCHAR_Idx_JSON(const int idx,REQOBJ *obj,char *out,const int len);
		WBPARSER_API const char* GetCHAR_Key_JSON_EX(const char *key,REQOBJ *obj, int& outlen);

		// 将已知的OBJECT转成INT 
		WBPARSER_API long long GetLong_JSON(REQOBJ *obj);

#endif //_USE_JSON_PARSER

		////////////////////////////////////////////////////////////////////////////////
#if defined(_USE_XML_PARSER)
		/** 解析XML,data format : UTF8 */
		WBPARSER_API REQOBJ* Parse_data_XML(const char* data);

		/** 释放 */
		WBPARSER_API void Parse_free_XML(REQOBJ *obj);

		// object
		WBPARSER_API int     GetObject_XML_SIZE(REQOBJ *obj);
		WBPARSER_API REQOBJ *GetObject_Key_XML(const char *key ,REQOBJ *obj);
		WBPARSER_API REQOBJ *GetObject_Idx_XML(const int idx ,REQOBJ *&obj );
		// long
		WBPARSER_API long GetLONG_Key_XML(const char *key,REQOBJ *obj);
		WBPARSER_API long GetLONG_Idx_XML(const int idx ,REQOBJ *obj);
		// string 
		WBPARSER_API void GetCHAR_Key_XML(const char *key,REQOBJ *obj,char *out,const int len);
		WBPARSER_API void GetCHAR_Idx_XML(const int idx,REQOBJ *obj,char *out,const int len);
		WBPARSER_API const char* GetCHAR_Key_XML_EX(const char *key,REQOBJ *obj, int& outlen);

		// 将OBJ转成INT 
		WBPARSER_API long long GetLong_XML(REQOBJ *obj);

		/** 获取兄弟节点 */
		WBPARSER_API REQOBJ *GetSibling_XML(const char* key ,REQOBJ *obj);
#endif //_USE_XML_PARSER

	} // namespace

#ifdef  __cplusplus
}
#endif


#endif //#define __WEIBO_PARSER_H__