/**
*  @brief memory malloc/free ,
*  @file  WeiboParseMemop.h
*  @author welbon 
*  @Email < libo.weng@gmail.com >
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
#ifndef __WEIBO_PARSER_MEMOPRATOR_H__
#define __WEIBO_PARSER_MEMOPRATOR_H__


#if defined(WIN32) || defined(WINCE)
#	define EXTERN extern
#else
#	define EXTERN 
#endif //WIN32 || WINCE

namespace wbParserNS
{
	EXTERN void *wbParse_Malloc_Cursor(int size);
	EXTERN void  wbParse_Free_Cursor(void *pvoid,int size);
	//用户
	EXTERN void *wbParse_Malloc_User(int size);
	EXTERN void  wbParse_Free_User(void *p,int size);

	//状态
	EXTERN void *wbParse_Malloc_Status(int size);
	EXTERN void  wbParse_Free_Status(void  *p,int size);

	//评论
	EXTERN void *wbParse_Malloc_Comment(int size);
	EXTERN void  wbParse_Free_Comment(void  *p,int size);

	//评论数
	EXTERN void *wbParse_Malloc_Commentcount(int size);
	EXTERN void  wbParse_Free_Commentcount(void  *p,int size);

	//未读数
	EXTERN void *wbParse_Malloc_Unread(int size);
	EXTERN void  wbParse_Free_Unread(void  *p,int size);

	//地理位置
	EXTERN void *wbParse_Malloc_Geo(int size);
	EXTERN void  wbParse_Free_Geo(void * p,int size);

	// 私信
	EXTERN void *wbParse_Malloc_Directmessage(int size);
	EXTERN void  wbParse_Free_Directmessage(void *p,int size);

	// 好友关系
	EXTERN void *wbParse_Malloc_Friendship(int size);
	EXTERN void  wbParse_Free_Friendship(void *p,int size);

	// ids
	EXTERN void *wbParse_Malloc_IDs(int size);
	EXTERN void  wbParse_Free_IDs(void *pvoid,int size);

	// 访问限制
	EXTERN void *wbParse_Malloc_Limits(int size);
	EXTERN void  wbParse_Free_Limits(void *p,int size);

	// 表情
	EXTERN void *wbParse_Malloc_Emotion(int size);
	EXTERN void  wbParse_Free_Emotion(void *p,int size);

	// 错误信息
	EXTERN void *wbParse_Malloc_Error(int size);
	EXTERN void  wbParse_Free_Error(void* p,int size);

	// t_wbParse_Media
	EXTERN void *wbParse_Malloc_Media(int size);
	EXTERN void  wbParse_Free_Media(void* p,int size);

	// t_wbParse_Media_item
	EXTERN void *wbParse_Malloc_Media_Item(int size);
	EXTERN void  wbParse_Free_Media_Item(void* p,int size);

	// Result
	EXTERN void *wbParse_Malloc_Ret(int size);
	EXTERN void  wbParse_Free_Ret(void* p,int size);

	// 用户标签
	EXTERN void *wbParse_Malloc_Tag(int size);
	EXTERN void  wbParse_Free_Tag(void* p,int size);

	////////////////////////////////////////////////////////////////////////
	// 话题
	EXTERN void *wbParse_Malloc_Trend(int size);
	EXTERN void  wbParse_Free_Trend(void* p,int size);

	// 话题热点查询结构
	EXTERN void *wbParse_Malloc_TrendHotQuery(int size);
	EXTERN void  wbParse_Free_TrendHotQuery(void* pvoid,int size);

	// 话题热点查询item结构
	EXTERN void *wbParse_Malloc_TrendHotQuery_Item(int size);
	EXTERN void  wbParse_Free_TrendHotQuery_Item(void* pvoid,int size);

	////////////////////////////////////////////////////////////////////////
	// 邀请联系人结构
	EXTERN void *wbParse_Malloc_InviteContact(int size);
	EXTERN void  wbParse_Free_InviteContact(void *p,int size);

	// 邀请联系人，用户结构
	EXTERN void *wbParse_Malloc_InviteContact_Usr(int size);
	EXTERN void wbParse_Free_InviteContact_Usr(void *p,int size);

	// 邀请联系人，用户分组结构
	EXTERN void *wbParse_Malloc_InviteContact_Group(int size);
	EXTERN void wbParse_Free_InviteContact_Group(void *p,int size);

	////////////////////////////////////////////////////////////////////////
	// Media

	// 短链接
	EXTERN void *wbParse_Malloc_Media_ShortURLBatch(int size);
	EXTERN void wbParse_Free_Media_ShortURLBatch(void *p,int size);

	// 短链接ITEM
	EXTERN void *wbParse_Malloc_Media_ShortURLBatch_ItemAnnotions(int size);
	EXTERN void wbParse_Free_Media_ShortURLBatch_ItemAnnotions(void *p,int size);

	////////////////////////////////////////////////////////////////////////
	// 
	//t_wbParse_UploadPic
	EXTERN void *wbParse_Malloc_UploadPic(int size);
	EXTERN void  wbParse_Free_UploadPic(void* p,int size);

}

#endif //__WEIBO_PARSER_MEMOPRATOR_H__