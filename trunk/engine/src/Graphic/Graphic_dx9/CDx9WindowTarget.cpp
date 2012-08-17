#include "stdafx.h"
#include "CDX9WindowTarget.h"
#include "CDx9MainWindow.h"
#include "CGraphic.h"
#include "CDx9DeviceCapacity.h"
#include "CRenderPipeline.h"
#include "BaseHelper.h"
#include "CColor.inl"

DX9WindowTarget::DX9WindowTarget(const RenderSettings& settings)
: m_pSwapChain(NULL)
, m_oldBackBuffer(NULL)
{
	m_pD3DDevice		= GetD3DDevice();
	m_ClearColor		= CColor::Green;
	ChangeRenderSettings(settings);
}

DX9WindowTarget::~DX9WindowTarget()
{
	Reset();
}

void DX9WindowTarget::Reset(void) 
{
	if (NULL != m_pBloomTarget)
		m_pBloomTarget->Release();

	SafeRelease(m_pSwapBack);
	SafeRelease(m_pSwapChain);
	m_DepthBuffer.release();
}

bool DX9WindowTarget::ReCreate(void)
{
	if(m_pRenderGroup)
		m_pRenderGroup->GetLightSystem()->Reset();

	if (NULL != m_pBloomTarget)
	{
		m_pBloomTarget->Create();
		m_pBloomTarget->UpdateParmas();
	}

	DX9MainWindowTarget* pMainWin = (DX9MainWindowTarget*)CMainWindowTarget::GetInst();
	HRESULT hr;

	m_PresentParam.BackBufferFormat			= pMainWin->GetD3D_PARAM().BackBufferFormat;
	m_PresentParam.AutoDepthStencilFormat	= pMainWin->GetD3D_PARAM().AutoDepthStencilFormat;

	while(1)
	{
		if ( SUCCEEDED(hr = m_pD3DDevice->CreateAdditionalSwapChain(
			&m_PresentParam, &m_pSwapChain) ) )
		{
			m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, m_pSwapBack.address());
			m_DepthBuffer.SetRect(m_rcViewPort);
			m_DepthBuffer.SetUseDepthBuffer(true);
			m_DepthBuffer.ReCreate();
			return true;
		}

		if( hr!= D3DERR_DEVICELOST)
			break;

		Sleep(500);
	}
	return false;
}

void DX9WindowTarget::BeginRender(void)
{
	if( m_oldBackBuffer == NULL )
	{
		m_pD3DDevice->BeginScene();
		CCamera* camera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
		m_pD3DDevice->GetViewport(&m_oldViewport);
		m_pD3DDevice->GetRenderTarget(0, m_oldBackBuffer.address());
		m_oldAspectRatio = camera->getAspectRatio();

		m_pD3DDevice->SetRenderTarget(0, m_pSwapBack);
		m_DepthBuffer.Begin(false);

		camera->setAspectRatio(float(m_rcViewPort.Width()) / float(m_rcViewPort.Height()));
		D3DVIEWPORT9 newViewport;
		ZeroMemory(&newViewport, sizeof(newViewport));
		newViewport.Width = m_rcViewPort.Width();
		newViewport.Height = m_rcViewPort.Height();
		newViewport.MaxZ = 1.0f;
		V(m_pD3DDevice->SetViewport(&newViewport));
	}
}

void DX9WindowTarget::EndRender(void)
{
	if( m_oldBackBuffer != NULL )
	{
		m_pD3DDevice->SetRenderTarget(0, m_oldBackBuffer);
		m_DepthBuffer.End();
		CGraphic::GetInst()->GetActiveRenderTarget()
			->GetCamera()->setAspectRatio(m_oldAspectRatio);
		m_pD3DDevice->SetViewport(&m_oldViewport);
		m_oldBackBuffer = NULL;
		m_pD3DDevice->EndScene();
		HRESULT hr = m_pSwapChain->Present(NULL, NULL, m_RenderSettings.GetWindowHandle(), NULL, 0);
		m_pD3DDevice->SetViewport(&m_oldViewport);
	}
}

