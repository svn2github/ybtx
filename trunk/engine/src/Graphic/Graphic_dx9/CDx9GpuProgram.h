#ifndef _GPU_PROGRAM_H_
#define _GPU_PROGRAM_H_

#include "CGpuProgram.h"
#include "CDx9GpuProgramParameters.h"

namespace sqr
{
class CDx9GpuProgram : public CGpuProgram
{
public:
	CDx9GpuProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDx9GpuProgram();
public:
	bool load(const string& str);
	bool load(const char* buf,size_t fsize);
	bool setExternalMicrocode(LPD3DXBUFFER pMicrocode);
	CGpuProgramParameters*	getParamPointer(void)
	{
		return &m_Params;
	}
protected:
	bool					m_isLoad;
	DWORD					m_ID;
	CDx9GpuProgramParameters	m_Params;
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	virtual bool			_load(LPD3DXBUFFER pMicrocode) = 0;
	bool _next(void)
	{
		return false;    //这个接口是完全为了多Pass设计的
	}
};

class CGpuVertexProgram : public CDx9GpuProgram
{
public:
	CGpuVertexProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice);
	~CGpuVertexProgram();
	bool bind(void);
	bool unbind(void);
	bool release();
	GpuProgramType getType(void)
	{
		return GPT_VERTEX_PROGRAM;
	}
protected:
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	bool _load(LPD3DXBUFFER pMicrocode);
};

class CGpuPixelProgram : public CDx9GpuProgram
{
public:
	CGpuPixelProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice);
	~CGpuPixelProgram();
	bool bind(void);
	bool unbind(void);
	bool release();
	GpuProgramType getType(void)
	{
		return GPT_FRAGMENT_PROGRAM;
	}
protected:
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
	bool _load(LPD3DXBUFFER pMicrocode);
};

}

#endif //_GPU_PROGRAM_H_