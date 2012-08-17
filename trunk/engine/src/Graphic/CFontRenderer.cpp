//------------------------------------------------------------------------------
//  CFontRenderer.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CFontRenderer.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"

//------------------------------------------------------------------------------
namespace sqr
{
const size_t TextBufferSize = 1024;

//------------------------------------------------------------------------------
CFontRenderer::CFontRenderer()
: numChars(0)
, texture(NULL)
, shaderId(0)
, bufferSize(0)
, m_bAlignToPixel(true)
{
	this->GrowTextBuffer(TextBufferSize);
}

//------------------------------------------------------------------------------
CFontRenderer::~CFontRenderer()
{
	Ast(NULL == this->texture);
	Ast(0 == this->numChars);

	this->textIB.clear();
}

//------------------------------------------------------------------------------
void
CFontRenderer::Begin( ITexture* tex, uint id )
{
	if ( NULL != this->texture || 0 != this->numChars )
	{
		// 这种情况除了end抛异常，还有其他的原因吗？想不出来 - -!
		stringstream errmsg;
		errmsg << "Number Chars:" << this->numChars << "; Texture pointer:" << this->texture << "\n";
		
		CError error("Font Render");
		error.AppendMsg(errmsg.str());
		GfkLogExp(error);

		this->texture = NULL;
		this->numChars = 0;
	}

	this->texture = tex;
	this->shaderId = id;
}

//------------------------------------------------------------------------------
/**
	1----3
	| \  |
	|  \ |
	0----2
 */
void
CFontRenderer::AddChar( const CFRect& charRect,
					    const CFRect& uvRect,
						float depth,
						const CColor& top,
						const CColor& bottom,
						float italicOffset)
{
	Ast(NULL != this->texture);

	index_t offset = this->numChars * 4;

	// position
	this->textVB[offset+0].x = PixelAligned(charRect.left);
	this->textVB[offset+0].y = PixelAligned(charRect.bottom);
	this->textVB[offset+0].z = depth;
	this->textVB[offset+1].x = PixelAligned(charRect.left+italicOffset);
	this->textVB[offset+1].y = PixelAligned(charRect.top);
	this->textVB[offset+1].z = depth;
	this->textVB[offset+2].x = PixelAligned(charRect.right);
	this->textVB[offset+2].y = PixelAligned(charRect.bottom);
	this->textVB[offset+2].z = depth;
	this->textVB[offset+3].x = PixelAligned(charRect.right+italicOffset);
	this->textVB[offset+3].y = PixelAligned(charRect.top);
	this->textVB[offset+3].z = depth;
	// color
	this->textVB[offset+0].diffuse = bottom;	
	this->textVB[offset+1].diffuse = top;		
	this->textVB[offset+2].diffuse = bottom;	
	this->textVB[offset+3].diffuse = top;		
	// uv
	this->textVB[offset+0].tu = uvRect.left;
	this->textVB[offset+0].tv = uvRect.bottom;
	this->textVB[offset+1].tu = uvRect.left;
	this->textVB[offset+1].tv = uvRect.top;
	this->textVB[offset+2].tu = uvRect.right;
	this->textVB[offset+2].tv = uvRect.bottom;
	this->textVB[offset+3].tu = uvRect.right;
	this->textVB[offset+3].tv = uvRect.top;

	++this->numChars;
	if (this->numChars >= this->bufferSize)
	{
		this->GrowTextBuffer(this->bufferSize + TextBufferSize);
	}
}

void
CFontRenderer::AddVerticalChar( const CFRect& charRect,
					   const CFRect& uvRect,
					   float depth,
					   const CColor& top,
					   const CColor& bottom,
					   float italicOffset)
{
	Ast(NULL != this->texture);

	index_t offset = this->numChars * 4;

	// position
	this->textVB[offset+0].x = PixelAligned(charRect.left);
	this->textVB[offset+0].y = PixelAligned(charRect.bottom);
	this->textVB[offset+0].z = depth;
	this->textVB[offset+1].x = PixelAligned(charRect.left);
	this->textVB[offset+1].y = PixelAligned(charRect.top);
	this->textVB[offset+1].z = depth;
	this->textVB[offset+2].x = PixelAligned(charRect.right);
	this->textVB[offset+2].y = PixelAligned(charRect.bottom+italicOffset);
	this->textVB[offset+2].z = depth;
	this->textVB[offset+3].x = PixelAligned(charRect.right);
	this->textVB[offset+3].y = PixelAligned(charRect.top+italicOffset);
	this->textVB[offset+3].z = depth;
	// color
	this->textVB[offset+0].diffuse = bottom;	
	this->textVB[offset+1].diffuse = bottom;		
	this->textVB[offset+2].diffuse = top;	
	this->textVB[offset+3].diffuse = top;		
	// uv
	this->textVB[offset+1].tu = uvRect.left;
	this->textVB[offset+1].tv = uvRect.bottom;
	this->textVB[offset+3].tu = uvRect.left;
	this->textVB[offset+3].tv = uvRect.top;
	this->textVB[offset+0].tu = uvRect.right;
	this->textVB[offset+0].tv = uvRect.bottom;
	this->textVB[offset+2].tu = uvRect.right;
	this->textVB[offset+2].tv = uvRect.top;

	++this->numChars;
	if (this->numChars >= this->bufferSize)
	{
		this->GrowTextBuffer(this->bufferSize + TextBufferSize);
	}
}

//------------------------------------------------------------------------------
void
CFontRenderer::End()
{
	Ast(NULL != this->texture);
	
	ITexture*	t_texture = this->texture;
	DWORD		t_shaderId = this->shaderId;
	size_t		t_numChars = this->numChars;

	// clear
	this->texture = NULL;
	this->shaderId = 0;
	this->numChars = 0;
	// commit to graphic
	if (0 != t_numChars)
	{
		void* vb = NULL;
		void* ib = NULL;
		CGraphic* graphic = CGraphic::GetInst();
		RenderParam& renderParam = graphic->LockRenderStack(
			PT_TRIANGLELIST, t_numChars * 4, t_numChars * 2,
			Vertex2D::Format, sizeof(Vertex2D), &vb, &ib);
		memcpy(vb, &this->textVB.front(), sizeof(Vertex2D) * t_numChars * 4);
		memcpy(ib, &this->textIB.front(), sizeof(uint16) * t_numChars * 6);
		renderParam.m_RS = graphic->GetStaticRS(RS_FONT);
		renderParam.m_RS.m_Texture_S0 = t_texture;
		renderParam.m_RS.m_ShaderType = t_shaderId;
		graphic->UnlockRenderStack(renderParam);
	}
	// 画表情类字
	CTextureFontMgr::GetInst()->DrawEmtList();
}

//------------------------------------------------------------------------------
void 
CFontRenderer::FlushOneFrame()
{
	Ast(NULL != this->texture);

	ITexture*	t_texture = this->texture;
	DWORD		t_shaderId = this->shaderId;
	size_t		t_numChars = this->numChars;

	// clear
	this->numChars = 0;
	// commit to graphic
	if (0 != t_numChars)
	{
		void* vb = NULL;
		void* ib = NULL;
		CGraphic* graphic = CGraphic::GetInst();
		RenderParam& renderParam = graphic->LockRenderStack(
			PT_TRIANGLELIST, t_numChars * 4, t_numChars * 2,
			Vertex2D::Format, sizeof(Vertex2D), &vb, &ib);
		memcpy(vb, &this->textVB.front(), sizeof(Vertex2D) * t_numChars * 4);
		memcpy(ib, &this->textIB.front(), sizeof(uint16) * t_numChars * 6);
		renderParam.m_RS = graphic->GetStaticRS(RS_FONT);
		renderParam.m_RS.m_Texture_S0 = t_texture;
		renderParam.m_RS.m_ShaderType = t_shaderId;
		graphic->UnlockRenderStack(renderParam);
	}
	// 画表情类字
	CTextureFontMgr::GetInst()->DrawEmtList();

	// flush
	CGraphic::GetInst()->DrawStack();
}

//------------------------------------------------------------------------------
void
CFontRenderer::GrowTextBuffer( size_t size )
{
	Ast(this->bufferSize < size);

	this->textVB.resize(size * 4);
	this->textIB.resize(size * 6);
	// write indices
	for (uint16 i = this->bufferSize; i < size; ++i)
	{
		this->textIB[i * 6 + 0] = i * 4 + 0;
		this->textIB[i * 6 + 1] = i * 4 + 1;
		this->textIB[i * 6 + 2] = i * 4 + 2;
		this->textIB[i * 6 + 3] = i * 4 + 2;
		this->textIB[i * 6 + 4] = i * 4 + 1;
		this->textIB[i * 6 + 5] = i * 4 + 3;
	}
	this->bufferSize = size;
}

void CFontRenderer::SetAlignToPixel( bool bAlignToPixel )
{
	m_bAlignToPixel = bAlignToPixel;
}

}// namespace sqr
