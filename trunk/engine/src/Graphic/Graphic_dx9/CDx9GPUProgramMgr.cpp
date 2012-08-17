#include "stdafx.h"
#include "CDx9GPUProgramMgr.h"
#include "CDx9VertexProgram.h"
#include "CDx9FragmentProgram.h"
#include "CDx9EffectProgram.h"
#include "CDx9MainWindow.h"
#include "TSqrAllocator.inl"


CDx9GpuProgramMgr::CDx9GpuProgramMgr()
{
	m_pMacroMgr = &m_ShaderMacro;
	m_pShaderInc = &m_ShaderInc;
}
CDx9GpuProgramMgr::~CDx9GpuProgramMgr()
{
}

CGpuProgram* CDx9GpuProgramMgr::_create_vertex_pro(DWORD handle,const string& fun)
{
	return new CDx9VertexProgram(handle,GetD3DDevice(),fun);
}

CGpuProgram* CDx9GpuProgramMgr::_create_fragment_pro(DWORD handle,const string& fun)
{
	return new CDx9FragmentProgram(handle,GetD3DDevice(),fun);
}

CGpuProgram* CDx9GpuProgramMgr::_create_effects_pro(DWORD handle,const string& fun,const string& params)
{
	return new CEffectProgram(handle,GetD3DDevice(),fun,params);
}

void CDx9GpuProgramMgr::resetDevice(void)
{
	GpuProList::iterator it = m_ProgramList.begin();
	for (;it!=m_ProgramList.end();++it)
		it->second->reset();
}

void CDx9GpuProgramMgr::lostDevice()
{
	GpuProList::iterator it = m_ProgramList.begin();
	for (;it!=m_ProgramList.end();++it)
		it->second->lost();
}
