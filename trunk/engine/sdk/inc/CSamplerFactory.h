#pragma once

#include "CBaseSampler.h"
#include "CBezierSampler.h"
#include "CSimBezierSampler.h"
#include "CLineSampler.h"
#include "CCircleSampler.h"
#include "CHermiteSampler.h"

template <class Type>

class CSamplerFactory
{
public:
	CSamplerFactory();
	~CSamplerFactory();

public:
	virtual CBaseSampler<Type>* CreateSampler(SAMPLER_TYPE type);
};

template <class Type>
CSamplerFactory<Type>::CSamplerFactory()
{

}

template <class Type>
CSamplerFactory<Type>::~CSamplerFactory()
{

}

template <class Type>
CBaseSampler<Type>* CSamplerFactory<Type>::CreateSampler( SAMPLER_TYPE type )
{
	switch(type)
	{
	case SIMBEZIER:
		return new CSimBezierSampler<Type>;

	case BEZIER:
		return new CBezierSampler<Type>;

	case LINE:
		return new CLineSampler<Type>;

	case CIRCLE:
		return new CCircleSampler<Type>;

	case HERMITE:
		return new CHermiteSampler<Type>;
	}
	return NULL;
}