#ifndef _CGPU_PROGRAM_PARAMETERS_
#define _CGPU_PROGRAM_PARAMETERS_
#include "Dx9Base.h"
#include "CGpuProgramParameters.h"

namespace sqr
{
class CDx9GpuProgramParameters : public CGpuProgramParameters
{
public:
	CDx9GpuProgramParameters(LPDIRECT3DDEVICE9 pDevice);
	~CDx9GpuProgramParameters();
public:
	void release(void);

	bool Create(LPD3DXBUFFER pMicrocode);
	bool Create(LPD3DXCONSTANTTABLE pConstTable);

	bool setBool(const string& handle,BOOL b);
	bool setBoolArray(const string& handle,const BOOL *pb,size_t count);

	bool setFloat(const string& handle,float f);
	bool setFloatArray(const string& handle,const float *pf,size_t count);

	bool setInteger(const string& handle,int i);
	bool setIntegerArray(const string& handle,const int *pi,size_t count);

	bool setMatrix(const string& handle,CMatrix* pm);
	bool setMatrixArray(const string& handle,CMatrix* pm,size_t count);
	bool setMatrixPointerArray(const string& handle,CMatrix** pm,size_t count);

	//轻易不要使用以下两个设置向量的函数
	//为引擎中的向量定义方式于D3D不同，这二者之间的转换将会变得非常低效
	bool setVector(const string& handle,CVector3f* pv);
	bool setVectorArray(const string& handle,CVector3f* pv,size_t count);

	bool setD3DVector(const string& handle,D3DXVECTOR4* pv);
	bool setD3DVectorArray(const string& handle,D3DXVECTOR4* pv,size_t count);

	bool setValue(const string& handle,void* p,size_t count);

	void reset(LPDIRECT3DDEVICE9 pDevice)
	{
		m_pDevice = pDevice;
	}
protected:
	LPD3DXCONSTANTTABLE		m_pConstTable;
	LPDIRECT3DDEVICE9		m_pDevice;
};
}
#endif //_CGPU_PROGRAM_PARAMETERS_