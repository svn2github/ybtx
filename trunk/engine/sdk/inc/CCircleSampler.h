#pragma once

#include "CBaseSampler.h"
#include "CCircleMethod.h"

namespace sqr
{
	template <class Type>
	class CCircleSampler : public CBaseSampler<Type>
	{
	public:
		CCircleSampler();
		~CCircleSampler();

	public:
		virtual void SetControlPoint(MathVector<Type> vector, bool closed);
		virtual MathVector<Type> GetControlPoint(void);
		virtual void SetParam( Type center, float radius );

	protected:
		virtual void Generate(void);

	private:
		CCircleMethod<Type>* m_pCircleMethod;
	};

	template <class Type>
	sqr::CCircleSampler<Type>::CCircleSampler()
		:CBaseSampler<Type>()
		,m_pCircleMethod(NULL)
	{
		m_pCircleMethod = new CCircleMethod<CVector3f>;
	}

	template <class Type>
	sqr::CCircleSampler<Type>::~CCircleSampler()
	{
		if (m_pCircleMethod)
		{
			delete m_pCircleMethod;
			m_pCircleMethod = NULL;
		}
	}

	template <class Type>
	void sqr::CCircleSampler<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_pCircleMethod->SetControlPoint(vector, closed);
	}

	template <class Type>
	MathVector<Type> sqr::CCircleSampler<Type>::GetControlPoint( void )
	{
		return m_pCircleMethod->GetControlPoint();
	}

	template <class Type>
	void sqr::CCircleSampler<Type>::SetParam( Type center, float radius )
	{
		m_pCircleMethod->SetParam(center, radius);
		Generate();
	}

	template <class Type>
	void sqr::CCircleSampler<Type>::Generate( void )
	{
		m_vtPoints.clear();

		size_t PointNum = INCREMENT * m_iDensity;

		if( PointNum > 1 )
		{
			for(size_t i = 0; i < PointNum; i++)
			{
				m_vtPoints.push_back(m_pCircleMethod->GetPoint( float(i) / (PointNum-1) ));
			}
		}
	}
}