#ifndef _VERTEX_PROGRAM_H_
#define _VERTEX_PROGRAM_H_

#include "CGpuProgram.h"
#include "CDx9GpuProgramParameters.h"
namespace sqr
{
class CDx9VertexProgram : public CGpuProgram
{
public:
	CDx9VertexProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fMain = "main");
	~CDx9VertexProgram();
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
		return GPT_VERTEX_PROGRAM;
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
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	//static const GString	m_VertexShaderVerCom[ShaderVerCount];
protected:
	//bool	_load();
	bool	_next(void)
	{
		return false;
	}
};
}

#endif //_VERTEX_PROGRAM_H_