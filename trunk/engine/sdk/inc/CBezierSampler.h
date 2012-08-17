#pragma once

#include "CBaseSampler.h"
#include "CBezierMethod.h"

namespace sqr
{
	template <class Type>
	class CBezierSampler : public CBaseSampler<Type>
	{
	public:
		CBezierSampler();
		~CBezierSampler();

	public:
		void SetSmooth(float value);
		virtual void SetControlPoint(MathVector<Type> vector, bool closed);
		virtual MathVector<Type> GetControlPoint(void);

	protected:
		virtual void Generate(void);

	public:
		CBezierMethod<Type>* m_pBezierMethod;
	};

	template <class Type>
	sqr::CBezierSampler<Type>::CBezierSampler()
		: CBaseSampler<Type>()
		, m_pBezierMethod(NULL)
	{
		m_pBezierMethod = new CBezierMethod<Type>;
	}

	template <class Type>
	sqr::CBezierSampler<Type>::~CBezierSampler()
	{
		if (m_pBezierMethod)
		{
			delete m_pBezierMethod;
			m_pBezierMethod = NULL;
		}
	}

	template <class Type>
	void sqr::CBezierSampler<Type>::SetSmooth( float value )
	{
		m_pBezierMethod->SetSmooth(value);
	}

	template <class Type>
	void sqr::CBezierSampler<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_pBezierMethod->SetControlPoint(vector, closed);
		Generate();
	}

	template <class Type>
	MathVector<Type> sqr::CBezierSampler<Type>::GetControlPoint( void )
	{
		return m_pBezierMethod->GetControlPoint();
	}

	template <class Type>
	void sqr::CBezierSampler<Type>::Generate( void )
	{
		m_vtPoints.clear();

		size_t PointNum = INCREMENT * m_iDensity;

		if( PointNum > 1 )
		{
			for(size_t i = 0; i < PointNum; i++)
			{
				m_vtPoints.push_back(m_pBezierMethod->GetPoint( float(i) / (PointNum-1) ));
			}
		}
	}
}