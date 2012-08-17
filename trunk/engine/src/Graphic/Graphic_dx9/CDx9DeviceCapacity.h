#pragma once
//------------------------------------------------------------------------------
/**
	@class CDx9DeviceCapacity

	用于检测显卡的兼容性.
	因游戏面向于ShaderModel2.0以上的显卡, 所以一些老显卡不支持的特性就不检测了.
	还有一些不常用或不标准的特性(如N卡和A卡只有一家支持), 也不检测
	大部分兼容性问题会出在intel的集成显卡上, 因笔记本的用户也不小, 不得不检测...

	(C) 2009 ThreeOGCMan
*/
#include "CHardwareCapacity.h"
#include "Dx9Base.h"

namespace sqr
{

class Dx9Adapter;
class CDx9DeviceCapacity : public CHardwareCapacity
{

public:
	/// 是否支持硬件T&L(intel那些太监卡不支持)
	bool IsSupportHwTandL();

	/// 特殊顶点格式支持, 如D3DDTCAPS_UBYTE4之类
	bool IsSupportVertexDeclType(DWORD type);
	/// 最大的顶点索引, 即16/32位的index buffer
	uint GetMaxVertexIndex();

	/// 最大纹理宽
	uint GetMaxTextureWidth();
	/// 最大纹理高
	uint GetMaxTextureHeight();
	/// 是否可以自动生成mipmap(也就intel的笨得不会)
	bool IsTextureCanAutoGenMipmap();
	/// 是否无条件支持非2^n大小的纹理, 如果不是, 需要满足这几个条件:
	/// 1. D3DTADDRESS_CLAMP寻址 2. D3DRS_WRAP禁用 3. 不使用Mipmap 4. 非DXTn格式
	bool IsTextureSupportNonPowerOf2();

	/// 检测Vertex Shader版本
	bool IsSupportVS(uint major, uint minor);
	/// 检测Pixel Shader版本
	bool IsSupportPS(uint major, uint minor);
	/// 是否支持条件分支控制语句, 如if
	uint GetVS2DynamicFlowControlDepth();
	uint GetPS2DynamicFlowControlDepth();
	/// 是否支持静态流程控制语句, 如for
	uint GetVS2StaticFlowControlDepth();
	uint GetPS2StaticFlowControlDepth();

	/// 检测指定纹理格式是否支持
	bool IsSupportTexture(texFORMAT format);
	/// 检测指定RT格式是否支持
	bool IsSupportRenderTarget(texFORMAT format);

	bool IsSupportDepthTexture(texFORMAT format);

	/// 检测是否支持硬件ShadowMap, 返回深度纹理格式. 不支持的返回D3DFMT_UNKNOWN
	texFORMAT GetHwDepthTextureFormat() const;

	/// Multi-RenderTarget相关(先忽略了)
	//@{
	//uint GetNumSimultaneousRTs();
	//bool IsSupportMRTBitDepths();
	//bool IsSupportMRTPostPixelShaderBlending();
	//@}

	/// 是否支持反锯齿(又是intel的不支持)
	bool IsCanAntiAlias(texFORMAT format);

	void Initialize();
private:

	friend class DX9Graphic;
	friend class DX9MainWindowTarget;

	IDirect3D9* d3d9;
	D3DCAPS9	d3dcaps;
	Dx9Adapter* adapter;
};

//------------------------------------------------------------------------------

}// namespace sqr

