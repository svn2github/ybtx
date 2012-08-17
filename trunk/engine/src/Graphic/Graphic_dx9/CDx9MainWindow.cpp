#include "stdafx.h"

#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CHardwareBufferMgr.h"
#include "CGraphic.h"

#include "CDX9MainWindow.h"
#include "CDx9Adapter.h"
#include "CDx9DeviceCapacity.h"
#include "CDx9GpuProgramMgr.h"
#include "CDx9RenderPipeline.h"
#include "CDx9Texture.h"
#include "CDx9TexFmtMapper.h"

const D3DFORMAT g_DisFormat[DF_COUNT] =
{
	D3DFMT_X8R8G8B8,
	D3DFMT_A8R8G8B8,
	D3DFMT_X1R5G5B5,
	D3DFMT_R5G6B5,
	D3DFMT_UNKNOWN,
};

LPDIRECT3D9			DX9MainWindowTarget::m_lpD3d		= 0;
LPDIRECT3DDEVICE9	DX9MainWindowTarget::m_lpD3DDevice	= 0;

DX9MainWindowTarget::DX9MainWindowTarget()
		: m_bResetGuard(false)
		, m_bForceReset(false)
{
}

DX9MainWindowTarget::~DX9MainWindowTarget()
{
	ReleaseUtilRes();

	delete CHardwareCapacity::GetInst();
	SafeRelease(m_lpD3DDevice);
	SafeRelease(m_lpD3d);
}

////////////////////////////////////////////////////////////////////////
// 功能：创建应用程序所需所有资源
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
HRESULT DX9MainWindowTarget::CreateUtilRes()
{
	SQR_TRY
	{
		CRenderPipeline::GetInst()->InitPipeline();

		typedef GSet<CWindowTarget*>	TargetList;
		TargetList::iterator tgt_it		= CGraphic::GetInst()->m_RenderWindowList.begin();
		TargetList::iterator tgt_eit	= CGraphic::GetInst()->m_RenderWindowList.end();
		for ( ; tgt_it != tgt_eit; ++tgt_it )
		{
			SQR_TRY
			{
				(*tgt_it)->ReCreate();
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("存在CWindowTarget ReCreate失败的情况");
				GfkLogExp(exp);
			}
			SQR_TRY_END
		}

		typedef GSet<CRenderTexture*>	RenderTextureList;
		RenderTextureList::iterator rt_it	= CGraphic::GetInst()->m_depthTexList.begin();
		RenderTextureList::iterator rt_eit	= CGraphic::GetInst()->m_depthTexList.end();
		for ( ; rt_it != rt_eit; ++rt_it )
		{
			SQR_TRY
			{
				(*rt_it)->ReCreate();
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("存在CRenderTexture ReCreate失败的情况");
				GfkLogExp(exp);
			}
			SQR_TRY_END
		}

		CHardwareBufferMgr::GetInst()->Update();
		((CDx9GpuProgramMgr*)CGpuProgramMgr::GetInst())->resetDevice();
		ReCreate();

		IDirect3DDevice9* d3dDevice = GetD3DDevice();
		d3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00bfbfbf );
		d3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		d3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		d3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, 0 );
		d3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
		d3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		d3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		d3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		d3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：释放应用程序所有资源
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
HRESULT DX9MainWindowTarget::ReleaseUtilRes()
{
	SQR_TRY
	{
		CGraphic::GetInst()->m_EffectNpcVB.ResetStack();
		CGraphic::GetInst()->m_EffectSceneVB.ResetStack();
		CGraphic::GetInst()->m_DynamicVB.ResetStack();
		CGraphic::GetInst()->m_TempVB.ResetStack();

		((CDx9GpuProgramMgr*)CGpuProgramMgr::GetInst())->lostDevice();
		CHardwareBufferMgr::GetInst()->Release();
		Reset();

		CRenderPipeline::GetInst()->ReleasePipeline();

		for_each(CGraphic::GetInst()->m_RenderWindowList.begin(), CGraphic::GetInst()->m_RenderWindowList.end(), mem_fun(&CRenderTexture::Reset));
		for_each(CGraphic::GetInst()->m_depthTexList.begin()    , CGraphic::GetInst()->m_depthTexList.end()   , mem_fun(&CRenderTexture::Reset));
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	return 0;
}


bool DX9MainWindowTarget::Update()
{
	m_rcViewPort.left	= 0;
	m_rcViewPort.top	= 0;
	m_rcViewPort.right	= m_PresentParam.BackBufferWidth;
	m_rcViewPort.bottom	= m_PresentParam.BackBufferHeight;
	return CMainWindowTarget::Update();
}

void DX9MainWindowTarget::RestoreSize( bool useClientRect )
{
	if (m_bResetGuard)
		return;

	RECT rect;
	GetClientRect( m_RenderSettings.GetWindowHandle(), &rect );
	if ( !m_bForceReset && m_PresentParam.BackBufferWidth == rect.right && m_PresentParam.BackBufferHeight == rect.bottom )
		return;
	if (useClientRect)
	{
		m_rcViewPort.right = rect.right;
		m_rcViewPort.bottom = rect.bottom;
		m_RenderSettings.SetWidth(rect.right);
		m_RenderSettings.SetHeight(rect.bottom);
	}
	else if (!m_beFullScreen)
	{
		float viewRatio = m_RenderSettings.GetOrgRatio();
		m_RenderSettings.SetWidth(rect.right);
		m_RenderSettings.SetHeight(min(uint(rect.right * viewRatio), uint(rect.bottom)));
	}

	AdjustPresentParams();
	ReleaseUtilRes();

	HRESULT hr;
	do 
	{
		if( FAILED( hr = m_lpD3DDevice->Reset( &m_PresentParam ) ) )
		{
			Sleep(1000);
		}
	} while (FAILED(hr));

	ResetAntiAliasQuality();

	m_lpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xFF000000, 1.0f, 8 );
	CreateUtilRes();
	CRenderPipeline::GetInst()->ResetRenderState();
	m_bForceReset = false;
}

