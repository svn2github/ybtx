#pragma once
#include "GraphicBase.h"
#include "TSingleton.h"

namespace sqr
{
	class CHardwareCapacity 
		: public Singleton<CHardwareCapacity>
		, public CGraphicMallocObject
	{
	public:
		/// 显卡厂商
		enum EVendorType
		{
			VT_NVIDIA,
			VT_ATI,
			VT_INTEL,
			VT_OTHER
		};

	public:
		CHardwareCapacity()
			: vendor(VT_OTHER)
			, VideoMem(0)
		{}
		/// 是否支持硬件T&L(intel那些太监卡不支持)
		virtual bool IsSupportHwTandL();

		/// 特殊顶点格式支持, 如D3DDTCAPS_UBYTE4之类
		virtual bool IsSupportVertexDeclType(DWORD type);
		/// 最大的顶点索引, 即16/32位的index buffer
		virtual uint GetMaxVertexIndex();

		/// 最大纹理宽
		virtual uint GetMaxTextureWidth();
		/// 最大纹理高
		virtual uint GetMaxTextureHeight();
		/// 是否可以自动生成mipmap(也就intel的笨得不会)
		virtual bool IsTextureCanAutoGenMipmap();
		/// 是否无条件支持非2^n大小的纹理, 如果不是, 需要满足这几个条件:
		/// 1. D3DTADDRESS_CLAMP寻址 2. D3DRS_WRAP禁用 3. 不使用Mipmap 4. 非DXTn格式
		virtual bool IsTextureSupportNonPowerOf2();

		/// 检测Vertex Shader版本
		virtual bool IsSupportVS(uint major, uint minor);
		/// 检测Pixel Shader版本
		virtual bool IsSupportPS(uint major, uint minor);
		/// 是否支持条件分支控制语句, 如if
		virtual uint GetVS2DynamicFlowControlDepth();
		virtual uint GetPS2DynamicFlowControlDepth();
		/// 是否支持静态流程控制语句, 如for
		virtual uint GetVS2StaticFlowControlDepth();
		virtual uint GetPS2StaticFlowControlDepth();

		/// 检测指定纹理格式是否支持
		virtual bool IsSupportTexture(texFORMAT format);
		/// 检测指定RT格式是否支持
		virtual bool IsSupportRenderTarget(texFORMAT format);

		virtual bool IsSupportDepthTexture(texFORMAT format);

		/// 检测是否支持硬件ShadowMap, 返回深度纹理格式. 不支持的返回D3DFMT_UNKNOWN
		virtual texFORMAT GetHwDepthTextureFormat() const;

		/// Multi-RenderTarget相关(先忽略了)
		//@{
		//uint GetNumSimultaneousRTs();
		//bool IsSupportMRTBitDepths();
		//bool IsSupportMRTPostPixelShaderBlending();
		//@}

		/// 是否支持反锯齿(又是intel的不支持)
		virtual bool IsCanAntiAlias(texFORMAT format);

		/// 取得显卡厂商
		EVendorType GetVendor() const;
		virtual void Initialize();
		UINT		GetVideoMem() const;
		UINT		GetAGPMem() const;

	protected:
		EVendorType vendor;
		UINT		VideoMem;
		UINT		AGPMem;
	};

	inline CHardwareCapacity::EVendorType CHardwareCapacity::GetVendor() const
	{
		return vendor;
	}

	inline UINT CHardwareCapacity::GetVideoMem() const
	{
		return VideoMem;
	}

	inline UINT CHardwareCapacity::GetAGPMem() const
	{
		return AGPMem;
	}
}