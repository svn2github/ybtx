#include "stdafx.h"
#include "CDx9VertexProgram.h"
#include "BaseHelper.h"
#include "CDX9ShaderMacro.h"
#include "CDx9MainWindow.h"
#include "CGpuProgramMgr.h"
//#include "TimeHelper.h"
//
//const GString CDx9VertexProgram::m_VertexShaderVerCom[] = 
//{
//	"vs_1_0",
//	"vs_1_0",
//	"vs_2_0",
//	"vs_2_0",
//	"vs_2_0",
//};

CDx9VertexProgram::CDx9VertexProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fMain)
		:m_ID(ID),m_pD3DDevice(pDevice),m_Params(pDevice),m_sMain(fMain.c_str()),
		m_pConstTable(NULL),m_pVertexShader(NULL),m_isActive(false)
{
}

CDx9VertexProgram::~CDx9VertexProgram()
{
	release(); ///....诶
}

bool CDx9VertexProgram::load(const GString str)
{
	return load(str.c_str(),str.length());
}

bool CDx9VertexProgram::load(const char* buf,size_t fsize)
{
	if (m_isLoad)
		release();
	LPD3DXBUFFER errors;
	LPD3DXBUFFER pBuffer;

	HRESULT hr;

	hr = D3DXPreprocessShader(
	    buf,
	    static_cast<uint>(fsize),
	    ((DX9_ShaderMacroMgr*)ShaderMacroMgr::GetInst())->GET_MACRO(m_ID),
	    DX9_ShaderInclude::GetInst(),
	    &pBuffer,
	    &errors);

	if (FAILED(hr))
	{
		string message = m_sMain.c_str();
		message.append(" D3D9 Vertex Shader拼接不成功 Errors:\n");
		message.append(static_cast<const char*>(errors->GetBufferPointer()));
		errors->Release();
		OutputDebugString(message.c_str());
		GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),message);
		return false;
	}

	m_cCode = static_cast<const char*>(pBuffer->GetBufferPointer());
	SafeRelease(pBuffer);
	return true;

}

bool CDx9VertexProgram::release(void)
{
	SafeRelease(m_pVertexShader);
	m_isLoad = false;
	return true;
}


void CDx9VertexProgram::reset(void)
{
	m_pD3DDevice = GetD3DDevice();
	m_Params.reset(m_pD3DDevice);
};

bool CDx9VertexProgram::bind(void)
{
	if(!m_isLoad)
	{
		DWORD dwShaderFlags = D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_PREFER_FLOW_CONTROL;
#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

		LPD3DXBUFFER errors;
		LPD3DXBUFFER pBuffer;
		HRESULT hr;

		hr = D3DXCompileShader(
			m_cCode.c_str(),
			static_cast<uint>(m_cCode.size()),
			NULL,
			DX9_ShaderInclude::GetInst(),
			m_sMain.c_str(),
			"vs_2_0",
			dwShaderFlags,
			&pBuffer,
			&errors,
			&m_pConstTable);
		if (FAILED(hr))
		{
			string message = m_sMain.c_str();
			message.append(" D3D9 Vertex Shader编译不成功 Errors:\n");
			message.append(static_cast<const char*>(errors->GetBufferPointer()));
			errors->Release();
			OutputDebugString(message.c_str());
			GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),message);
			return false;
		}

		SafeRelease(errors);

		if (SUCCEEDED(m_pD3DDevice->CreateVertexShader(
			(DWORD*)pBuffer->GetBufferPointer(), &m_pVertexShader)))
		{
			m_Params.Create(m_pConstTable);
			m_isLoad = true;
			m_isNeedUpdateParameters = true;
			SafeRelease(pBuffer);
		}
		else
		{
			SafeRelease(pBuffer);
			return false;
		}
	}

	if (!m_isActive)
	{
		m_isActive = true;
		return SUCCEEDED(m_pD3DDevice->SetVertexShader(m_pVertexShader));
	}
	return true;
}

bool CDx9VertexProgram::unbind(void)
{
	m_pD3DDevice->SetVertexShader(NULL);
	m_isActive = false;
	return true;
}

//bool CDx9VertexProgram::_load()
//{
//	return 
//}