void DX9MainWindowTarget::ReSaveSize( uint w, uint h )
{
	m_RenderSettings.SetWidth(w);
	m_RenderSettings.SetHeight(h);

	m_uHeight = m_RenderSettings.GetHeight();
	m_uWidth = m_RenderSettings.GetWidth();
}
void DX9MainWindowTarget::Reset()
{
	if (NULL != m_pBloomTarget)
		m_pBloomTarget->Release();
}

bool DX9MainWindowTarget::ReCreate(void)
{
	Update();

	if(m_pRenderGroup)
		m_pRenderGroup->GetLightSystem()->Reset();
	if (NULL != m_pBloomTarget)
	{
		m_pBloomTarget->Create();
		m_pBloomTarget->UpdateParmas();
	}

	m_lpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0 );
	m_lpD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,
	                              (D3DMULTISAMPLE_NONE != m_PresentParam.MultiSampleType) ? TRUE : FALSE);
	return true;
}

namespace sqr
{
	extern bool	MakeDirectory( const wchar_t* szDirectory );
}

wstring DX9MainWindowTarget::DoScreenShot(const wchar_t* szAliasPath, const wchar_t * szFilename)
{
	ID3DSurfacePtr tmp;
	ID3DSurfacePtr back;
	V(m_lpD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, back.address()));
	D3DSURFACE_DESC d;
	back->GetDesc(&d);
	V(m_lpD3DDevice->CreateOffscreenPlainSurface(d.Width,d.Height,D3DFMT_X8R8G8B8,
	        D3DPOOL_SYSTEMMEM, tmp.address(), NULL));
	V(D3DXLoadSurfaceFromSurface(tmp, NULL, NULL, back, NULL, NULL, D3DX_FILTER_POINT, 0));

	wstring save_path = wstring(szAliasPath) + szFilename;
	MakeDirectory(save_path.substr(0, save_path.rfind(L"/")).c_str());

	V(D3DXSaveSurfaceToFileW(save_path.c_str(), D3DXIFF_PNG, tmp, NULL, NULL));

	return save_path;
}

LPDIRECT3D9 DX9MainWindowTarget::GetD3d9()
{
	return m_lpD3d;
}

LPDIRECT3DDEVICE9 DX9MainWindowTarget::GetDevice()
{
	return m_lpD3DDevice;
}

