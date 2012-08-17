/*
 * =====================================================================================
 *
 *       Filename:  TagsDef.h
 *
 *    Description:  各种模板tags定义
 *
 *        Version:  1.0
 *        Created:  2008年08月03日 18时42分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#pragma once

namespace sqr
{

/*-----------------------------------------------------------------------------
 *  ThreadTag
 *-----------------------------------------------------------------------------*/
class TagMultiThread { };                   /* 多线程标记 */
class TagSingleThread { };                  /* 单线程标记 */

/*-----------------------------------------------------------------------------
 *  ArgTypeTag
 *-----------------------------------------------------------------------------*/
class TagPtr { };                           /* 指针 */
class TagConstPtr { };                      /* Const指针 */
class TagRef { };                           /* 引用 */
class TagConstRef { };                      /* Const引用 */
class TagValue { };                         /* 值 */

/*-----------------------------------------------------------------------------
 *  AccessTag
 *-----------------------------------------------------------------------------*/
class TagRead { };                          /* 只读 */
class TagWrite { };                         /* 只写 */
class TagReadWrite : public TagRead, public TagWrite { };                     /* 读写 */
}
