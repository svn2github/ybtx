#pragma once

#include "CMatrix.h"
#include "CVector3.h"
#include "CGraphicMallocObject.h"

namespace sqr
{
	class CGpuProgramParameters : public CGraphicMallocObject
{
public:
	virtual ~CGpuProgramParameters() {};
	virtual void release(void) = 0;

	virtual bool setBool(const string& handle,BOOL b) = 0;
	virtual bool setBoolArray(const string& handle,const BOOL *pb,size_t count) = 0;

	virtual bool setFloat(const string& handle,float f) = 0;
	virtual bool setFloatArray(const string& handle,const float *pf,size_t count) = 0;

	virtual bool setInteger(const string& handle,int i) = 0;
	virtual bool setIntegerArray(const string& handle,const int *pi,size_t count) = 0;

	virtual bool setMatrix(const string& handle,CMatrix* pm) = 0;
	virtual bool setMatrixArray(const string& handle,CMatrix* pm,size_t count)= 0;
	virtual bool setMatrixPointerArray(const string& handle,CMatrix** pm,size_t count) = 0;

	virtual bool setVector(const string& handle,CVector3f* pv) = 0;
	virtual bool setVectorArray(const string& handle,CVector3f* pv,size_t count) = 0;

	virtual bool setValue(const string& handle,void* p,size_t count) = 0;
};
}