void DX9WindowTarget::Render(void)
{
	if(m_pSwapChain==NULL)
		return;

	BeginRender();
	Clear();
	CRenderPipeline::GetInst()->SetCamera(m_pRenderGroup->GetCamera());
	CWindowTarget::Render();
	EndRender();
}

wstring DX9WindowTarget::DoScreenShot(const wchar_t* szAliasPath, const wchar_t * szFilename)
{
	ID3DSurfacePtr tmp;
	ID3DSurfacePtr back;
	V(m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, back.address()));
	D3DSURFACE_DESC d;
	back->GetDesc(&d);
	V(m_pD3DDevice->CreateOffscreenPlainSurface(d.Width,d.Height,D3DFMT_X8R8G8B8,
		D3DPOOL_SYSTEMMEM, tmp.address(), NULL));
	V(D3DXLoadSurfaceFromSurface(tmp, NULL, NULL, back, NULL, NULL, D3DX_FILTER_POINT, 0));

	wstring szSavePath = wstring(szAliasPath) + szFilename;

	V(D3DXSaveSurfaceToFileW(szSavePath.c_str(), D3DXIFF_PNG, tmp, NULL, NULL));

	return szSavePath;
}

HRESULT DX9WindowTarget::ChangeRenderSettings(const RenderSettings& settings)
{
	if (m_RenderSettings == settings)
		return S_OK;
	m_RenderSettings = settings;
	HWND hWnd = m_RenderSettings.GetWindowHandle();
	// ratio to presume upon
	if (m_RenderSettings.IsWindowed() && m_RenderSettings.GetWidth() != 0)
	{
		m_RenderSettings.SetOrgRatio((float)m_RenderSettings.GetHeight()/(float)m_RenderSettings.GetWidth());
		if (m_beFullScreen && m_RenderSettings.GetUseDef())
		{
			m_RenderSettings.SetHeight(m_uHeight);
			m_RenderSettings.SetWidth(m_uWidth);
		}
		else
		{
			m_uHeight = m_RenderSettings.GetHeight();
			m_uWidth = m_RenderSettings.GetWidth();
		}
		m_beFullScreen = false;
	}
	else if (!m_RenderSettings.IsWindowed())
	{
		if (m_beFullScreen)
		{
			m_RenderSettings.SetHeight(m_uHeight);
			m_RenderSettings.SetWidth(m_uWidth);
		}
		else
		{
			m_uHeight = m_RenderSettings.GetHeight();
			m_uWidth = m_RenderSettings.GetWidth();
		}
		m_RenderSettings.SetOrgRatio((float)m_RenderSettings.GetHeight()/(float)m_RenderSettings.GetWidth());
		m_beFullScreen = true;
	}

	// hide window, or there will be a "ghost window"
	//ShowWindow(hWnd, SW_HIDE);
	//// reset window style
	//int posX = 0, posY = 0;
	//int wndWidth = RenderSettings::GetScreenWidth();
	//int wndHeight = RenderSettings::GetScreenHeight();
	//DWORD style = WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
	//if (m_RenderSettings.IsWindowed())
	//{
	//	style = WS_OVERLAPPEDWINDOW;
	//	RECT rect = {0, 0, m_RenderSettings.GetWidth(), m_RenderSettings.GetHeight()};
	//	AdjustWindowRect(&rect, style, FALSE);
	//	RECT workArea;
	//	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	//	wndWidth = min(rect.right - rect.left, workArea.right - workArea.left);
	//	wndHeight = min(rect.bottom - rect.top, workArea.bottom - workArea.top);
	//	posX = workArea.left + (workArea.right - workArea.left - wndWidth) / 2;
	//	posY = workArea.top + (workArea.bottom - workArea.top - wndHeight) / 2;
	//}

	//m_bResetGuard = true;
	//SetWindowLong(hWnd, GWL_STYLE, style);
	//SetWindowPos(hWnd, 0, posX, posY, wndWidth, wndHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
	m_bResetGuard = false;
	//// show window
	//ShowWindow(hWnd, SW_NORMAL);
	//UpdateWindow(hWnd);
	//// reset device
	//PostMessage(hWnd, WM_SIZE, SIZE_RESTORED, 0);
	RestoreSize();
	m_bForceReset = true;

	return S_OK;
}

