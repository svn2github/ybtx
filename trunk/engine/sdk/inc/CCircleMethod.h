#pragma once

namespace sqr
{
	template <class Type>
	class CCircleMethod
	{
	public:
		CCircleMethod();
		~CCircleMethod();

	public:
		void SetParam(Type center, float radius);
		void SetControlPoint(MathVector<Type> vector, bool closed);
		MathVector<Type> GetControlPoint(void);
		virtual Type GetPoint(float value);

	private:
		MathVector<Type> m_vtOriginPoints;
		Type m_Center;
		float m_fRadius;
	};

	template <class Type>
	sqr::CCircleMethod<Type>::CCircleMethod()
	{

	}

	template <class Type>
	sqr::CCircleMethod<Type>::~CCircleMethod()
	{

	}

	template <class Type>
	void sqr::CCircleMethod<Type>::SetParam( Type center, float radius )
	{
		m_Center = center;
		m_fRadius = radius;
	}

	template <class Type>
	void sqr::CCircleMethod<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_vtOriginPoints = vector;
	}

	template <class Type>
	MathVector<Type> sqr::CCircleMethod<Type>::GetControlPoint( void )
	{
		return m_vtOriginPoints;
	}


	template <class Type>
	Type sqr::CCircleMethod<Type>::GetPoint( float value )
	{
		float xDelta = cos(value*2*float(PI)) * m_fRadius;
		float yDelta = sin(value*2*float(PI)) * m_fRadius;
		return CVector3f(m_Center.x + xDelta, m_Center.y, m_Center.z + yDelta);
	}
}