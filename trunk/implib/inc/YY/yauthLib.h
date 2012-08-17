/*
 * yauthdLib.h
 *
 *  Created on: 2011-3-25
 *      Author: Administrator
 */

#ifndef YAUTHDLIB_H_
#define YAUTHDLIB_H_

/*
 * yauthLib.cpp
 *
 *  Created on: 2011-3-25
 *      Author: Caizhi
 */

#ifndef WIN32
  #include "stddef.h"
  #include "stdint.h"
  #define DLL_EXPORT
#else
  typedef   unsigned       uint32_t; 
  #define DLL_EXPORT _declspec(dllexport)
#endif

#define YAUTH_CONF_FILE "yauth.conf"


#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: This library is not threadsafe, it use internal data to store private key and also buffers used to generate request.
   Please lock before calling in multi-thread programming model */

DLL_EXPORT int YAuth_init();
DLL_EXPORT int YAuth_init2(const char *confPath);

DLL_EXPORT void YAuth_cleanUp();

DLL_EXPORT const char * YAuth_genRequest(uint32_t connId);

DLL_EXPORT int YAuth_verifyToken(const char *token, uint32_t connId);

DLL_EXPORT int YAuth_fromHexToStr(const char *hexStr, char *outbuf,int outbufsize);

DLL_EXPORT const char * YAuth_getAttribute(const char * str, const char * attribute, char *buf, size_t buflen);


#ifdef __cplusplus
}
#endif

#endif /* YAUTHDLIB_H_ */