HRESULT DX9MainWindowTarget::ChangeRenderSettings(const RenderSettings& settings)
{
	if (m_RenderSettings == settings)
		return S_OK;
	SetBeProtected(true);
	m_RenderSettings = settings;
	HWND hWnd = m_RenderSettings.GetWindowHandle();

	int posX = 0, posY = 0;
	int wndWidth = RenderSettings::GetScreenWidth();
	int wndHeight = RenderSettings::GetScreenHeight();

	if ( m_RenderSettings.GetUseDef() )
	{
		m_RenderSettings.SetHeight(m_uHeight);
		m_RenderSettings.SetWidth(m_uWidth);
	}
	m_uHeight = m_RenderSettings.GetHeight();
	m_uWidth = m_RenderSettings.GetWidth();

	if (m_RenderSettings.GetWidth() != 0)
		m_RenderSettings.SetOrgRatio((float)m_RenderSettings.GetHeight()/(float)m_RenderSettings.GetWidth());
	// ratio to presume upon
	if (m_RenderSettings.IsWindowed())
		m_beFullScreen = false;
	else if (!m_RenderSettings.IsWindowed())
		m_beFullScreen = true;

	// hide window, or there will be a "ghost window"

	if( m_RenderSettings.GetIsCtrlWindow() )
	{
		ShowWindow(hWnd, SW_HIDE);
		// reset window style
		DWORD style = WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
		if (m_RenderSettings.IsWindowed())
		{
			style = WS_OVERLAPPEDWINDOW;
			RECT rect = {0, 0, m_RenderSettings.GetWidth(), m_RenderSettings.GetHeight()};
			AdjustWindowRect(&rect, style, FALSE);
			RECT workArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
			wndWidth = min(rect.right - rect.left, workArea.right - workArea.left);
			wndHeight = min(rect.bottom - rect.top, workArea.bottom - workArea.top);
			posX = workArea.left + (workArea.right - workArea.left - wndWidth) / 2;
			posY = workArea.top + (workArea.bottom - workArea.top - wndHeight) / 2;
			// resize the render setting
			if ( (rect.right - rect.left) > (workArea.right - workArea.left) )
			{
				int dis = (rect.right - rect.left) - (workArea.right - workArea.left);
				m_RenderSettings.SetWidth(m_uWidth - dis);
			}
			if ( (rect.bottom - rect.top) > (workArea.bottom - workArea.top) )
			{
				int dis = (rect.bottom - rect.top) - (workArea.bottom - workArea.top);
				m_RenderSettings.SetHeight(m_uHeight - dis);
			}
			
		}
		m_bResetGuard = true;
		
		SetWindowLong(hWnd, GWL_STYLE, style);
		SetWindowPos(hWnd, 0, posX, posY, wndWidth, wndHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
		m_bResetGuard = false;
		// show window
		ShowWindow(hWnd, SW_NORMAL);
		UpdateWindow(hWnd);
		// reset device
		SetBeProtected(false);
		PostMessage(hWnd, WM_SIZE, SIZE_RESTORED, 0);
	}
	else
		RestoreSize( true );

	m_bForceReset = true;
	return S_OK;
}

HRESULT DX9MainWindowTarget::CreateSuitableDevice()
{
	const D3DDISMD& DisplayModel = AdapterList::GetInst()->GetCurrentAdapter().GetDesktopMode();

	// Set default settings
	uint AdapterToUse=D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
#ifdef _DEBUG
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (uint adapter=0;adapter<m_lpD3d->GetAdapterCount();adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		m_lpD3d->GetAdapterIdentifier(adapter,0,&Identifier);
		if (strstr(Identifier.Description,"PerfHUD") != NULL)
		{
			AdapterToUse=adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}
#else
 	D3DPERF_SetOptions( DWORD(1) ); //禁用PIX
#endif

	//bool hwTL = CHardwareCapacity::GetInst()->IsSupportHwTandL();
	DWORD extraFlags = D3DCREATE_FPU_PRESERVE;
	DWORD CreateFlag[3] = {D3DCREATE_HARDWARE_VERTEXPROCESSING,D3DCREATE_MIXED_VERTEXPROCESSING,D3DCREATE_SOFTWARE_VERTEXPROCESSING};

	HRESULT hr;
	//这是比较好的情况
	for( ; DeviceType <= D3DDEVTYPE_REF;  )
	{
		for( int i = 0; i<3; ++i )
		{
			for ( int32 j = 0; j < DF_COUNT; j++ )
			{

				m_PresentParam.BackBufferFormat = g_DisFormat[j];

				if( FAILED( m_lpD3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,
					DeviceType,  m_PresentParam.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
					D3DRTYPE_SURFACE, D3DFMT_D24S8 )))
				{
					SetClearModeType(CMT_CLEAR_TARGE_ZBUFFER);

					if( FAILED( m_lpD3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,
						DeviceType,  m_PresentParam.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
						D3DRTYPE_SURFACE, D3DFMT_D32 )))
					{
						// 这是世界上现存的最垃圾显卡
						m_PresentParam.AutoDepthStencilFormat = D3DFMT_D16;
					}
					else
						m_PresentParam.AutoDepthStencilFormat = D3DFMT_D32;
				}
				else
				{
					SetClearModeType(CMT_CLEAR_ALL);
					// Woohoo!
					if( SUCCEEDED( m_lpD3d->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, DeviceType,
						m_PresentParam.BackBufferFormat, m_PresentParam.BackBufferFormat, D3DFMT_D24S8 ) ) )
					{
						m_PresentParam.AutoDepthStencilFormat = D3DFMT_D24S8; 
					} 
					else 
						m_PresentParam.AutoDepthStencilFormat = D3DFMT_D24X8; 
				}

				while(1)
				{
					if ( SUCCEEDED(hr = m_lpD3d->CreateDevice( AdapterToUse, DeviceType,
						m_RenderSettings.GetWindowHandle(),
						CreateFlag[i] | extraFlags, 
						&m_PresentParam, &m_lpD3DDevice ) ) )
						return 0;

					if( hr!= D3DERR_DEVICELOST)
						break;

					Sleep(500);
				}
			}
		}
		if(DeviceType == D3DDEVTYPE_HAL)
		{
			GString ErrStr = "硬件加速启动失败::启动游戏时请不要锁屏";
			::MessageBox(m_PresentParam.hDeviceWindow,ErrStr.c_str(),"显卡异常",0);
		}
		DeviceType = (D3DDEVTYPE)(((DWORD)DeviceType) + 1);
	}

	//string ErrMsg;
	//ErrMsg += "D3D Device创建失败:\n ErrMsg:";
	//switch(hr)
	//{
	//case D3DERR_DEVICELOST:
	//	ErrMsg += "D3DERR_DEVICELOST";
	//	break;
	//case D3DERR_INVALIDCALL:
	//	ErrMsg += "D3DERR_INVALIDCALL";
	//	break;
	//case D3DERR_NOTAVAILABLE:
	//	ErrMsg += "D3DERR_NOTAVAILABLE";
	//	break;
	//case D3DERR_OUTOFVIDEOMEMORY:
	//	ErrMsg += "D3DERR_OUTOFVIDEOMEMORY";
	//	break;
	//default:
	//	ErrMsg += "未知错误";
	//}
	//GfkDrvLog(GraphicErr::GetErrTypeStr(Create_Err),ErrMsg);

	//这是无法接受的情况
	return GERROR;
}

