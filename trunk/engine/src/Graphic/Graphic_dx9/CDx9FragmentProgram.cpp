#include "stdafx.h"
#include "CDx9FragmentProgram.h"
#include "BaseHelper.h"
#include "CDX9ShaderMacro.h"
#include "CDx9MainWindow.h"
#include "CGpuProgramMgr.h"
#include <DxErr.h>

//const GString CDx9FragmentProgram::m_FragmentVerCom[] = 
//{
//	"ps_1_0",
//	"ps_1_0",
//	"ps_2_0",
//	"ps_2_0",
//	"ps_2_0",
//};

CDx9FragmentProgram::CDx9FragmentProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fMain)
		:m_ID(ID),m_pD3DDevice(pDevice),m_Params(pDevice),m_sMain(fMain.c_str()),
		m_pConstTable(NULL),m_pPixelShader(NULL),m_isActive(false)
{
}

CDx9FragmentProgram::~CDx9FragmentProgram()
{
	release(); ///....诶
}

bool CDx9FragmentProgram::load(const GString str)
{
	return load(str.c_str(),str.length());
}

bool CDx9FragmentProgram::load(const char* buf,size_t fsize)
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
		string message = "D3D9 Pixel Shader拼接不成功 Errors:\n";
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

bool CDx9FragmentProgram::release(void)
{
	SafeRelease(m_pPixelShader);
	m_isLoad = false;
	return true;
}

bool CDx9FragmentProgram::bind(void)
{
	if(!m_isLoad)
	{
		LPD3DXBUFFER errors;
		LPD3DXBUFFER pBuffer;
		DWORD dwShaderFlags = D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_PREFER_FLOW_CONTROL;
#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DXSHADER_DEBUG;
#endif
		HRESULT hr = D3DXCompileShader(
			m_cCode.c_str(),
			static_cast<uint>(m_cCode.size()),
			NULL,
			DX9_ShaderInclude::GetInst(),
			m_sMain.c_str(),
			"ps_2_0",
			dwShaderFlags,
			&pBuffer,
			&errors,
			&m_pConstTable);
		if (FAILED(hr))
		{
			hr = D3DXCompileShader(
				m_cCode.c_str(),
				static_cast<uint>(m_cCode.size()),
				NULL,
				DX9_ShaderInclude::GetInst(),
				m_sMain.c_str(),
				"ps_3_0",
				dwShaderFlags,
				&pBuffer,
				&errors,
				&m_pConstTable);
			if (FAILED(hr))
			{
				string message = "D3D9 Pixel Shader创建不成功 Errors:\n";
				message.append(static_cast<const char*>(errors->GetBufferPointer()));
				errors->Release();
				OutputDebugString(message.c_str());
				LogErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),message);
				return false;
			}
		}

		SafeRelease(errors);

		hr = m_pD3DDevice->CreatePixelShader((DWORD*)pBuffer->GetBufferPointer(),&m_pPixelShader);
		SafeRelease(pBuffer);
		if ( SUCCEEDED(hr))
		{
			m_Params.Create(m_pConstTable);
			m_isLoad = true;
			m_isNeedUpdateParameters = true;
		}
		else
		{
			stringstream ss;
			ss << "CreatePixelShader失败: " << DXGetErrorDescription(hr);
			LogErr(GraphicErr::GetErrTypeStr(ShaderInit_Err), ss.str());
			return false;
		}
	}

	if(!m_isActive)
	{
		m_isActive = true;
		return SUCCEEDED(m_pD3DDevice->SetPixelShader(m_pPixelShader));
	}
	return true;
}

void CDx9FragmentProgram::reset(void)
{
	m_pD3DDevice = GetD3DDevice();
	m_Params.reset(m_pD3DDevice);
};

bool CDx9FragmentProgram::unbind(void)
{
	m_isActive =false;
	m_pD3DDevice->SetPixelShader(NULL);
	return true;
}

//bool CDx9FragmentProgram::_load()
//{
//	
//}