//------------------------------------------------------------------------------
//  CDistanceFont.cpp
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CDistanceFont.h"
#include "CGraphic.h"
#include "CPkgFile.h"
#include "CGpuProgramParameters.h"
#include "TSqrAllocator.inl"
#include "CFontMgr.h"
#include "CGpuProgramMgr.h"
#include "CHardwareCapacity.h"

namespace sqr
{

uint CDistanceFont::ShaderIdBase = 0x00000002 | MakeGroupID(4) | SSI_BASE_PS_SYSTEM_ID;

/// 字体文件头
struct KFontHeader
{
	uint fourcc;
	uint version;
	uint start_ptr;
	uint validate_chars;
	uint non_empty_chars;
	uint char_size;

	int16 base;
	int16 scale;
};

//------------------------------------------------------------------------------
CDistanceFont::CDistanceFont()
{
	ZeroMemory(this->distRange, sizeof(this->distRange));
	this->isScalable = true;
}

//------------------------------------------------------------------------------
bool
CDistanceFont::LoadFontFile()
{
	do
	{
		CPkgFile file;
		GString szLanFolder	  = CFontMgr::GetInst()->GetLangFolder();
		GString szDisFontName = CFontMgr::GetInst()->GetDisFontName();
		GString szDisFontPath = szLanFolder + "/fonts/" + szDisFontName;
		file.Open(PATH_ALIAS_LAN.c_str(), szDisFontPath.c_str());
		if (!file.IsValid())
			break;

		// font file header
		KFontHeader header;
		file.Read(&header, sizeof(header));
		if (MAKEFOURCC('K', 'F', 'N', 'T') != header.fourcc)
			break;
		if (1 != header.version)
			break;
		this->fontCharSize = header.char_size;
		m_font_size = (float)this->fontCharSize;
		m_fShowSize = m_font_size;
		this->distRange[0] = header.base / 32768.0f * 32 + 1;
		this->distRange[1] = (header.scale / 32768.0f + 1.0f) * 32;

		file.Seek(header.start_ptr, ios_base::beg);

		// font info index
		GVector<pair<int, int> > tempCharIndices(header.non_empty_chars);
		file.Read(&tempCharIndices.front(), sizeof(tempCharIndices.front()) * tempCharIndices.size());
		this->charIndexMap.insert(tempCharIndices.begin(), tempCharIndices.end());

		// font size
		// 资源中的Distance字体大小是uint16型，但现在GUi以改成浮点坐标系统,这里进行一下转换
		// 将来需要改掉
		GVector<pair<int, TVector2<uint16> > > tempCharSizesRes(header.validate_chars);
		GVector<pair<int, CharComposeSize> >   tempCharSizes(header.validate_chars);
		file.Read(&tempCharSizesRes.front(), sizeof(tempCharSizesRes.front()) * tempCharSizesRes.size());
		for ( size_t i=0; i<tempCharSizesRes.size(); ++i )
		{
			tempCharSizes[i].first	  = tempCharSizesRes[i].first;
			tempCharSizes[i].second.x = tempCharSizesRes[i].second.x;
			tempCharSizes[i].second.y = tempCharSizesRes[i].second.y;
		}

		this->composeSizeMap.insert(tempCharSizes.begin(), tempCharSizes.end());

		// font info
		this->charInfos.resize(header.non_empty_chars);
		file.Read(&this->charInfos.front(), sizeof(this->charInfos.front()) * this->charInfos.size());
		// distance
		this->distances.resize(header.non_empty_chars * header.char_size * header.char_size);
		file.Read(&this->distances.front(), sizeof(this->distances.front()) * this->distances.size());
		file.Close();

		return true;
	}
	while (0);

	return false;
}

//------------------------------------------------------------------------------
bool
CDistanceFont::UpdateShaderParams()
{
	SQR_TRY
	{
		CGpuProgramMgr* manager = CGpuProgramMgr::GetInst();
		CGpuProgram* program = NULL;
		for (index_t i = 0; i < FontEffectNum; i++)
		{
			this->shaderIds[i] = ShaderIdBase + i;

			program = manager->getGpuProgram(this->shaderIds[i]);
			if ( program )
			{
				if(!program->isLoaded())
				{
					program->bind();
					program->unbind();
				}
				program->getParamPointer()->setFloatArray("distanceRange", this->distRange, 2);
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("DistanceFont 更新shader失败", true);
		GfkLogExp(exp);
	}
	SQR_TRY_END

	return true;
}

//------------------------------------------------------------------------------
void
CDistanceFont::CacheCharacter( wchar_t ch, int x, int y )
{
	Ast(this->cacheTex);
	index_t charIndex = this->charIndexMap[ch];
	CIRect charRect(x, y, x + this->fontCharSize, y + this->fontCharSize);
	TEXLOCKED_RECT lockedRect;
	HRESULT hr = this->cacheTex->Lock(0, &lockedRect, &charRect);
	if (FAILED(hr))
	{
		GenErr(GraphicErr::GetErrTypeStr(Font_Err),"On hell! CDistanceFont lock texture failed!");
		return;
	}

	TBYTE* texData = (TBYTE*)(lockedRect.pBits);
	TBYTE* charData = &this->distances[uint(charIndex * this->GetSize() * this->GetSize())];
	for (index_t i = 0; i < this->fontCharSize; ++i)
	{
		memcpy(texData, charData, this->fontCharSize);
		texData += lockedRect.Pitch;
		charData += this->fontCharSize;
	}
	this->cacheTex->Unlock(0);
}

//------------------------------------------------------------------------------
DWORD
CDistanceFont::GetShaderID()
{
	this->UpdateShaderParams();

	index_t index = FontEffectNormal;
	if (this->IsBold())
	{
		index = FontEffectBold;
	}

	return this->shaderIds[index];
}

//------------------------------------------------------------------------------
bool
CDistanceFont::OnCreate()
{
	if (!this->LoadFontFile())
		return false;
	if (!this->UpdateShaderParams())
		return false;

	m_PixelSize = 1;

	bool InitText = false;
	if( CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 )
	{
		if( !InitText && CHardwareCapacity::GetInst()->IsSupportTexture(TFMT_A8))
		{
			m_PixelSize = 1;
			InitText = SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
				CFontBase::st_CacheTextureSize, CFontBase::st_CacheTextureSize, 1, TF_A8, &this->cacheTex));
		}

		if( !InitText && CHardwareCapacity::GetInst()->IsSupportTexture(TFMT_A8R8G8B8) )
		{
			m_PixelSize = 4;
			InitText = SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
				CFontBase::st_CacheTextureSize, 1, CFontBase::st_CacheTextureSize, TF_ARGB32, &this->cacheTex));
		}
	}
	else
	{
		m_PixelSize = 1;
		InitText = SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
			CFontBase::st_CacheTextureSize, CFontBase::st_CacheTextureSize, 1, TF_L8, &this->cacheTex));
	}		

	return CFontBase::OnCreate()&&InitText;
}