void DX9MainWindowTarget::ResetAntiAliasQuality()
{
	// check is valid quality
	EAntiAliasQuality quality = m_RenderSettings.GetAntiAliasQuality();
	if(CHardwareCapacity::GetInst()->IsCanAntiAlias(type_cast<_texFORMAT>(m_PresentParam.BackBufferFormat)) && quality>AA_None )
	{
		m_PresentParam.SwapEffect	= 	D3DSWAPEFFECT_DISCARD;
		HRESULT hr;
		while( quality != AA_None)
		{
			m_PresentParam.MultiSampleType = (D3DMULTISAMPLE_TYPE)quality;
			if( SUCCEEDED( hr = m_lpD3DDevice->Reset( &m_PresentParam ) ) )
				return;
			
			if( hr== D3DERR_DEVICELOST)
			{
				Sleep(100);
				continue;
			}
			quality = EAntiAliasQuality(((UINT)quality) - 2);
		}

		m_PresentParam.MultiSampleType	= D3DMULTISAMPLE_NONE;
		do 
		{
			if( FAILED( hr = m_lpD3DDevice->Reset( &m_PresentParam ) ) )
			{
				Sleep(100);
			}
		} while (FAILED(hr));
	}
}


//ECreateError DX9MainWindowTarget::CreateD3d( HANDLE hDev, const D3DPRS_PARAM& PresentParam )
//{
//	m_lpD3DDevice = (LPDIRDEV)hDev;
//	m_lpD3DDevice->GetDirect3D( &m_lpD3d );
//	AdapterList::GetInst()->EnumerateApdaters(m_lpD3d);
//	AdapterList::GetInst()->SetCurrentAdapterIndex(D3DADAPTER_DEFAULT);
//	m_PresentParam = PresentParam;
//	m_RenderSettings.SetWindowHandle(PresentParam.hDeviceWindow);
//	m_RenderSettings.SetWidth(PresentParam.BackBufferWidth);
//	m_RenderSettings.SetHeight(PresentParam.BackBufferHeight);
//
//	CDx9DeviceCapacity* caps = new CDx9DeviceCapacity;
//	caps->adapter = const_cast<Dx9Adapter*>(&AdapterList::GetInst()->GetCurrentAdapter());
//	caps->d3d9 = m_lpD3d;
//	caps->d3d9->GetDeviceCaps(caps->adapter->GetAdapterNo(), D3DDEVTYPE_HAL, &caps->d3dcaps);
//	caps->Initialize();
//	CGraphic::GetInst()->Initialize();
//
//	return eCE_Null;
//
//}

