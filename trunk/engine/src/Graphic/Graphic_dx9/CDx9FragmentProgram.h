#ifndef _HLSL_PROGRAM_H_
#define _HLSL_PROGRAM_H_

#include "CGpuProgram.h"
#include "CDx9GpuProgramParameters.h"
namespace sqr
{
class CDx9FragmentProgram : public CGpuProgram
{
public:
	CDx9FragmentProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fMain = "main");
	~CDx9FragmentProgram();
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
		return GPT_FRAGMENT_PROGRAM;
	}
	void reset(void);
	void lost()
	{
		m_isNeedUpdateParameters=true;
	}
protected:
	bool					m_isActive;
	DWORD					m_ID;
	CDx9GpuProgramParameters	m_Params;
	GString				m_sMain;
	GString				m_cCode;
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPD3DXCONSTANTTABLE		m_pConstTable;
	LPDIRECT3DPIXELSHADER9	m_pPixelShader;
	//static const GString	m_FragmentVerCom[ShaderVerCount];
protected:
	//bool	_load();
	bool	_next(void)
	{
		return false;
	}
};
}

#endif //_HLSL_PROGRAM_H_