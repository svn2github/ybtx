#include "stdafx.h"
#include "CDx9GpuProgram.h"
#include "BaseHelper.h"

CDx9GpuProgram::CDx9GpuProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice)
		:m_ID(ID),m_pD3DDevice(pDevice),m_Params(pDevice)
{}

CDx9GpuProgram::~CDx9GpuProgram()
{}

bool CDx9GpuProgram::load(const string& str)
{
	if (m_isLoad)
		release();
	LPD3DXBUFFER microcode;
	LPD3DXBUFFER errors;

	HRESULT hr = D3DXAssembleShaderFromFile(
	                 str.c_str(),
	                 NULL,
	                 NULL,
	                 0,
	                 &microcode,
	                 &errors);
	if (FAILED(hr))
	{
		string message = "D3D9 shaderɹ Errors:\n";
		message.append(static_cast<const char*>(errors->GetBufferPointer()));
		errors->Release();
		SafeRelease(errors);
		GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),message);
		//return false;
	}
	bool ret = _load(microcode);
	SafeRelease(microcode);
	SafeRelease(errors);
	return ret;
}

bool CDx9GpuProgram::load(const char* buf,size_t fsize)
{
	if (m_isLoad)
		release();

	LPD3DXBUFFER microcode;
	LPD3DXBUFFER errors;

	HRESULT hr = D3DXAssembleShader(
	                 buf,
	                 fsize,
	                 NULL,
	                 NULL,
	                 0,
	                 &microcode,
	                 &errors);
	if (FAILED(hr))
	{
		string message = "D3D9 shaderɹ Errors:\n";
		message.append(static_cast<const char*>(errors->GetBufferPointer()));
		errors->Release();
		SafeRelease(errors);
		GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),message);
		//return false;
	}
	bool ret = _load(microcode);
	SafeRelease(microcode);
	SafeRelease(errors);
	return ret;
}
bool CDx9GpuProgram::setExternalMicrocode(LPD3DXBUFFER pMicrocode)
{
	if (m_isLoad)
		release();
	return _load(pMicrocode);
}

CGpuVertexProgram::CGpuVertexProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice)
		:CDx9GpuProgram(ID,pDevice),m_pVertexShader(NULL)
{}

CGpuVertexProgram::~CGpuVertexProgram()
{
	SafeRelease(m_pVertexShader);
}

bool CGpuVertexProgram::bind(void)
{
	Ast(m_pVertexShader);
	return SUCCEEDED(m_pD3DDevice->SetVertexShader(m_pVertexShader));
}

bool CGpuVertexProgram::unbind()
{
	return SUCCEEDED(m_pD3DDevice->SetVertexShader(NULL));
}

bool CGpuVertexProgram::release()
{
	SafeRelease(m_pVertexShader);
	m_isLoad = false;
	return true;
}

bool CGpuVertexProgram::_load(LPD3DXBUFFER pMicrocode)
{
	if (m_isLoad)
		release();

	m_pD3DDevice->CreateVertexShader(
	    (DWORD*)pMicrocode->GetBufferPointer(),
	    &m_pVertexShader);
	m_isLoad = true;

	if (m_pVertexShader)
		if (m_Params.Create(pMicrocode))
			return true;

	SafeRelease(m_pVertexShader);
	m_isLoad = false;
	return false;
}

CGpuPixelProgram::CGpuPixelProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice)
		:CDx9GpuProgram(ID,pDevice),m_pPixelShader(NULL)
{}

CGpuPixelProgram::~CGpuPixelProgram()
{
	SafeRelease(m_pPixelShader);
}

bool CGpuPixelProgram::bind(void)
{
	Ast(m_pPixelShader);
	return SUCCEEDED(m_pD3DDevice->SetPixelShader(m_pPixelShader));
}

bool CGpuPixelProgram::unbind()
{
	return SUCCEEDED(m_pD3DDevice->SetPixelShader(NULL));
}

bool CGpuPixelProgram::release()
{
	SafeRelease(m_pPixelShader);
	m_isLoad = false;
	return true;
}

bool CGpuPixelProgram::_load(LPD3DXBUFFER pMicrocode)
{
	if (m_isLoad)
		release();

	m_pD3DDevice->CreatePixelShader(
	    (DWORD*)pMicrocode->GetBufferPointer(),
	    &m_pPixelShader);
	m_isLoad = true;

	if (m_pPixelShader)
		if (m_Params.Create(pMicrocode))
			return true;

	SafeRelease(m_pPixelShader);
	m_isLoad = false;
	return false;
}