//------------------------------------------------------------------------------
void
CDistanceFont::OnResize()
{
	// restore the size
	m_fShowSize = m_font_size;
	m_font_size = (float)this->fontCharSize;
}

//------------------------------------------------------------------------------
inline float CDistanceFont::GetLineSpacing() const
{
	return m_fShowSize;
}

//------------------------------------------------------------------------------
void
CDistanceFont::OnDestroy()
{
	this->charInfos.clear();
	CFontBase::OnDestroy();
}

//------------------------------------------------------------------------------
const CFontBase::CharInfo*
CDistanceFont::GetCharInfo( wchar_t ch ,uint8 idx)
{
	IndexMap::iterator it = this->charIndexMap.find(ch);
	if (this->charIndexMap.end() == it)
	{
		return NULL;
	}
	else
	{
		if (this->composeSizeMap.find(ch) == this->composeSizeMap.end())
			this->composeSizeMap.insert(make_pair(ch, CharComposeSize(charInfos[it->second].width, charInfos[it->second].height)));

		return &this->charInfos[it->second];
	}
}

//-------------------------------------------------------------------------------
CVector2f 
CDistanceFont::CalculateSize( const wstring& text )
{
	// compute text rect area
	CVector2f temp_vec = CFontBase::CalculateSize(text);
	temp_vec.y = temp_vec.y / CDistanceFont::GetLineSpacing() * (float)fontCharSize;
	return temp_vec;
}
}// namespace sqr
