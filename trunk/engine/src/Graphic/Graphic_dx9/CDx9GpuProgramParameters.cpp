#include "stdafx.h"
#include "CDx9GpuProgramParameters.h"
#include "BaseHelper.h"

CDx9GpuProgramParameters::CDx9GpuProgramParameters(LPDIRECT3DDEVICE9 pDevice)
		:m_pConstTable(NULL),m_pDevice(pDevice)
{
	AstMsg(pDevice, "Shader参数初始化错误");
}

CDx9GpuProgramParameters::~ CDx9GpuProgramParameters()
{
	release();
}

bool CDx9GpuProgramParameters::Create(LPD3DXBUFFER pMicrocode)
{
	HRESULT hr =D3DXGetShaderConstantTable(
	                (DWORD*)pMicrocode->GetBufferPointer(),
	                &m_pConstTable);
	if (FAILED(hr))
	{
		GenErr(GraphicErr::GetErrTypeStr(ShaderInit_Err),"Shader参数获取不成功...");
		//			return false;
	}
	return true;
}

void CDx9GpuProgramParameters::release(void)
{
	SafeRelease(m_pConstTable);
}

bool CDx9GpuProgramParameters::Create(LPD3DXCONSTANTTABLE pConstTable)
{
	if (NULL==pConstTable)
		return false;
	m_pConstTable = pConstTable;
	return true;
}

bool CDx9GpuProgramParameters::setBool(const string& handle,BOOL b)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetBool(m_pDevice,handle.c_str(),b));
	else
		return false;
}

bool CDx9GpuProgramParameters::setBoolArray(const string& handle,const BOOL *pb,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetBoolArray(m_pDevice,handle.c_str(),pb,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setFloat(const string& handle,float f)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetFloat(m_pDevice,handle.c_str(),f));
	else
		return false;
}

bool CDx9GpuProgramParameters::setFloatArray(const string& handle,const float *pf,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetFloatArray(m_pDevice,handle.c_str(),pf,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setInteger(const string& handle,int i)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetInt(m_pDevice,handle.c_str(),i));
	else
		return false;
}

bool CDx9GpuProgramParameters::setIntegerArray(const string& handle,const int *pi,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetIntArray(m_pDevice,handle.c_str(),pi,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setMatrix(const string& handle,CMatrix* pm)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetMatrix(m_pDevice,handle.c_str(),(D3DXMATRIX*)pm));
	else
		return false;
}

bool CDx9GpuProgramParameters::setMatrixArray(const string& handle,CMatrix* pm,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetMatrixArray(m_pDevice,handle.c_str(),(D3DXMATRIX*)pm,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setMatrixPointerArray(const string& handle,CMatrix** pm,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetMatrixPointerArray(m_pDevice,handle.c_str(),(const D3DXMATRIX**)pm,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setVector(const string& handle,CVector3f* pv)
{
	if ( m_pConstTable )
	{
		D3DXVECTOR4 v;
		v.x = pv->x;
		v.y = pv->y;
		v.z = pv->z;
		v.w = 1.0f;
		return SUCCEEDED(m_pConstTable->SetVector(m_pDevice,handle.c_str(),&v));
	}
	return 
		false;
}

bool CDx9GpuProgramParameters::setVectorArray(const string& handle,CVector3f* pv,size_t count)
{
	if ( m_pConstTable && count < 64 )
	{
		D3DXVECTOR4 v[64];
		for (size_t i=0;i<count;i++)
		{
			v[i].x = pv[i].x;
			v[i].y = pv[i].y;
			v[i].z = pv[i].z;
			v[i].w = 1.0f;
		}
		return SUCCEEDED(m_pConstTable->SetVectorArray(m_pDevice,handle.c_str(),v,count));
	}
	else
		return false;
}

bool CDx9GpuProgramParameters::setD3DVector(const string& handle,D3DXVECTOR4* pv)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetVector(m_pDevice,handle.c_str(),pv));
	else
		return false;
}

bool CDx9GpuProgramParameters::setD3DVectorArray(const string& handle,D3DXVECTOR4* pv,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetVectorArray(m_pDevice,handle.c_str(),pv,count));
	else
		return false;
}

bool CDx9GpuProgramParameters::setValue(const string& handle,void* p,size_t count)
{
	if ( m_pConstTable )
		return SUCCEEDED(m_pConstTable->SetValue(m_pDevice,handle.c_str(),p,count));
	else
		return false;
}