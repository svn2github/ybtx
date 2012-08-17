/**
*  @brief weibo call back
*  @file  splitstr.h
*  @author loach 
*  @Email < loachmr@sina.com >
*
* Copyright (C) 1996-2010 SINA Corporation, All Rights Reserved
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef  __cplusplus
extern "C" {
#endif

int  split_url_param(const char*& next , const char*& key , const char*& qe , const char*& split ,const char endflag );

int  split_url_copy_keyval(char* val , const char* start , const char* end);

/**
* split and parse URL parameters replied by the test-server
* into <em>oauth_token</em> and <em>oauth_token_secret</em>.
*/
int  wb_parse_oauth(const char *reply,char *token,char *secret, char* userid );

int	 wb_get_cookie(const char* reply , char *token, char *secret );

bool wb_readline( const char *reply , const char* &linestart, const char* &lineend );

int  wb_parse_cookie( const char *reply , char *token, char *secret );

/** ¥ÌŒÛ∑÷Œˆ */
void wb_parse_error(const char *reply, char *request , char* error ,  char *error_code );

/** ∂Ã¡¥Ω” */
int  wb_split_shortlink(const char* txt , const char* szEnd , const char*& start ,const char*& end , const char*& shortID);

#ifdef  __cplusplus
}
#endif