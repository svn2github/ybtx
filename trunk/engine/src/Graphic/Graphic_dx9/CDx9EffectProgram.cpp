#include "stdafx.h"
#include "CDx9EffectProgram.h"
#include "BaseHelper.h"
#include "CDX9ShaderMacro.h"

CDx9EffectParameters::CDx9EffectParameters()
		:m_pD3DEffect(NULL)
{}

CDx9EffectParameters::~CDx9EffectParameters()
{
	release();
}

void CDx9EffectParameters::release(void)
{
	m_pD3DEffect = NULL;
}

void CDx9EffectParameters::Create(ID3DXEffect* pD3DEffect)
{
	m_pD3DEffect = pD3DEffect;
}

bool CDx9EffectParameters::setBool(const string& handle,BOOL b)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetBool(handle.c_str(),b));
}

bool CDx9EffectParameters::setBoolArray(const string& handle,const BOOL *pb,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetBoolArray(handle.c_str(),pb,count));
}

bool CDx9EffectParameters::setFloat(const string& handle,float f)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetFloat(handle.c_str(),f));
}

bool CDx9EffectParameters::setFloatArray(const string& handle,const float *pf,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetFloatArray(handle.c_str(),pf,count));
}

bool CDx9EffectParameters::setInteger(const string& handle,int i)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetInt(handle.c_str(),i));
}

bool CDx9EffectParameters::setIntegerArray(const string& handle,const int *pi,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetIntArray(handle.c_str(),pi,count));
}

bool CDx9EffectParameters::setMatrix(const string& handle,CMatrix* pm)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetMatrix(handle.c_str(),(D3DXMATRIX*)pm));
}

bool CDx9EffectParameters::setMatrixArray(const string& handle,CMatrix* pm,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetMatrixArray(handle.c_str(),(D3DXMATRIX*)pm,count));
}

bool CDx9EffectParameters::setMatrixPointerArray(const string& handle,CMatrix** pm,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetMatrixPointerArray(handle.c_str(),(const D3DXMATRIX**)pm,count));
}

bool CDx9EffectParameters::setVector(const string& handle,CVector3f* pv)
{
	Ast(m_pD3DEffect);
	D3DXVECTOR4 v;
	v.x = pv->x;
	v.y = pv->y;
	v.z = pv->z;
	v.w = 1.0f;
	return SUCCEEDED(m_pD3DEffect->SetVector(handle.c_str(),&v));
}

bool CDx9EffectParameters::setVectorArray(const string& handle,CVector3f* pv,size_t count)
{
	Ast(m_pD3DEffect && count<64);
	D3DXVECTOR4 v[64];
	for (size_t i=0;i<count;i++)
	{
		v[i].x = pv[i].x;
		v[i].y = pv[i].y;
		v[i].z = pv[i].z;
		v[i].w = 1.0f;
	}
	return SUCCEEDED(m_pD3DEffect->SetVectorArray(handle.c_str(),v,count));
}

bool CDx9EffectParameters::setValue(const string& handle,void* p,size_t count)
{
	Ast(m_pD3DEffect);
	return SUCCEEDED(m_pD3DEffect->SetValue(handle.c_str(),p,count));
}

//-----------------------------------------------------------------------------------------------------
CEffectProgram::CEffectProgram(DWORD ID,LPDIRECT3DDEVICE9 pDevice,const string& fName ,const string& fParams )
		:m_ID(ID),m_pD3DDevice(pDevice),m_sName(fName.c_str()),m_sParams(fParams.c_str()),
		m_pD3DEffect(NULL),m_iNumPass(0),m_isActive(false),m_iPass(0)\
{}

CEffectProgram::~CEffectProgram()
{
	release();
}

bool CEffectProgram::load(const GString str)
{
	return load(str.c_str(),str.length());
}

bool CEffectProgram::load(const char* buf,size_t fsize)
{

	if (m_isLoad)
		release();
	LPD3DXBUFFER errors = NULL;
	DWORD dwShaderFlags = D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_PREFER_FLOW_CONTROL;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	HRESULT hr = D3DXCreateEffectEx(
	                 m_pD3DDevice,
	                 buf,
	                 fsize,
	                 ((DX9_ShaderMacroMgr*)ShaderMacroMgr::GetInst())->GET_MACRO(m_ID), //pDefines
	                 DX9_ShaderInclude::GetInst(),
	                 m_sParams.c_str(),
	                 dwShaderFlags,
	                 NULL, //pPool
	                 &m_pD3DEffect,
	                 &errors);
	if (FAILED(hr))
	{
		string message = "D3D9 Effect Shader Errors:\n";
		if ( 0 != errors )
		{
			message.append(static_cast<const char*>(errors->GetBufferPointer()));
			errors->Release();
		}
		OutputDebugString(message.c_str());
		GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err), message);
		//return false;
	}
	SafeRelease(errors);
	if (m_hTechnique = m_pD3DEffect->GetTechniqueByName(m_sName.c_str()))
	{
		m_Params.Create(m_pD3DEffect);
		m_isNeedUpdateParameters = true;
		m_isLoad = true;
		return true;
	}
	release();
	return false;
}

bool CEffectProgram::release(void)
{
	SafeRelease(m_pD3DEffect);
	m_isLoad = false;
	m_iNumPass = 0;
	return true;
}

bool CEffectProgram::bind(void)
{
	Ast(m_pD3DEffect);
	if (!m_isActive)
	{
		m_isActive = true;
		m_pD3DEffect->SetTechnique( m_hTechnique );
		m_pD3DEffect->Begin( &m_iNumPass, 0 );
	}
	m_iPass = 0;
	return(SUCCEEDED(m_pD3DEffect->BeginPass(m_iPass)));
}

bool CEffectProgram::_next(void)
{
	Ast(m_pD3DEffect);
	m_pD3DEffect->EndPass();
	if (m_iNumPass>++m_iPass)
		return SUCCEEDED(m_pD3DEffect->BeginPass(m_iPass));
	return false;
}

bool CEffectProgram::unbind(void)
{
	Ast(m_pD3DEffect);
	m_isActive = false;
	return SUCCEEDED(m_pD3DEffect->End());
}

//------------------------------------------------------------------------------
bool CEffectProgram::commit()
{
	Ast(NULL != this->m_pD3DEffect);
	return SUCCEEDED(this->m_pD3DEffect->CommitChanges());
}