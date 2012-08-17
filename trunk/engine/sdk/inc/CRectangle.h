#pragma once

#include "CPos.h"
#ifdef _WIN32
#include <windows.h>
#endif


namespace sqr
{
template<class T> struct TRect;

typedef TRect<int>		CIRect;
typedef TRect<float>	CFRect;
typedef TRect<CPos>		CPosRect;

#ifndef _WIN32
struct RECT
{
	long left;
	long top;
	long right;
	long bottom;
};
#endif

template <typename T>
struct TRect : public CMathMallocObject
{
public:
	TRect() : left( T() ),top( T() ),right( T() ),bottom( T() ) {}

	/// @brief from left, top, right, and bottom
	TRect(T l, T t, T r, T b)
	{
		left		= l;
		top			= t;
		right		= r;
		bottom		= b;
	};
	TRect(const TRect<T>& srcRect)
	{
		left		= srcRect.left;
		top			= srcRect.top;
		right		= srcRect.right;
		bottom		= srcRect.bottom;
	};

	TRect(const RECT& srcRect)
	{
		left		= (T)srcRect.left;
		top			= (T)srcRect.top;
		right		= (T)srcRect.right;
		bottom		= (T)srcRect.bottom;
	};

	operator RECT() const
	{
		RECT rc = { (long)left, (long)top, (long)right, (long)bottom };
		return rc;
	}

	/// 求两个矩形的交集
	TRect Intersection( const TRect& rt ) const
	{
		TRect ret( max( left, rt.left ), max( top, rt.top ), min( right, rt.right ), min( bottom, rt.bottom ) );
		return ret.IsEmpty() ? TRect() : ret;
	}

	/// 求两个矩形的并集 
	TRect Combination( const TRect& rt ) const
	{
		if(rt.IsEmpty())
			return *this;
		if(IsEmpty())
			return rt;
		TRect ret( min( left, rt.left ), min( top, rt.top ), max( right, rt.right ), max( bottom, rt.bottom ) );
		return ret;
	}

	TRect operator+ ( const TPos<T> pos )
	{
		return TRect( left + pos.x, top + pos.y, right + pos.x, bottom + pos.y );
	}

	TRect operator+ ( const TRect<T> rt )
	{
		return TRect( left + rt.left, top + rt.top, right + rt.right, bottom + rt.bottom );
	}

	TRect operator- ( const TRect<T> rt )
	{
		return TRect( left - rt.left, top - rt.top, right - rt.right, bottom - rt.bottom );
	}

	TRect operator* ( T n )
	{
		return TRect( left*n, top*n, right*n, bottom*n );
	}
	TRect& Offset(T x, T y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
		return *this;
	}
	/// @brief retrieves the width
	T Width() const
	{
		return right - left;
	}
	/// @brief returns the height
	T Height() const
	{
		return bottom - top;
	}
	/// @brief return true if(left >= right || top >= bottom)
	bool IsEmpty()const
	{
		return left >= right || top >= bottom;
	}
	/// @brief is point in rect
	bool IsPtInRect( TPos<T> p )
	{
		return p.x >= left && p.x < right && p.y <= top && p.y > bottom;
	}
	TRect& SetValue(T l, T t, T r, T b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
		return *this;
	}
public:
	T    left;
	T    top;
	T    right;
	T    bottom;
};

template <>
struct TRect<CPos>
{
public:
	TRect() : ptLT( CPos() ), ptRT( CPos() ), ptLB( CPos() ), ptRB( CPos() ) {};

	/// @brief  from left, top, right, and bottom
	TRect(CPos lt, CPos rt, CPos lb, CPos rb)
	{
		ptLT		= lt;
		ptRT    	= rt;
		ptLB		= lb;
		ptRB		= rb;
	};
	TRect(const TRect<CPos>& srcRect)
	{
		ptLT		= srcRect.ptLT;
		ptRT	    = srcRect.ptRT;
		ptLB		= srcRect.ptLB;
		ptRB		= srcRect.ptRB;
	};
public:
	CPos    ptLT;
	CPos    ptRT;
	CPos    ptLB;
	CPos    ptRB;
};
};

;
