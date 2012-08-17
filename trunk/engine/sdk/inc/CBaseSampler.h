#pragma once

#include "CVector3.h"
#include "CMathMallocObject.h"

namespace sqr
{
	const int INCREMENT = 50;

	enum SAMPLER_TYPE
	{
		SIMBEZIER = 0,
		BEZIER,
		LINE,
		CIRCLE,
		HERMITE,
	};

	template <class Type>
	class CBaseSampler
	{
		//template <class Type>
	public:
		CBaseSampler();
		~CBaseSampler();

	public:
		virtual void SetControlPoint(MathVector<Type> vector, bool closed) = 0;
		virtual MathVector<Type> GetControlPoint(void) = 0;
		virtual void SetDensity(uint value);
		virtual MathVector<Type>& GetAllPoints(void);

	protected:
		virtual void Generate(void) = 0;

	protected:
		MathVector<Type> m_vtPoints;
		uint  m_iDensity;
	};

	template <class Type>
	sqr::CBaseSampler<Type>::CBaseSampler()
		: m_iDensity(1)
	{

	}

	template <class Type>
	sqr::CBaseSampler<Type>::~CBaseSampler()
	{

	}

	template <class Type>
	void sqr::CBaseSampler<Type>::SetDensity( uint value )
	{
		m_iDensity = value;
	}

	template <class Type>
	MathVector<Type>& sqr::CBaseSampler<Type>::GetAllPoints( void )
	{
		return m_vtPoints;
	}
}