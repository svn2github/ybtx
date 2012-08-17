#pragma once

namespace sqr
{
	namespace MathLib
	{
		template <class T>
		inline const T& clamp( const T& val, const T& low, const T& high )
		{
			return max(low, min(high, val));
		}

		template <class T>
		inline T lerp( const T& lhs, const T& rhs, float s )
		{
			return lhs + (rhs-lhs)*s;
		}

		template <typename T>
		inline typename T::value_type dot(T const & lhs, T const & rhs)
		{
			return detail::dot_helper<typename T::value_type,
				T::elem_num>::Do(&lhs[0], &rhs[0]);
		}

		template <typename T>
		inline typename T::value_type length_sq(T const & rhs)
		{
			return dot(rhs, rhs);
		}

		inline float sqrt(float x)
		{
			return std::sqrt(x);
		}
	}
}