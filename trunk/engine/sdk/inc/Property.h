/*
 * =====================================================================================
 *
 *       Filename:  Property.h
 *
 *    Description:  提供类似C#的Property语法定义
 *
 *        Version:  1.0
 *        Created:  2008年08月03日 18时41分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#include "ArgType.h"

namespace sqr
{

/*
 * =====================================================================================
 *        Class:  Property
 *  Description:  Property定义
 *  TemplateArg:  GetRetTypeTag:Get返回类型，值、引用等 SetArgTypeTag:Set函数的参数类型，值、引用等 AccessTag:访问权限
 * =====================================================================================
 */
template < class T, class AccessTag = TagReadWrite, class GetRetTypeTag = TagConstRef, class SetArgTypeTag = TagConstRef>
class Property
{
	typedef typename ArgType<T, SetArgTypeTag>::type SetArgType;
	typedef typename ArgType<T, GetRetTypeTag>::type GetRetType;
public:

	/* ====================  LIFECYCLE     ======================= */
	Property ()                             /* constructor */
	{
	}

	template <class T1>
	explicit Property(T1 value):value(value)
	{
	}


	/* ====================  ACCESS        ======================== */
	void Set(SetArgType value)
	{
		DoSet(value, AccessTag());
	}

	template <class T1>
	void Set(T1 value)
	{
		DoSet(value, AccessTag());
	}

	/* ====================  INQUIRY       ======================== */
	GetRetType Get() const
	{
		return DoGet(AccessTag());
	}

private:
	inline void DoSet(SetArgType value, TagWrite)
	{
		this->value = value;
	}

	template <class T1>
	inline void DoSet(T1 value, TagWrite)
	{
		this->value = value;
	}

	inline GetRetType DoGet(TagRead) const
	{
		return value;
	}
	T value;
}; /* ----------  end of template class  Property  ---------- */

//Get返回引用类型的Property
template < class T, class AccessTag = TagReadWrite>
class RefProperty : public Property<T, AccessTag, TagRef, TagConstRef>
{
};

//Get返回值类型的Property
template < class T, class AccessTag = TagReadWrite>
class ValueProperty : public Property<T, AccessTag, TagValue, TagValue>
{
};
}
