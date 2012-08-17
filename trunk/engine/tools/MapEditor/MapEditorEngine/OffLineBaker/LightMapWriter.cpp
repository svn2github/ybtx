//------------------------------------------------------------------------------
//  LightMapWriter.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LightMapWriter.h"
#include "StringHelper.h"
#include "CWindowTarget.h"
#include "IoSystem.h"
#include "Algorithm.h"
#include "BaseHelper.h"
#include "CTexture.h"
#include "CGraphic.h"
#include "CColor.inl"

namespace sqr
{
#pragma region CLightMapWriter
	//------------------------------------------------------------------------------
	CLightMapWriter::CLightMapWriter()
	: width(0)
	, depth(0)
	{
	}

	//------------------------------------------------------------------------------
	CLightMapWriter::~CLightMapWriter()
	{
		SafeRelease(m_srcTexture);
		//SafeRelease(m_srcSurface);
		//m_srcSurface = NULL;
		//this->srcTexture = NULL;
	}

	//------------------------------------------------------------------------------
	void
	CLightMapWriter::Write( index_t x, index_t z, const ITexture* surface )
	{
		//Ast(surface.isvalid());

		/*Ast(D3DXLoadSurfaceFromSurface(m_srcTexture, NULL, NULL,
									 surface, NULL, NULL, D3DX_FILTER_NONE, 0));*/
		m_srcTexture->FillFromTexture(surface,NULL,NULL);
	}
#pragma endregion CLightMapWriter

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
#pragma region CLMFileWriter
	//------------------------------------------------------------------------------
	void
	CLMFileWriter::Write( index_t x, index_t z, const ITexture* surface )
	{
		CLightMapWriter::Write(x, z, surface);

		string fileName = format("lit:%d_%d.dds", x, z);
		fileName = IoSystem::GetInst()->GetFullPath(fileName);
		m_srcTexture->SaveToFile(fileName.c_str());
		//V(D3DXSaveSurfaceToFile(fileName.c_str(), D3DXIFF_DDS, m_srcSurface,
		//						NULL, NULL));
	}

	//------------------------------------------------------------------------------
	CLMFileWriter::CLMFileWriter()
	{
		// 创建用于取得表面的纹理
		 CGraphic::GetInst()->CreateEmptyTexture(512,512,1, TF_DXT1,&m_srcTexture);
		//V( GetD3DDevice()->CreateTexture(512, 512, 
		//		1, D3DUSAGE_DYNAMIC, D3DFMT_DXT1, D3DPOOL_SYSTEMMEM, 
		//		srcTexture.address(), NULL) );
		//Ast( m_srcTexture->GetSurfaceLevel(0, m_srcSurface.address()) );
	}
#pragma endregion CLMFileWriter
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
#pragma region CLogicalShadowWriter
	//------------------------------------------------------------------------------
	CLogicalShadowWriter::CLogicalShadowWriter()
			: blockSize(16 * 4)
	{
		// create offscreen surface
		//Ast(GetD3DDevice()->CreateOffscreenPlainSurface(512, 512,
		//	D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, m_srcSurface.address(),
		//	NULL));

		CGraphic::GetInst()->CreateEmptyTexture( 512, 512, 1, TF_ARGB32, &m_srcTexture );
	}

	//------------------------------------------------------------------------------
	void
	CLogicalShadowWriter::Write( index_t x, index_t z, const ITexture* surface )
	{
		CLightMapWriter::Write(x, z, surface);
		if (this->buffer.empty())
		{
			size_t size = this->width * this->depth * this->blockSize * this->blockSize;
			this->buffer.resize(size);
		}

		// write to buffer
		_TEXLOCKED_RECT srcRect;
		m_srcTexture->Lock(0,&srcRect, NULL,0, LOCK_READONLY);
		int offset = (z * this->width * this->blockSize + x) * this->blockSize;
		for (int j = this->blockSize - 1; j >= 0; --j)
		{
			for (int i = 0; i < int(this->blockSize); ++i)
			{
				byte color = this->GetBlockColor(i, j, srcRect.Pitch, (CColor*)srcRect.pBits);
				//Ast(color > 100);
				this->buffer[offset + i] = color;
			}
			// next row
			offset += this->width * this->blockSize;
		}
		m_srcTexture->Unlock(0);
	}

	//------------------------------------------------------------------------------
	byte
	CLogicalShadowWriter::GetBlockColor( index_t x, index_t y, int pitch, const CColor* data )
	{
		pitch /= sizeof(CColor);
		uint size = pitch / this->blockSize;
		RECT rect;
		rect.left = x * size;
		rect.top = y * size;
		rect.right = rect.left + size;
		rect.bottom = rect.top + size;

		float gray = 0;
		for (int j = rect.top; j < rect.bottom; ++j)
		{
			for (int i = rect.left; i < rect.right; ++i)
			{
				index_t index = j * pitch + i;
				gray += data[index].ToGray();
			}
		}
		gray /= size * size;
		return static_cast<byte>(clamp(gray, 0.0f, 255.0f));
	}

	//------------------------------------------------------------------------------
	void
	CLogicalShadowWriter::Save(int offset)
	{
		Ast(!this->buffer.empty());

		string fileName = format("lit:shadow.sh%d", offset);
		fileName = IoSystem::GetInst()->GetFullPath(fileName);
		locale::global(locale(""));
		ofstream filestream(fileName.c_str());
		if (filestream)
		{
			filestream.write((const TCHAR*)(&this->buffer.front()), this->buffer.size());	
		}
		else
		{
			string errmsg = fileName + "写入失败";
			MessageBox(NULL,errmsg.c_str(),"提示",MB_OK);
		}

		this->buffer.clear();
	}
#pragma endregion CLogicalShadowWriter
}
