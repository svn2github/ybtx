#pragma once
#include "GraphicRenderStyle.h"
#include "CGraphicMallocObject.h"
/// 显示模式.
class DisplayMode : public CGraphicMallocObject
{
public:
	/// 构造函数
	DisplayMode();
	/// 常用构造函数
	DisplayMode(uint w, uint h, uint format);
	/// 取得水平分辨率
	uint GetWidth() const;
	/// 设置水平分辨率
	void SetWidth(uint w);
	/// 取得垂直分辨率
	uint GetHeight() const;
	/// 设置垂直分辨率
	void SetHeight(uint h);
	/// 取得像素格式
	uint GetPixelFormat() const;
	/// 设置像素格式
	void SetPixelFormat(uint format);
	/// 取得颜色深度
	uint GetColorDepth() const;
	/// 取得宽高比
	float GetAspectRatio() const;

	/// 等于 operator
	bool operator==(const DisplayMode& rhs) const;
	/// 不等于 operator
	bool operator!=(const DisplayMode& rhs) const;

private:
	uint width;			///< 水平分辨率
	uint height;		///< 垂直分辨率
	uint pixelFormat;	///< 像素格式
};

typedef GVector<DisplayMode> DisplayModeArray;


//------------------------------------------------------------------------------
inline
DisplayMode::DisplayMode()
		: width(0)
		, height(0)
		, pixelFormat(TFMT_UNKNOWN)
{
	// empty
}

//------------------------------------------------------------------------------
inline
DisplayMode::DisplayMode( uint w, uint h, uint format )
		: width(w)
		, height(h)
		, pixelFormat(format)
{
	// empty
}

//------------------------------------------------------------------------------
inline uint
DisplayMode::GetWidth() const
{
	return this->width;
}

//------------------------------------------------------------------------------
inline void
DisplayMode::SetWidth(uint w)
{
	this->width = w;
}

//------------------------------------------------------------------------------
inline uint
DisplayMode::GetHeight() const
{
	return this->height;
}

//------------------------------------------------------------------------------
inline void
DisplayMode::SetHeight(uint h)
{
	this->height = h;
}

//------------------------------------------------------------------------------
inline uint
DisplayMode::GetPixelFormat() const
{
	return this->pixelFormat;
}

//------------------------------------------------------------------------------
inline void
DisplayMode::SetPixelFormat(uint format)
{
	this->pixelFormat = format;
}

//------------------------------------------------------------------------------
inline uint
DisplayMode::GetColorDepth() const
{
	switch (this->pixelFormat)
	{
		case TFMT_X8R8G8B8:
		case TFMT_A8R8G8B8:
		case TFMT_R8G8B8:
			return 32;
		default:
			return 16;
	}
}

//------------------------------------------------------------------------------
inline float
DisplayMode::GetAspectRatio() const
{
	return (float(this->width) / float(this->height));
}

//------------------------------------------------------------------------------
inline bool
DisplayMode::operator==(const DisplayMode& rhs) const
{
	return ((this->width == rhs.width) &&
	        (this->height == rhs.height) &&
	        (this->pixelFormat == rhs.pixelFormat));
}

//------------------------------------------------------------------------------
inline bool
DisplayMode::operator!=(const DisplayMode& rhs) const
{
	return !(*this == rhs);
}
