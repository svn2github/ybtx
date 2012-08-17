#pragma once

namespace sqr
{
	template <class Type>
	class CLineMethod
	{
	public:
		CLineMethod();
		~CLineMethod();

	private:
		void GenerateControlPoint( int index, Type* point );
		int GetStartIndex(float value, float& radio);
		Type LineFunc(Type staPoint, Type endPoint, float radio);

	public:
		void SetControlPoint(MathVector<Type> vector, bool closed);
		MathVector<Type> GetControlPoint(void);
		virtual Type GetPoint(float value);

	public:
		float m_fSumLength;
		MathVector<float> m_vtLengths;
		MathVector<Type> m_vtOriginPoints;
		int m_iLastIndex;
	};

	template <class Type>
	sqr::CLineMethod<Type>::CLineMethod()
		: m_iLastIndex(0)
		, m_fSumLength(0)
	{

	}

	template <class Type>
	sqr::CLineMethod<Type>::~CLineMethod()
	{

	}


	template <class Type>
	void sqr::CLineMethod<Type>::GenerateControlPoint( int index, Type* controlPoint )
	{

	}

	template <class Type>
	int sqr::CLineMethod<Type>::GetStartIndex( float value, float& radio )
	{
		float tempLength = 0.0f;
		for (unsigned int i = 0; i < m_vtLengths.size(); i++)
		{
			tempLength += m_vtLengths[i];
			float temp = tempLength / m_fSumLength;
			if (value <= temp)
			{
				radio = (value - (tempLength - m_vtLengths[i]) / m_fSumLength) / (m_vtLengths[i]/m_fSumLength);
				return i;
			}
		}
		return -1;
	}

	template <class Type>
	Type sqr::CLineMethod<Type>::LineFunc( Type staPoint, Type endPoint, float radio )
	{
		return staPoint + (endPoint - staPoint) * radio;
	}

	template <class Type>
	void sqr::CLineMethod<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_vtLengths.clear();
		m_vtOriginPoints = vector;
		m_fSumLength = 0.0f;
		for (unsigned int i = 0; i < m_vtOriginPoints.size(); i++)
		{
			if (!closed)
			{
				if (i == m_vtOriginPoints.size()-1)
					return;
			}

			Type dir = m_vtOriginPoints[i] - m_vtOriginPoints[(i+1)%m_vtOriginPoints.size()];
			float length = dir.Mag();
			m_fSumLength += length;
			m_vtLengths.push_back(length);
		}
	}

	template <class Type>
	MathVector<Type> sqr::CLineMethod<Type>::GetControlPoint( void )
	{
		return m_vtOriginPoints;
	}

	template <class Type>
	Type sqr::CLineMethod<Type>::GetPoint( float value )
	{
		float radio;
		int index = GetStartIndex(value, radio);

		if (m_iLastIndex != index)
		{
			int tempIndex = m_iLastIndex;
			m_iLastIndex = index;
			return m_vtOriginPoints[index];
		}

		return LineFunc( m_vtOriginPoints[index], m_vtOriginPoints[(index+1)%m_vtOriginPoints.size()], radio);
	}
}