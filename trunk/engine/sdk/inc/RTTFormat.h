#pragma once
#include "GraphicBase.h"
#include "CGraphicExp.h"

namespace sqr
{
// enum _texFORMAT
// {
// 	TFMT_X8R8G8B8,
// 	TFMT_A8R8G8B8,
// 	TFMT_R5G6B5,
// 	TFMT_G16R16F,
// 	TFMT_A16B16G16R16F,
// 	TFMT_R32F,
// 	TFMT_A32B32G32R32F,
// 	TFMT_G16R16,
// 	ERTTFomatNum
// };

namespace RTTFormat_Kit
{
/// ×ªÎª×Ö·û´®
static string ToString(_texFORMAT format);
/// ´Ó×Ö·û´®×ª»»
static _texFORMAT FromString(const string& format);
};

//------------------------------------------------------------------------------
inline string RTTFormat_Kit::ToString( _texFORMAT format )
{
	switch (format)
	{
		case TFMT_X8R8G8B8:
			return "X8R8G8B8";
		case TFMT_A8R8G8B8:
			return "A8R8G8B8";
		case TFMT_R5G6B5:
			return "R5G6B5";
		case TFMT_G16R16F:
			return "G16R16F";
		case TFMT_A16B16G16R16F:
			return "A16B16G16R16F";
		case TFMT_R32F:
			return "R32F";
		case TFMT_A32B32G32R32F:
			return "A32B32G32R32F";
		case TFMT_G16R16:
			return "G16R16";
		default:
			return "Unknown";
	}
}

//------------------------------------------------------------------------------
inline _texFORMAT RTTFormat_Kit::FromString( const string& format )
{
	if (format == "X8R8G8B8")
		return TFMT_X8R8G8B8;
	else if (format == "A8R8G8B8")
		return TFMT_A8R8G8B8;
	else if (format == "R5G6B5")
		return TFMT_R5G6B5;
	else if (format == "G16R16F")
		return TFMT_G16R16F;
	else if (format == "A16B16B16R16F")
		return TFMT_A16B16G16R16F;
	else if (format == "R32F")
		return TFMT_R32F;
	else if (format == "A32B32G32R32F")
		return TFMT_A32B32G32R32F;
	else if (format == "G16R16")
		return TFMT_G16R16;
	else
	{
		GfkLogErr(GraphicErr::GetErrTypeStr(RenderRuntime_Err),format + ": Invalid render target format");
		
		return TFMT_UNKNOWN;
	}
}


}// namespace sqr
