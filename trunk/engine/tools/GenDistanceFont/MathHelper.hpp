#ifndef _MATHHELPER_HPP
#define _MATHHELPER_HPP

#include <boost/static_assert.hpp>
#pragma warning(disable:4244)

namespace sqr
{
	namespace detail
	{
		template <typename T, int N>
		struct dot_helper
		{
			static T Do(T const * lhs, T const * rhs)
			{
				return lhs[0] * rhs[0] + dot_helper<T, N - 1>::Do(lhs + 1, rhs + 1);
			}
		};
		template <typename T>
		struct dot_helper<T, 1>
		{
			static T Do(T const * lhs, T const * rhs)
			{
				return lhs[0] * rhs[0];
			}
		};

		template <typename T, int N>
		struct vector_helper
		{
			template <typename U>
			static void DoCopy(T out[N], U const rhs[N])
			{
				out[0] = rhs[0];
				vector_helper<T, N - 1>::DoCopy(out + 1, rhs + 1);
			}

			static void DoAssign(T out[N], T const & rhs)
			{
				out[0] = rhs;
				vector_helper<T, N - 1>::DoAssign(out + 1, rhs);
			}

			static void DoAdd(T out[N], T const lhs[N], T const rhs[N])
			{
				out[0] = lhs[0] + rhs[0];
				vector_helper<T, N - 1>::DoAdd(out + 1, lhs + 1, rhs + 1);
			}

			static void DoSub(T out[N], T const lhs[N], T const rhs[N])
			{
				out[0] = lhs[0] - rhs[0];
				vector_helper<T, N - 1>::DoSub(out + 1, lhs + 1, rhs + 1);
			}

			static void DoMul(T out[N], T const lhs[N], T const rhs[N])
			{
				out[0] = lhs[0] * rhs[0];
				vector_helper<T, N - 1>::DoMul(out + 1, lhs + 1, rhs + 1);
			}

			static void DoScale(T out[N], T const lhs[N], T const & rhs)
			{
				out[0] = lhs[0] * rhs;
				vector_helper<T, N - 1>::DoScale(out + 1, lhs + 1, rhs);
			}

			static void DoDiv(T out[N], T const lhs[N], T const rhs[N])
			{
				out[0] = lhs[0] / rhs[0];
				vector_helper<T, N - 1>::DoMul(out + 1, lhs + 1, rhs + 1);
			}

			static void DoNegate(T out[N], T const rhs[N])
			{
				out[0] = -rhs[0];
				vector_helper<T, N - 1>::DoNegate(out + 1, rhs + 1);
			}

			static bool DoEqual(T const lhs[N], T const rhs[N])
			{
				return vector_helper<T, 1>::DoEqual(lhs, rhs) && vector_helper<T, N - 1>::DoEqual(lhs + 1, rhs + 1);
			}

			static void DoSwap(T lhs[N], T rhs[N])
			{
				std::swap(lhs[0], rhs[0]);
				return vector_helper<T, N - 1>::DoSwap(lhs + 1, rhs + 1);
			}
		};
		template <typename T>
		struct vector_helper<T, 1>
		{
			template <typename U>
			static void DoCopy(T out[1], U const rhs[1])
			{
				out[0] = rhs[0];
			}

			static void DoAssign(T out[1], T const & rhs)
			{
				out[0] = rhs;
			}

			static void DoAdd(T out[1], T const lhs[1], T const rhs[1])
			{
				out[0] = lhs[0] + rhs[0];
			}

			static void DoSub(T out[1], T const lhs[1], T const rhs[1])
			{
				out[0] = lhs[0] - rhs[0];
			}

			static void DoMul(T out[1], T const lhs[1], T const rhs[1])
			{
				out[0] = lhs[0] * rhs[0];
			}

			static void DoScale(T out[1], T const lhs[1], T const & rhs)
			{
				out[0] = lhs[0] * rhs;
			}

			static void DoDiv(T out[1], T const lhs[1], T const rhs[1])
			{
				out[0] = lhs[0] / rhs[0];
			}

			static void DoNegate(T out[1], T const rhs[1])
			{
				out[0] = -rhs[0];
			}

			static bool DoEqual(T const lhs[1], T const rhs[1])
			{
				return lhs[0] == rhs[0];
			}

			static void DoSwap(T lhs[1], T rhs[1])
			{
				std::swap(lhs[0], rhs[0]);
			}
		};
	}
}

#endif		// _MATHHELPER_HPP
