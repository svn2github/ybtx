#ifndef _CGPUProgram_Mgr_H_
#define _CGPUProgram_Mgr_H_

#include "CGpuProgramMgr.h"
#include "Dx9Base.h"
#include "CDX9ShaderMacro.h"

namespace sqr
{
class CDx9GpuProgramMgr : public CGpuProgramMgr
{
public:
	CDx9GpuProgramMgr();
	~CDx9GpuProgramMgr();
public:
	void resetDevice();
	void lostDevice();
protected:
	CGpuProgram* _create_vertex_pro(DWORD handle,const string& fun);
	CGpuProgram* _create_fragment_pro(DWORD handle,const string& fun);
	CGpuProgram* _create_effects_pro(DWORD handle,const string& fun,const string& params = "");

	DX9_ShaderMacroMgr		m_ShaderMacro;
	DX9_ShaderInclude		m_ShaderInc;
};
}
#endif //_CGPUProgram_Mgr_H_