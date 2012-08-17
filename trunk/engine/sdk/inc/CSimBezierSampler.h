#pragma once

#include "CBaseSampler.h"
#include "CBezierCurve.h"

namespace sqr
{
	template <class Type>
	class CSimBezierSampler : public CBaseSampler<Type>
	{
	public:
		CSimBezierSampler();
		~CSimBezierSampler();

	public:
		virtual void SetControlPoint(MathVector<Type> vector, bool closed);
		virtual MathVector<Type> GetControlPoint(void);

	protected:
		virtual void Generate(void);

	private:
		CBaseBeizerCurve<Type>* m_pBezierMethod;
	};

	template <class Type>
	sqr::CSimBezierSampler<Type>::CSimBezierSampler()
		: CBaseSampler<Type>()
		, m_pBezierMethod(NULL)
	{
		m_pBezierMethod = new CBaseBeizerCurve<Type>;
	}

	template <class Type>
	sqr::CSimBezierSampler<Type>::~CSimBezierSampler()
	{
		if (m_pBezierMethod)
		{
			delete m_pBezierMethod;
			m_pBezierMethod = NULL;
		}
	}

	template <class Type>
	void sqr::CSimBezierSampler<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_pBezierMethod->Clear();

		for (unsigned int i = 0; i < vector.size(); i++)
		{
			m_pBezierMethod->Add(vector[i]);
		}

		if (closed)
		{
			m_pBezierMethod->Add(vector[0]);
		}

		m_pBezierMethod->BinomialCoffs();

		Generate();
	}

	template <class Type>
	MathVector<Type> sqr::CSimBezierSampler<Type>::GetControlPoint( void )
	{
		return m_pBezierMethod->GetAllControllerPoints();
	}

	template <class Type>
	void sqr::CSimBezierSampler<Type>::Generate( void )
	{
		m_vtPoints.clear();

		size_t PointNum = INCREMENT * m_iDensity;

		if( PointNum > 1 )
		{
			for(size_t i = 0; i < PointNum; i++)
			{
				m_vtPoints.push_back(m_pBezierMethod->Get( float(i) / (PointNum-1) ));
			}
		}
	}
}