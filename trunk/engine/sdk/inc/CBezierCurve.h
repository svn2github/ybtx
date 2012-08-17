#pragma	once
#include "CMathMallocObject.h"

namespace sqr
{
	template <class Type>
	class CBaseBeizerCurve : public CMathMallocObject
	{
		MathVector<Type> vecControllerPoint;
		MathVector<int>  vecBiCoeffs;

	public:
		CBaseBeizerCurve();
		~CBaseBeizerCurve();

		void  Clear();
		void  Add( Type key );
		void  Rectify(Type key, const int controlPointIndex);
		void  Delete( Type key );
		Type  Get( float u );		///多控制点计算多阶
		bool  IsExit( Type key );
		MathVector<Type> GetAllControllerPoints();
		void  BinomialCoffs();

		int Factorial(const int n)
		{
			if( n == 0 )
				return 1;

			int value = 1;

			for (int i = n; i > 0; --i)
			{
				value = value * i;
			}

			return value;
		}

		int   ComposizeValue(const int n, const int i)
		{
			if( i == 0 )
				return 1;

			int value = 1;
			int nFactorialValue = Factorial(i);

			for (int m = 0; m < i; ++m)
			{
				value = value * (n-m);
			}

			return value / nFactorialValue;

		}

		float power(const float value, const int n)
		{
			float result = 1.0f;
			for (int m = 0; m < n; ++m )
			{
				result *= value;
			}
			return result;
		}

	private:
		vector<Type> vecControllerPoints;
	};


	template <class Type>
	CBaseBeizerCurve<Type>::CBaseBeizerCurve()
	{
		Clear();
	}

	template <class Type>
	CBaseBeizerCurve<Type>::~CBaseBeizerCurve()
	{
		Clear();
	}

	template <class Type>
	void CBaseBeizerCurve<Type>::Clear()
	{
		vecControllerPoint.clear();
	}

	template <class Type>
	void CBaseBeizerCurve<Type>::Add( Type key )
	{
		vecControllerPoint.push_back(key);
	}

	template <class Type>
	void CBaseBeizerCurve<Type>::Rectify(Type key, const int controlPointIndex)
	{
		if( (size_t)controlPointIndex >= vecControllerPoint.size() )
			return;

		vecControllerPoint[controlPointIndex] = key;
	}
	
	template <class Type>
	void CBaseBeizerCurve<Type>::Delete( Type key )
	{
		MathVector<Type>::iterator iter = find(vecControllerPoint.begin(), vecControllerPoint.end(), key);
		if( iter != vecControllerPoint.end() )
			vecControllerPoint.erase(iter);
	}

	//////////////////////////////////////////////////////////////////////////
	/*
	P(u) = Sum( C(n,k) * u的k阶乘 * (1-u)的(n-k)阶乘
	*/
	template <class Type>
	Type CBaseBeizerCurve<Type>::Get( float u )
	{
		Type noVal;
		size_t size = vecControllerPoint.size();
		int n = size - 1;///n=阶数, 阶数比控制数少一个
		
		if( u == 0.0f )
			return vecControllerPoint[0];

		if( u == 1.0f )
			return vecControllerPoint[n];

		for ( size_t k = 0; k < size; ++k )
		{
			float t1 = pow(1.0f-u, (float)(n-k));
			float t2 = pow(u, (float)k);
			Type tt =  t1 * t2 * vecBiCoeffs[k] * vecControllerPoint[k];
			noVal   += tt ;
		}
		return noVal;
	}

	template <class Type>
	void CBaseBeizerCurve<Type>::BinomialCoffs()
	{
		size_t size = vecControllerPoint.size();
		int n = size - 1;///n=阶数, 阶数比控制数少一个
		vecBiCoeffs.clear();
		vecBiCoeffs.resize(size);

		for ( size_t t = 0; t < size; ++t )
		{
			vecBiCoeffs[t] = ComposizeValue(n, t);
		}
	}

	template <class Type>
	bool  CBaseBeizerCurve<Type>::IsExit( Type key )
	{
		MathVector<Type>::iterator iter = find(vecControllerPoint.begin(), vecControllerPoint.end(), key);
		if( iter != vecControllerPoint.end() )
			return true;
		else
			return false;
	}

	template <class Type>
	MathVector<Type> CBaseBeizerCurve<Type>::GetAllControllerPoints()
	{
		return vecControllerPoint;
	}

}