bool DX9MainWindowTarget::Initialize( const RenderSettings& settings )
{
	m_lpD3d = Direct3DCreate9(D3D_SDK_VERSION);
	if ( !m_lpD3d )
		return false;
	m_RenderSettings = settings;

	AdapterList::GetInst()->EnumerateApdaters(m_lpD3d);
	AdapterList::GetInst()->SetCurrentAdapterIndex(D3DADAPTER_DEFAULT);
	new CDx9DeviceCapacity;

	CDx9DeviceCapacity* caps = static_cast<CDx9DeviceCapacity*>(CHardwareCapacity::GetInst());
	caps->adapter = const_cast<Dx9Adapter*>(&AdapterList::GetInst()->GetCurrentAdapter());
	caps->d3d9 = m_lpD3d;
	caps->d3d9->GetDeviceCaps(caps->adapter->GetAdapterNo(), D3DDEVTYPE_HAL, &caps->d3dcaps);
	caps->Initialize();	

	AdjustPresentParams();

	if ( FAILED( CreateSuitableDevice() ) )
		return false;
	ResetAntiAliasQuality();

	new CDx9TexFmtMapper();
	CGraphic::GetInst()->Initialize();

	CreateUtilRes();
	return true;
}

void DX9MainWindowTarget::AdjustPresentParams()
{
	D3DFORMAT format = g_DisFormat[m_RenderSettings.GetFormat()];
	D3DFORMAT tFormat = m_PresentParam.AutoDepthStencilFormat;
	ZeroMemory( &m_PresentParam, sizeof(m_PresentParam) );

	// keep the viewport's ratio equals to settings
	int bufferWidth = m_RenderSettings.GetBufferWidth();
	int bufferHeight = m_RenderSettings.GetBufferHeight();
	HWND hWnd = m_RenderSettings.GetWindowHandle();
	RECT rect;
	GetClientRect(hWnd, &rect);
	float wndRatio = float(rect.bottom) / float(rect.right);
	float viewRatio = m_RenderSettings.GetOrgRatio();

	if ( isLockAspectRatio() && !gIsEqual(wndRatio, viewRatio) && wndRatio >0.0f )
	{
		if (wndRatio > viewRatio)
			bufferHeight = int(bufferWidth * wndRatio + 0.5f);
		else
			bufferWidth = int(bufferHeight / wndRatio + 0.5f);
	}

	m_PresentParam.Windowed						= TRUE;
	m_PresentParam.hDeviceWindow				= hWnd;
	m_PresentParam.BackBufferWidth				= bufferWidth;
	m_PresentParam.BackBufferHeight				= bufferHeight;
	m_PresentParam.BackBufferFormat				= format;
	m_PresentParam.AutoDepthStencilFormat		= tFormat;
	m_PresentParam.BackBufferCount				= m_RenderSettings.GetBackBufferNum();
	m_PresentParam.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParam.SwapEffect					= (m_PresentParam.Windowed && m_PresentParam.BackBufferCount == 1)?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	m_PresentParam.EnableAutoDepthStencil		= TRUE;
	//m_PresentParam.AutoDepthStencilFormat		= D3DFMT_D24S8;
	m_PresentParam.PresentationInterval			= m_RenderSettings.IsVSync()?D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// lock viewport's ratio
	if (isLockAspectRatio())
		LockAspectRatio(1.0f / viewRatio);
}

