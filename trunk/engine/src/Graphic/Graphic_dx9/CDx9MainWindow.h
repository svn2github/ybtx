#pragma once

#include "CRenderTarget.h"
#include "CViewport.h"
#include "RenderSettings.h"
#include "CRenderGroup.h"
#include "Dx9Base.h"

namespace sqr
{
class DX9MainWindowTarget : public CMainWindowTarget
{
public:
	DX9MainWindowTarget();
	~DX9MainWindowTarget();

	void					Reset(void);
	bool					ReCreate(void);
public:
	void					RestoreSize( bool useClientRect = true );
	wstring					DoScreenShot(const wchar_t* szAliasPath, const wchar_t * szFilename);
	virtual void			ReSaveSize( uint w, uint h );

	HRESULT					IsRenderActive();

	LPDIRECT3D9				GetD3d9();
	LPDIRECT3DDEVICE9		GetDevice();

	HRESULT					ChangeRenderSettings(const RenderSettings& settings);
	HRESULT					CreateSuitableDevice();

	bool					Initialize(const RenderSettings& settings);
	void					AdjustPresentParams();
	void					ResetAntiAliasQuality();
	EAntiAliasQuality		GetAntiAliasQuality();

	/// 返回所有显卡支持的分辨率宽度
	vector<uint> GetResolutionWidths();
	const D3DPRS_PARAM&		GetD3D_PARAM(void);
protected:

	HRESULT					CreateUtilRes();
	HRESULT					ReleaseUtilRes();
	bool					Update();
protected:
	//D3D设备组件
	static LPDIRECT3D9		 m_lpD3d;				//D3D接口
	static LPDIRECT3DDEVICE9 m_lpD3DDevice;			//D3D设备
	D3DPRS_PARAM			m_PresentParam;			//画面显示方式
	bool					m_bResetGuard;
	bool					m_bForceReset;
};

inline const D3DPRS_PARAM& 
DX9MainWindowTarget::GetD3D_PARAM(void)
{
	return m_PresentParam;
}

//------------------------------------------------------------------------------
/// 每次都强制转换一下, 真不爽
inline IDirect3DDevice9* GetD3DDevice()
{
	return ((DX9MainWindowTarget*)CMainWindowTarget::GetInst())->GetDevice();;
}

}// namespace sqr
