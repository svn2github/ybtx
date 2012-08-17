#pragma once

#include "BaseTypes.h"

// Used for standard calling conventions
#ifdef _WIN32

#define THREAD				__declspec( thread )
#define STDCALL				__stdcall
#define	FASTCALL			__fastcall
#define FORCEINLINE			__forceinline

#else

#define THREAD				__thread
#define STDCALL
#define FASTCALL
#define FORCEINLINE			inline


#endif


#define NAMESPACE_BEGIN(name) \
	namespace name \
{

#define NAMESPACE_END \
}


#ifdef NDEBUG

#define class_cast static_cast

#else

template<typename dst_type>
class assert_cast
{
public:
	template<typename src_type>
	inline assert_cast(src_type a)
	{
		if (!static_cast<dst_type>(a))
			*(int*)(0)=1;
			
		p=dynamic_cast<dst_type>(a);
		if(!p)
			*(int*)(0)=1;
	}
	inline dst_type operator->()
	{
		return p;
	}
	inline operator dst_type()
	{
		return p;
	}
private:
	dst_type p;
};

#define class_cast assert_cast

#endif


