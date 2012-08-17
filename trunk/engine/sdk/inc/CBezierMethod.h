#pragma once

namespace sqr
{
	template <class Type>
	class CBezierMethod
	{
	public:
		CBezierMethod();
		~CBezierMethod();

	private:
		void GenerateControlPoint( int index, Type* point );
		Type BezierFunc(float param, Type* point);
		int GetStartIndex(float value, float& radio);

	public:
		void SetSmooth(float value);
		void SetControlPoint(MathVector<Type> vector, bool closed);
		MathVector<Type> GetControlPoint(void);
		virtual Type GetPoint(float value);

	public:
		float m_fSmoothness;
		float m_fSumLength;
		MathVector<float> m_vtLengths;
		MathVector<Type> m_vtOriginPoints;
		int m_iLastIndex;
	};

	template <class Type>
	sqr::CBezierMethod<Type>::CBezierMethod()
		: m_iLastIndex(0)
		, m_fSmoothness(1.0f)
		, m_fSumLength(0)
	{

	}

	template <class Type>
	sqr::CBezierMethod<Type>::~CBezierMethod()
	{

	}


	template <class Type>
	void sqr::CBezierMethod<Type>::GenerateControlPoint( int index, Type* controlPoint )
	{
		int nextIndex = (index + 1) % m_vtOriginPoints.size();
		int backIndex = (index - 1 + m_vtOriginPoints.size()) % m_vtOriginPoints.size();
		Type midPoint1,midPoint2;
		midPoint1 = (m_vtOriginPoints[index] + m_vtOriginPoints[nextIndex]) / 2;
		midPoint2 = (m_vtOriginPoints[index] + m_vtOriginPoints[backIndex]) / 2;
		Type midmidPoint;
		midmidPoint = (midPoint1 + midPoint2) / 2;
		Type offset = m_vtOriginPoints[index] - midmidPoint;
		controlPoint[3] = m_vtOriginPoints[index];
		controlPoint[2] = midPoint1 + offset * m_fSmoothness;

		index = (index + 1) % m_vtOriginPoints.size();

		nextIndex = (index + 1) % m_vtOriginPoints.size();
		backIndex = (index - 1 + m_vtOriginPoints.size()) % m_vtOriginPoints.size();
		midPoint1 = (m_vtOriginPoints[index] + m_vtOriginPoints[nextIndex]) / 2;
		midPoint2 = (m_vtOriginPoints[index] + m_vtOriginPoints[backIndex]) / 2;
		midmidPoint = (midPoint1 + midPoint2) / 2;
		offset = m_vtOriginPoints[index] - midmidPoint;
		controlPoint[1] = midPoint2 + offset * m_fSmoothness;
		controlPoint[0] = m_vtOriginPoints[index];
	}

	template <class Type>
	MathVector<Type> sqr::CBezierMethod<Type>::GetControlPoint( void )
	{
		return m_vtOriginPoints;
	}

	template <class Type>
	Type sqr::CBezierMethod<Type>::BezierFunc( float param, Type* point )
	{
		Type part0 =  point[0] * param * param * param;
		Type part1 =  3 * point[1] * param * param * (1 - param);
		Type part2 =  3 * point[2] * param * (1 - param) * (1 - param);
		Type part3 =  point[3] * (1 - param) * (1 - param) * (1 - param);
		return part0 + part1 + part2 + part3;
	}

	template <class Type>
	int sqr::CBezierMethod<Type>::GetStartIndex( float value, float& radio )
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
	void sqr::CBezierMethod<Type>::SetSmooth( float value )
	{
		m_fSmoothness = value;
	}

	template <class Type>
	void sqr::CBezierMethod<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
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
	Type sqr::CBezierMethod<Type>::GetPoint( float value )
	{
		float radio;
		int index = GetStartIndex(value, radio);

		if (m_iLastIndex != index)
		{
			m_iLastIndex = index;
			return m_vtOriginPoints[m_iLastIndex];
		}

		Type controlPoint[4];
		GenerateControlPoint(index, controlPoint);
		Type result = BezierFunc(radio, controlPoint);
		return result;
	}
}