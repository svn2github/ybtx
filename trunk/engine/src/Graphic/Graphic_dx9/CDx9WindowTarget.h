#pragma once
#include "CWindowTarget.h"
#include "RenderSettings.h"
#include "CDx9DepthTexture.h"

namespace sqr
{
class DX9WindowTarget : public CWindowTarget
{
public:
	DX9WindowTarget(const RenderSettings& settings);
	~DX9WindowTarget();
	bool					ReCreate(void);
	void					Reset(void);

	wstring					DoScreenShot(const wchar_t* szAliasPath, const wchar_t * szFilename);

	HRESULT					ChangeRenderSettings(const RenderSettings& settings);
	void					RestoreSize( bool useClientRect = true );
	virtual void			ReSaveSize( uint w, uint h );
	void					AdjustPresentParams();
	void					ResetAntiAliasQuality();
	EAntiAliasQuality		GetAntiAliasQuality();
	void					Render(void);
protected:
	bool					Update();
	void					BeginRender(void);
	void					EndRender(void);
protected:
	IDirect3DDevice9*		m_pD3DDevice;
	DX9DepthTexture			m_DepthBuffer;
	D3DPRS_PARAM			m_PresentParam;			//画面显示方式
	LPDIRECT3DSWAPCHAIN9	m_pSwapChain;
	ID3DSurfacePtr			m_pSwapBack;
	ID3DSurfacePtr			m_oldBackBuffer;
	D3DVIEWPORT9			m_oldViewport;
	float					m_oldAspectRatio;
	bool					m_bResetGuard;
	bool					m_bForceReset;
	RenderSettings			m_RenderSettings;
};
}