#include "stdafx.h"
#include "CHardwareCapacity.h"
/*
默认是所有功能都磨认为最弱功能
*/

bool CHardwareCapacity::IsSupportHwTandL()
{
	return false;
}

bool CHardwareCapacity::IsSupportVertexDeclType(DWORD type)
{
	return false;
}

uint CHardwareCapacity::GetMaxVertexIndex()
{
	return 0xFFFF;
}

uint CHardwareCapacity::GetMaxTextureWidth()
{
	return 512;
}

uint CHardwareCapacity::GetMaxTextureHeight()
{
	return 512;
}

bool CHardwareCapacity::IsTextureCanAutoGenMipmap()
{
	return false;
}

bool CHardwareCapacity::IsTextureSupportNonPowerOf2()
{
	return false;
}

bool CHardwareCapacity::IsSupportVS(uint major, uint minor)
{
	return false;
}

bool CHardwareCapacity::IsSupportPS(uint major, uint minor)
{
	return false;
}

uint CHardwareCapacity::GetVS2DynamicFlowControlDepth()
{
	return 0;
}

uint CHardwareCapacity::GetPS2DynamicFlowControlDepth()
{
	return 0;
}

uint CHardwareCapacity::GetVS2StaticFlowControlDepth()
{
	return 0;
}

uint CHardwareCapacity::GetPS2StaticFlowControlDepth()
{
	return 0;
}

bool CHardwareCapacity::IsSupportTexture(texFORMAT format)
{
	return false;
}

bool CHardwareCapacity::IsSupportRenderTarget(texFORMAT format)
{
	return false;
}

bool CHardwareCapacity::IsSupportDepthTexture(texFORMAT format)
{
	return false;
}

texFORMAT CHardwareCapacity::GetHwDepthTextureFormat() const
{
	return TFMT_UNKNOWN;
}

bool CHardwareCapacity::IsCanAntiAlias(texFORMAT format)
{
	return false;
}

void CHardwareCapacity::Initialize()
{}