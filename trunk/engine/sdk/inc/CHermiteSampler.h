#pragma once

#include "CBaseSampler.h"
#include "CHermiteCurve.h"

namespace sqr
{
	template <class Type>
	class CHermiteSampler : public CBaseSampler<Type>
	{
	public:
		CHermiteSampler();
		~CHermiteSampler();

	public:
		virtual void SetControlPoint(MathVector<Type> vector, bool closed);
		virtual MathVector<Type> GetControlPoint(void);

	protected:
		virtual void Generate(void);

	private:
		CHermiteCurve* m_pHermiteMethod;
		MathVector<Type> m_vtOriginPoints;
	};

	template <class Type>
	sqr::CHermiteSampler<Type>::CHermiteSampler()
		: CBaseSampler<Type>()
		, m_pHermiteMethod(NULL)
	{
		m_pHermiteMethod = new CHermiteCurve;
	}

	template <class Type>
	sqr::CHermiteSampler<Type>::~CHermiteSampler()
	{
		if (m_pHermiteMethod)
		{
			delete m_pHermiteMethod;
			m_pHermiteMethod = NULL;
		}
	}

	template <class Type>
	void sqr::CHermiteSampler<Type>::SetControlPoint( MathVector<Type> vector, bool closed )
	{
		m_vtOriginPoints = vector;
		for (unsigned int i = 0; i < vector.size(); i++)
		{
			m_pHermiteMethod->Add(vector[i], float(i*10));
		}

		if (closed)
		{
			m_pHermiteMethod->Add(vector[0], float(vector.size()*10));
			m_vtOriginPoints.push_back(vector[0]);
		}

		Generate();
	}

	template <class Type>
	MathVector<Type> sqr::CHermiteSampler<Type>::GetControlPoint( void )
	{
		return m_vtOriginPoints;
	}

	template <class Type>
	void sqr::CHermiteSampler<Type>::Generate( void )
	{
		m_vtPoints.clear();

		size_t PointNum = INCREMENT * m_iDensity;

		if( PointNum > 1 )
		{
			for(size_t i = 0; i < PointNum; i++)
			{
				m_vtPoints.push_back(m_pHermiteMethod->Get( (m_vtOriginPoints.size()-1) * 10 * float(i) / (PointNum-1) ));
			}
		}
	}
}