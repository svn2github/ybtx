//-------------------------------------------------
// 文件名：	CDX9IndexHardwareBuffer.h
// 说明：	索引硬件缓存<D3D9实现>
// 创建人：	董恺
// 创建时间：2008年6月27日
// 备注：	用于测试
//-------------------------------------------------
#include "stdafx.h"
#include "CDX9IndexHardwareBuffer.h"
#include "CDX9RenderPipeline.h"
#include "CDx9MainWindow.h"
#include "BaseHelper.h"
#include "CShadowBuffer.h"


DX9IndexHardwareBuffer::DX9IndexHardwareBuffer(size_t numIndexes, Usage usage,
        void* pInitData,bool useSystemMem, bool useShadowBuffer)
		:CIndexHardwareBuffer(numIndexes,usage,useSystemMem,useShadowBuffer)
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

DX9IndexHardwareBuffer::~DX9IndexHardwareBuffer()
{
	SafeRelease(m_pD3DBuffer);
}



void DX9IndexHardwareBuffer::freeRes()
{
	if (m_D3DPool == D3DPOOL_DEFAULT)
		SafeRelease(m_pD3DBuffer);
}

void DX9IndexHardwareBuffer::updateRes()
{
	if (m_D3DPool == D3DPOOL_DEFAULT && m_pD3DBuffer==NULL)
	{
		// Create the Index buffer
		if( _imp_create() && (m_Usage&HBU_STATIC) == 0 )
			CIndexHardwareBuffer::updateRes();
	}
}
//---------------------------------------------------------------------------------
void* DX9IndexHardwareBuffer::_lock(size_t offset, size_t length, LockOptions options)
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
		HRESULT hr = GetD3DDevice()->TestCooperativeLevel();
		if ( hr == D3DERR_DEVICELOST )
			szDevState = "lost";
		else if ( hr == D3DERR_DEVICENOTRESET )
			szDevState = "not reset";

		stringstream ss;
		ss << "不能锁定索引缓存 " << "dev state:" << szDevState;
		GenErr(GraphicErr::GetErrTypeStr(BufferUse_Err), ss.str().c_str());
	}
	return pBuf;
}
void DX9IndexHardwareBuffer::_unlock(void)
{
	if(m_pD3DBuffer==NULL)
		return;

	m_pD3DBuffer->Unlock();
}

bool DX9IndexHardwareBuffer::IsValid(void)
{
	return m_pD3DBuffer!=NULL;
}


bool DX9IndexHardwareBuffer::_imp_create(void)
{
	if(IsValid())
		return true;

	HRESULT hr =  GetD3DDevice()->CreateIndexBuffer(
		static_cast<uint>(m_Size),
		DX9RenderPipeline::get(m_Usage),
		D3DFMT_INDEX16,
		m_D3DPool,
		&m_pD3DBuffer,
		NULL);

	if (FAILED(hr))
	{
		//D3DERR_INVALIDCALL, D3DERR_OUTOFVIDEOMEMORY, D3DXERR_INVALIDDATA, E_OUTOFMEMORY. 
		//GString msg = DXGetErrorDescription9(hr);
		string ErrStr = "DX9IndexHardwareBuffer创建失败";
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
			ErrStr += ": CreateIndexBuffer failed with unknown error";
		}
		m_pD3DBuffer = NULL;
		GfkLogErr( GraphicErr::GetErrTypeStr(ErrType) , ErrStr);
		return false;
	}

	if(m_pD3DBuffer && m_pShadowBuffer)
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