//void DX9MainWindowTarget::SetBackBufferNum( UINT Num )
//{
//	m_PresentParam.BackBufferCount = Num>D3DPRESENT_BACK_BUFFERS_MAX?D3DPRESENT_BACK_BUFFERS_MAX:Num;
//	if (NULL != m_lpD3DDevice)
//	{
//		m_bForceReset = true;
//		this->RestoreSize();
//	}
//}
//
//UINT DX9MainWindowTarget::GetBackBufferNum()
//{
//	return m_PresentParam.BackBufferCount;
//}

//void DX9MainWindowTarget::SetAntiAliasQuality(EAntiAliasQuality quality)
//{
//	// check is valid quality
//	bool res = CHardwareCapacity::GetInst()->IsCanAntiAlias(type_cast<_texFORMAT>(m_PresentParam.BackBufferFormat));
//	if (!res)
//	{
//		m_PresentParam.MultiSampleType = D3DMULTISAMPLE_NONE;
//		return;
//	}
//	// apply
//	m_PresentParam.MultiSampleType = (D3DMULTISAMPLE_TYPE)quality;
//	// reset device
//	if (NULL != m_lpD3DDevice)
//	{
//		this->RestoreSize();
//	}
//}
EAntiAliasQuality DX9MainWindowTarget::GetAntiAliasQuality()
{
	return (EAntiAliasQuality)m_PresentParam.MultiSampleType;
}

HRESULT DX9MainWindowTarget::IsRenderActive()
{	
	if(m_lpD3DDevice == NULL)
		return GERROR;

	HRESULT hr;
	if ( FAILED( hr = m_lpD3DDevice->TestCooperativeLevel() ) )
	{
		// 当D3DDevice丢失时,要等到设备有效后方可继续
		if ( D3DERR_DEVICELOST==hr )
			return hr;

		// Reset D3DDevice
		if ( D3DERR_DEVICENOTRESET==hr )
		{
			//// 在Reset前必须要释放资源(一些容易Lost的如bmp等)
			//ReleaseUtilRes();
			//do 
			//{
			//	if( FAILED( hr = m_lpD3DDevice->Reset( &m_PresentParam ) ) )
			//	{
			//		Sleep(100);
			//	}
			//} while (FAILED(hr));

			//// 在Reset后必须要重新载入资源(由ReleaseResource释放掉的)
			////SetEnvironment();
			//if ( FAILED( CreateUtilRes() ) )
			//	GfkDrvLog( GraphicErr::GetErrTypeStr(RenderRuntime_Err),"设备丢失，无法重置" );
			if(!m_bForceReset)
			{
				PostMessage(m_RenderSettings.GetWindowHandle(), WM_SIZE, SIZE_RESTORED, 0);
				m_bForceReset = true;
			}
			return GRESET;
		}
		return GERROR;
	}
	return S_OK;
}

//------------------------------------------------------------------------------
vector<uint>
DX9MainWindowTarget::GetResolutionWidths()
{
	vector<uint> widths;
	const DisplayModeArray& modes = AdapterList::GetInst()->GetCurrentAdapter().GetDisplayModeArray();
	for (DisplayModeArray::const_iterator it = modes.begin(); it != modes.end(); ++it)
	{
		const DisplayMode& mode = *it;
		if (std::find(widths.begin(), widths.end(), mode.GetWidth()) == widths.end())
		{
			widths.push_back(mode.GetWidth());
		}
	}
	std::sort(widths.begin(), widths.end());

	return widths;
}
