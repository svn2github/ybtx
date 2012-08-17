#pragma once
#include "GraphicBase.h"
/**
	@class RenderSettings

	设备创建所需参数.
 */

namespace sqr
{

class RenderSettings : public CGraphicMallocObject
{
public:
	RenderSettings();

	HWND GetWindowHandle() const;
	void SetWindowHandle(HWND hWnd);

	bool GetIsCtrlWindow();
	void SetIsCtrlWindow(bool b);

	bool IsWindowed() const;
	void SetWindowed(bool b);

	uint GetWidth() const;
	void SetWidth(uint w);

	uint GetHeight() const;
	void SetHeight(uint h);

	bool GetUseDef();
	void SetUseDef(bool b);

	float GetOrgRatio() const;
	void SetOrgRatio(float rt);

	uint GetBufferWidth() const;
	uint GetBufferHeight() const;
	
	uint GetWindowWidth() const;
	uint GetWindowHeight() const;

	float GetViewRatio() const;

	bool IsVSync() const;
	void SetVSync(bool b);

	EDisplayFormat GetFormat() const;
	void SetFormat(EDisplayFormat format);
	uint GetColorDepth() const;
	void SetColorDepth(uint depth);

	EAntiAliasQuality GetAntiAliasQuality() const;
	void SetAntiAliasQuality(EAntiAliasQuality quality);

	bool operator==(const RenderSettings& other);

	uint GetBackBufferNum(void) const;
	void SetBackBufferNum(uint Num);
public:
	static int GetScreenWidth();
	static int GetScreenHeight();

private:
	HWND wndHandle;			///< 窗口句柄
	bool isWindowed;		///< 全屏
	uint width;				///< 宽
	uint height;			///< 高
	float orgRatio;			///< 期望高度比例
	bool beDef;				///< 自定义大小
	bool isVSync;			///< 垂直同步
	EDisplayFormat format;	///< 色彩格式(色深)
	EAntiAliasQuality msaa;	///< 反锯齿质量
	uint backNum;			///< 缓冲Back数量

	bool isCtrlWindow;		///< 是否有当前窗口控制权
};
//------------------------------------------------------------------------------
inline int
RenderSettings::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

//------------------------------------------------------------------------------
inline int
RenderSettings::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

//------------------------------------------------------------------------------
inline
RenderSettings::RenderSettings()
		: isWindowed(true)
		, width(800)
		, height(500)
		, beDef(false)
		, isVSync(false)
		, format(DF_X8R8G8B8)
		, msaa(AA_None)
		, backNum(1)
		, isCtrlWindow(true)
{
}

//------------------------------------------------------------------------------
inline float
RenderSettings::GetViewRatio() const
{
	return float(this->height) / float(this->width);
}

//------------------------------------------------------------------------------
inline bool
RenderSettings::operator==( const RenderSettings& other )
{
	return this->isWindowed == other.isWindowed &&
	       this->width == other.width &&
	       this->height == other.height &&
	       this->isVSync == other.isVSync &&
	       this->format == other.format &&
	       this->msaa == other.msaa &&
		   this->backNum == other.backNum &&
		   this->wndHandle == other.wndHandle;
}

inline bool RenderSettings::GetIsCtrlWindow()
{
	return isCtrlWindow;
}

inline void RenderSettings::SetIsCtrlWindow(bool b)
{
	isCtrlWindow=b;
}
//------------------------------------------------------------------------------
inline HWND
RenderSettings::GetWindowHandle() const
{
	return this->wndHandle;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetWindowHandle( HWND hWnd )
{
	this->wndHandle = hWnd;
}

//------------------------------------------------------------------------------
inline bool
RenderSettings::IsWindowed() const
{
	return this->isWindowed;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetWindowed( bool b )
{
	this->isWindowed = b;
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetWidth() const
{
	return this->width;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetWidth( uint w )
{
	if (w <= 0)
		return;
	this->width = w;
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetHeight() const
{
	return this->height;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetHeight( uint h )
{
	if (h <= 0)
		return;
	this->height = h;
}

//------------------------------------------------------------------------------
inline bool 
RenderSettings::GetUseDef()
{
	return this->beDef;
}

//------------------------------------------------------------------------------
inline void 
RenderSettings::SetUseDef(bool b)
{
	this->beDef = b;
}

//------------------------------------------------------------------------------
inline float 
RenderSettings::GetOrgRatio() const
{
	return this->orgRatio;
}

//------------------------------------------------------------------------------
inline void 
RenderSettings::SetOrgRatio(float rt)
{
	if (rt < 0.0f)
		return;
	this->orgRatio = rt;
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetBufferWidth() const
{
	return uint(this->width);
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetBufferHeight() const
{
	return uint(this->height);
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetWindowWidth() const
{
	RECT rect;
	GetClientRect(this->wndHandle, &rect);
	return rect.right;
}

//------------------------------------------------------------------------------
inline uint
RenderSettings::GetWindowHeight() const
{
	RECT rect;
	GetClientRect(this->wndHandle, &rect);
	return rect.bottom;
}

//------------------------------------------------------------------------------
inline bool
RenderSettings::IsVSync() const
{
	return this->isVSync;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetVSync( bool b )
{
	this->isVSync = b;
}

//------------------------------------------------------------------------------
inline EDisplayFormat
RenderSettings::GetFormat() const
{
	return this->format;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetFormat( EDisplayFormat fmt )
{
	this->format = fmt;
}
//------------------------------------------------------------------------------
inline uint
RenderSettings::GetColorDepth() const
{
	switch (this->format)
	{
	case DF_X8R8G8B8:
	case DF_A8R8G8B8:
		return 32;
	default:
		return 16;
	}
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetColorDepth( uint depth )
{
	switch (depth)
	{
	case 16:
		this->format = DF_X1R5G5B5;
		break;
	case 32:
		this->format = DF_X8R8G8B8;
	default:
		Ast("color depth is not correct");
	}
}

//------------------------------------------------------------------------------
inline EAntiAliasQuality
RenderSettings::GetAntiAliasQuality() const
{
	return this->msaa;
}

//------------------------------------------------------------------------------
inline void
RenderSettings::SetAntiAliasQuality( EAntiAliasQuality quality )
{
	this->msaa = quality;
}

inline uint RenderSettings::GetBackBufferNum(void) const
{
	return this->backNum;
}

inline void RenderSettings::SetBackBufferNum(uint Num)
{
	backNum = Num;
}

}// namespace sqr
