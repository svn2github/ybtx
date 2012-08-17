#pragma once

#include "CBaseSampler.h"
#include "CLineMethod.h"

namespace sqr
{
	template <class Type>
	class CLineSampler : public CBaseSampler<Type>
	{
	public:
		CLineSampler();
		~CLineSampler();

	public:
		virtual void SetControlPoint(MathVector<Type> vector, bool closed);
		virtual MathVector<Type> GetControlPoint(void);

	protected:
		virtual void Generate(void);

	private:
		CLineMethod<Type>* m_pLineMethod;
	};

	template <class Type>
	sqr::CLineSampler<Type>::CLineSampler()
		: CBaseSampler<Type>()
		, m_pLineMethod(NULL)
	{
		m_pLineMethod = new CLineMethod<CVector3f>;
	}

	template <class Type>
	sqr::CLineSampler<Type>::~CLineSampler()
	{
		if (m_pLineMethod)
		{
			delete m_pLineMethod;
			m_pLineMethod = NULL;
		}
	}

	template <class Type>
	void sqr::CLineSampler<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_pLineMethod->SetControlPoint(vector, closed);
		Generate();
	}

	template <class Type>
	MathVector<Type> sqr::CLineSampler<Type>::GetControlPoint( void )
	{
		return m_pLineMethod->GetControlPoint();
	}

	template <class Type>
	void sqr::CLineSampler<Type>::Generate( void )
	{
		m_vtPoints.clear();

		size_t PointNum = INCREMENT * m_iDensity;

		if( PointNum > 1 )
		{
			for(size_t i = 0; i < PointNum; i++)
			{
				m_vtPoints.push_back(m_pLineMethod->GetPoint( float(i) / (PointNum-1) ));
			}
		}
	}
}