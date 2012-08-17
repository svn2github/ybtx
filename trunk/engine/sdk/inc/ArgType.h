/*
 * =====================================================================================
 *
 *       Filename:  ArgType.h
 *
 *    Description:  参数类型生成
 *
 *        Version:  1.0
 *        Created:  2008年08月03日 18时51分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#pragma once

#include "TagsDef.h"

namespace sqr
{
/*-----------------------------------------------------------------------------
 *  Arg Type Generator
 *-----------------------------------------------------------------------------*/
template < class T, class Tag = TagValue>
class ArgType
{
public:
	typedef T type;
};

template <class T>
class ArgType<T, TagRef>
{
public:
	typedef T& type;
};

template <class T>
class ArgType<T, TagConstRef>
{
public:
	typedef const T& type;
};

template <class T>
class ArgType<T, TagPtr>
{
public:
	typedef T* type;
};

template <class T>
class ArgType<T, TagConstPtr>
{
public:
	typedef const T* type;
};
}