void DX9WindowTarget::RestoreSize( bool useClientRect )
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

	AdjustPresentParams();

	Reset();
	ReCreate();

	ResetAntiAliasQuality();

	m_bForceReset = false;
}

void DX9WindowTarget::ReSaveSize( uint w, uint h )
{
	m_RenderSettings.SetWidth(w);
	m_RenderSettings.SetHeight(h);
}

void DX9WindowTarget::AdjustPresentParams()
{
	ZeroMemory( &m_PresentParam, sizeof(m_PresentParam) );

	// keep the viewport's ratio equals to settings
	int bufferWidth = m_RenderSettings.GetBufferWidth();
	int bufferHeight = m_RenderSettings.GetBufferHeight();
	HWND hWnd = m_RenderSettings.GetWindowHandle();
	RECT rect;
	GetClientRect(hWnd, &rect);
	float wndRatio = float(rect.bottom) / float(rect.right);
	float viewRatio = m_RenderSettings.GetOrgRatio();
	//float viewRatio = m_RenderSettings.GetViewRatio();
	if ( isLockAspectRatio() && !gIsEqual(wndRatio, viewRatio) && wndRatio >0.0f )
	{
		if (wndRatio > viewRatio)
			bufferHeight = int(bufferWidth * wndRatio + 0.5f);
		else
			bufferWidth = int(bufferHeight / wndRatio + 0.5f);
	}
	DX9MainWindowTarget* pMainWin = (DX9MainWindowTarget*)CMainWindowTarget::GetInst();

	m_PresentParam.Windowed						= TRUE;
	m_PresentParam.hDeviceWindow				= hWnd;
	m_PresentParam.BackBufferWidth				= bufferWidth;
	m_PresentParam.BackBufferHeight				= bufferHeight;
	m_PresentParam.BackBufferFormat				= pMainWin->GetD3D_PARAM().BackBufferFormat;;
	m_PresentParam.AutoDepthStencilFormat		= pMainWin->GetD3D_PARAM().AutoDepthStencilFormat;
	m_PresentParam.BackBufferCount				= m_RenderSettings.GetBackBufferNum();
	m_PresentParam.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParam.SwapEffect					= (m_PresentParam.Windowed && m_PresentParam.BackBufferCount == 1)?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	m_PresentParam.EnableAutoDepthStencil		= TRUE;
	m_PresentParam.PresentationInterval			= m_RenderSettings.IsVSync()?D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// lock viewport's ratio
	if (isLockAspectRatio())
		LockAspectRatio(1.0f / viewRatio);
	else
		Update();
}

void DX9WindowTarget::ResetAntiAliasQuality()
{
	// check is valid quality
	EAntiAliasQuality quality = CMainWindowTarget::GetInst()->GetAntiAliasQuality();
	if(CHardwareCapacity::GetInst()->IsCanAntiAlias(type_cast<_texFORMAT>(m_PresentParam.BackBufferFormat)) && quality>AA_None )
	{
		m_PresentParam.SwapEffect	= 	D3DSWAPEFFECT_DISCARD;
		while( quality != AA_None)
		{
			m_PresentParam.MultiSampleType = (D3DMULTISAMPLE_TYPE)quality;
			Reset();
			if(ReCreate())
				return;
			quality = EAntiAliasQuality(((UINT)quality) - 2);
		}
	}
}

EAntiAliasQuality	DX9WindowTarget::GetAntiAliasQuality()
{
	return (EAntiAliasQuality)m_PresentParam.MultiSampleType;
}

bool DX9WindowTarget::Update()
{
	m_rcViewPort.left	= 0;
	m_rcViewPort.top	= 0;
	m_rcViewPort.right	= m_PresentParam.BackBufferWidth;
	m_rcViewPort.bottom	= m_PresentParam.BackBufferHeight;
	return CWindowTarget::Update();
}
