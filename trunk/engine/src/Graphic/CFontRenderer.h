#pragma once
//------------------------------------------------------------------------------
/**
    @class sqr::CFontRenderer

	字体绘制器.
	GUI那边DrawText太频繁了, 为了减少DP数, 增加一对Begin/End对字体绘制进行打包.
	淘汰原来static数组的缓存方式, 采用动态大小的数组, 以免字数超出限制.

	@todo 所有字体共享同一张纹理

    (C) 2009 ThreeOGCMan
*/
#include "TSingleton.h"
#include "GraphicBase.h"
#include "CGraphic.h"
#include "CTextureFontMgr.h"

//------------------------------------------------------------------------------
namespace sqr
{
class ITexture;

class CFontRenderer 
	: public TSingleton<CFontRenderer>
	, public CGraphicMallocObject
{
public:
	/// 开始添加
	void Begin(ITexture* texture, uint shader);
	/// 添加字符
	void AddChar(const CFRect& charRect,
				 const CFRect& uvRect,
				 float depth,
				 const CColor& top,
				 const CColor& bottom,
				 float italicOffset);
	void AddVerticalChar(const CFRect& charRect,
				const CFRect& uvRect,
				float depth,
				const CColor& top,
				const CColor& bottom,
				float italicOffset);
	/// 结束添加, 提交到Graphic
	void End();

	void FlushOneFrame();

	void SetAlignToPixel(bool bAlignToPixel);
	
	inline float PixelAligned(float x);

private:
	friend class TSingleton<CFontRenderer>;
	CFontRenderer();
	~CFontRenderer();

	void GrowTextBuffer(size_t size);
private:
	size_t numChars;
	ITexture* texture;
	DWORD shaderId;

	size_t bufferSize;
	GVector<Vertex2D> textVB;
	GVector<uint16> textIB;
	bool	m_bAlignToPixel;
};

inline float CFontRenderer::PixelAligned(float x)
{
	if ( m_bAlignToPixel )
		return (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f));
	else
		return x;
}

}// namespace sqr
