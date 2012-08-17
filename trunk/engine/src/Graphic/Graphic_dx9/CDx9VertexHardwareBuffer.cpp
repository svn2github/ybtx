#include "stdafx.h"
#include "CDX9VertexHardwareBuffer.h"
#include "CDX9RenderPipeline.h"
#include "CDx9MainWindow.h"
#include "BaseHelper.h"
#include "GraphicClassDefine.h"
#include "CShadowBuffer.h"
#include <Dxerr.h>

DX9VertexHardwareBuffer::DX9VertexHardwareBuffer(size_t vertexSize, size_t numVertices,
        Usage usage, void* pInitData, bool useSystemMem, bool useShadowBuffer)
		:CVertexHardwareBuffer(vertexSize,numVertices,usage,useSystemMem,useShadowBuffer)
		,m_pD3DBuffer(NULL)
{
	Ast( ((usage&HBU_STATIC) == 0 || pInitData != NULL)&&"HBU_STATIC 一定要有初始化数据" );
	m_D3DPool = useSystemMem? D3DPOOL_SYSTEMMEM : ((usage & HBU_DYNAMIC)?D3DPOOL_DEFAULT : D3DPOOL_MANAGED);

	if(pInitData)
	{
		if(!m_UseShadowBuffer)
			m_pShadowBuffer =  new ShadowBuffer(m_Size, HBU_DYNAMIC);
		m_pShadowBuffer->write(0,m_Size,pInitData,true);
		m_ShadowUpdated = true;
	}
	_imp_create();
}


DX9VertexHardwareBuffer::~DX9VertexHardwareBuffer()
{
	SafeRelease(m_pD3DBuffer);
}

void DX9VertexHardwareBuffer::freeRes()
{
	SQR_TRY
	{
		if (m_D3DPool == D3DPOOL_DEFAULT)
			SafeRelease(m_pD3DBuffer);
	}
	SQR_CATCH(exp)
	{
		exp.AppendType(GraphicErr::GetErrInfo(BufferUse_Err));
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

void DX9VertexHardwareBuffer::updateRes()
{
	SQR_TRY
	{
		if (m_D3DPool == D3DPOOL_DEFAULT && m_pD3DBuffer==NULL)
		{
			if( _imp_create() && (m_Usage&HBU_STATIC) == 0 )
				CVertexHardwareBuffer::updateRes();
		}

	}
	SQR_CATCH(exp)
	{
		exp.AppendType(GraphicErr::GetErrInfo(BufferUse_Err));
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

//----------------------------------------------------------------------------------
void* DX9VertexHardwareBuffer::_lock(size_t offset, size_t length, LockOptions options)
{
	if(!IsValid() && !_imp_create())
		return NULL;

	void* pBuf = 0;
	HRESULT hr = m_pD3DBuffer->Lock(
	                 static_cast<uint>(offset),
	                 static_cast<uint>(length),
	                 &pBuf,
	                 DX9RenderPipeline::get(options, m_Usage));

	if (FAILED(hr))
	{
		string szDevState = "ok";
		HRESULT hrDevice = GetD3DDevice()->TestCooperativeLevel();
		if ( hrDevice == D3DERR_DEVICELOST )
			szDevState = "lost";
		else if ( hrDevice == D3DERR_DEVICENOTRESET )
			szDevState = "not reset";

		stringstream ss;
		ss << "不能锁定顶点缓存 options:" << options
			<< "dev state:" << szDevState << " " << DXGetErrorDescription(hr);
		GenErr(GraphicErr::GetErrTypeStr(BufferUse_Err), ss.str().c_str());
	}
	return pBuf;
}

void DX9VertexHardwareBuffer::_unlock(void)
{
	if(m_pD3DBuffer==NULL)
		return;

	m_pD3DBuffer->Unlock();
}

bool DX9VertexHardwareBuffer::IsValid(void)
{
	return m_pD3DBuffer != NULL;
}

bool DX9VertexHardwareBuffer::_imp_create(void)
{
	if(IsValid())
		return true;

	HRESULT hr =  GetD3DDevice()->CreateVertexBuffer(
		static_cast<uint>(m_Size),
		DX9RenderPipeline::get(m_Usage),
		0, // 不对其FVF进行设定
		m_D3DPool,
		&m_pD3DBuffer,
		NULL);

	if (FAILED(hr))
	{
		//GString msg = DXGetErrorDescription9(hr);
		string ErrStr = "DX9VertexHardwareBuffer创建失败";
		GraphicErrType ErrType = BufferUse_Err;
		switch ( hr )
		{
		case D3DERR_NOTAVAILABLE:
			ErrStr += ": D3DERR_NOTAVAILABLE";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			ErrStr += ": D3DERR_OUTOFVIDEOMEMORY";
			ErrType = Memory_Err;
			break;
		case D3DERR_INVALIDCALL:
			ErrStr += ": D3DERR_INVALIDCALL";
			break;
		case D3DXERR_INVALIDDATA:
			ErrStr += ": D3DXERR_INVALIDDATA";
			break;
		case E_OUTOFMEMORY:
			ErrStr += ": E_OUTOFMEMORY";
			ErrType = Memory_Err;
			break;
		default:
			ErrStr += DXGetErrorString(hr);
			ErrStr += " - ";
			ErrStr += DXGetErrorDescription(hr);
		}
		m_pD3DBuffer = NULL; 
		GfkLogErr( GraphicErr::GetErrTypeStr(ErrType) , ErrStr);
		return false;
	}

	if(m_pD3DBuffer && m_pShadowBuffer && m_ShadowUpdated == true)
	{
		void* pBuf;
		HRESULT hr = m_pD3DBuffer->Lock( static_cast<uint>(0)
			,static_cast<uint>(m_Size)
			,&pBuf
			,DX9RenderPipeline::get(HBL_NO_OVERWRITE,m_Usage));
		
		if (SUCCEEDED(hr))
		{
			memcpy(pBuf, m_pShadowBuffer->lock(HBL_READ_ONLY),m_Size);
			m_pShadowBuffer->unlock();
			m_pD3DBuffer->Unlock();	

			if(!m_UseShadowBuffer)
				GfkSafeDelete(m_pShadowBuffer);

			return true;
		}
		else
		{
			SafeRelease(m_pD3DBuffer);
			GenErr(GraphicErr::GetErrTypeStr(BufferUse_Err),"VertexBuffer 初始化数据失败");	
			return  false;
		}
	}

	return true;	
}