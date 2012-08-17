#ifndef _CEffectProgram_H_
#define _CEffectProgram_H_

#include "CGpuProgram.h"
#include "CDx9GpuProgramParameters.h"

namespace sqr
{
class CDx9EffectParameters : public CGpuProgramParameters
{
public:
	CDx9EffectParameters();
	~CDx9EffectParameters();
public:
	void release(void);
	void Create(ID3DXEffect* pD3DEffect);

	bool setBool(const string& handle,BOOL b);
	bool setBoolArray(const string& handle,const BOOL *pb,size_t count);

	bool setFloat(const string& handle,float f);
	bool setFloatArray(const string& handle,const float *pf,size_t count);

	bool setInteger(const string& handle,int i);
	bool setIntegerArray(const string& handle,const int *pi,size_t count);

	bool setMatrix(const string& handle,CMatrix* pm);
	bool setMatrixArray(const string& handle,CMatrix* pm,size_t count);
	bool setMatrixPointerArray(const string& handle,CMatrix** pm,size_t count);

	bool setVector(const string& handle,CVector3f* pv);
	bool setVectorArray(const string& handle,CVector3f* pv,size_t count);

	bool setValue(const string& handle,void* p,size_t count);
protected:
	ID3DXEffect*			m_pD3DEffect;
};

class CEffectProgram : public CGpuProgram
{
public:
	CEffectProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fName = "Main",const string& fParams = "");
	~CEffectProgram();
public:
	bool load(const GString str);
	bool load(const char* buf,size_t fsize);

	bool release(void);
	bool bind(void);
	bool unbind(void);

	CGpuProgramParameters*	getParamPointer(void)
	{
		return &m_Params;
	}
	GpuProgramType			getType(void)
	{
		return GPT_EFFECTS_PROGRAM;
	}
	void reset(void)
	{
		m_pD3DEffect->OnResetDevice();
	}

	void lost()
	{
		m_isNeedUpdateParameters=true;
		m_pD3DEffect->OnLostDevice();
	}

	/// you should call this method when you changes the parameters
	/// between bind() and unbind()
	bool commit();

	bool _next(void);

	ID3DXEffect*	GetD3DEffect()
	{
		return m_pD3DEffect;
	}
protected:
	bool					m_isActive;
	CDx9EffectParameters		m_Params;
	DWORD					m_ID;
	GString				m_sName;
	GString				m_sParams;
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	ID3DXEffect*			m_pD3DEffect;
	D3DXHANDLE				m_hTechnique;
	uint					m_iNumPass;
	uint					m_iPass;
private:
};

}
#endif //_CEffectProgram_H_