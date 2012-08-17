#pragma once
//------------------------------------------------------------------------------
/**
	@class CFontBase

	字体基类.
	每个字符都是一个矩形, 贴上纹理就变成字了.
	所以矩形可以画到任意位置(甚至是3D世界中).
	由于没法一次性创建一张包含全部字符的纹理(汉字太TMD多了),
	所以采用LRU算法只把画过的字符保存在纹理缓存中, 满了的话覆盖最老没有引用过的.
	为了节省纹理显存, 采用灰度图, 字的颜色由顶点色来控制, 这样可以做出彩虹字(每个顶点一种颜色)等效果.
 */
#include "CRefObject.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CColor.h"
#include "CTexture.h"
#include "RenderTextParam.h"
#include "CTextureFontMgr.h"
#include "CFontListContainer.h"
#include <hash_map>

namespace sqr
{
	class CFontRenderer;

	class CFontBase 
		: public CRefObject
		, public CGraphicMallocObject
	{
	public:
		CFontBase();
		virtual ~CFontBase();

		bool			Create( int32 id, bool bold, float fSize);
		void			Resize(float size);

		int32			GetID() const;
		float			GetSize() const;
		bool			IsBold() const;

		virtual CVector2f CalculateSize(const wstring& text);
		size_t			GetCharAtPixel(const wstring &text, size_t start_char, float pixel) const;

		void			Begin();
		void			DrawText(const RenderTextParam& param);
		void			End();

		void			FlushOneFrame();
		void			SetFrameFirst();

		void			UpdateCacheTexture( const wchar_t& ch, const float size = -1.0f, uint8 idx = 0 );
		void			UpdateCache(const wstring& text, const float size = -1.0f, uint8 idx = 0);
		bool			IsScalable() const;
		float			GetScale(float size);
		virtual	void	InitTextKey(RenderTextParam& param, bool bInKey){};

		void			SetAlignToPixel(bool bAlignToPixel);

		void			UpdateCharInfo(const wstring& text);

		virtual float	GetLineSpacing()	const;
		virtual float	GetFontHeight()		const;
		virtual float	GetFontGlyphHeight(wchar_t c)	const;

		// 字符在一个单元格中所占的区域.
		struct CharInfo
		{
			int16 top;
			int16 left;
			int16 width;
			int16 height;
		};

		static const int32 INVALID_FONT_ID	= -1;
		static const int32 DISTANCE_FONT_ID = 99999;
		// 自行使用区域
		static const int32 EMT_FONT_ID_BEGIN = 0xE000;
		static const int32 EMT_FONT_ID_END = 0xF8FF;
		static const int32 st_CacheTextureSize;
	protected:
		virtual bool			OnCreate();
		virtual void			OnResize();
		virtual void			OnDestroy();
		virtual void			PreProcessStr(){};
		virtual const CharInfo* GetCharInfo(wchar_t ch, uint8 idx = 0)	 = 0;
		virtual void			CacheCharacter(wchar_t ch, int x, int y) = 0;
		virtual DWORD			GetShaderID()							 = 0;

		virtual void AddText(const RenderTextParam& param);
		virtual bool CheckAndAddTex(wchar_t ch, uint8 idx, float& x, float& y, float& scale, 
									const CFRect& clipRect, float& charWidth, const RenderTextParam& param );
		void CFontBase::FormatVerticalTex(const wstring& text, const CFRect& clipRect, const CVector3f& pos , float& scale, float& height, float& x, float& y, const RenderTextParam& param );

	protected:
		typedef TVector2<float>						CharComposeSize;
		typedef GSet<wchar_t>						WCharSet;
		typedef GHashMap<wchar_t, CFRect>			RectMap;
		typedef GHashMap<wchar_t, CharComposeSize>	ComposeSizeMap;
		typedef GMap<wchar_t, CFontListNode*>		LRUFindMap;

		int32			 id;
		float			 m_font_size;
		bool			 isBold;
		bool			 isScalable;
		bool			 isBegin;
		RectMap			 charRectMap;		// 纹理缓冲中的字符所占区域
		CFontList		 charLRU;			// 缓冲队列, Least Recently Used
		LRUFindMap		 m_lruFindMap;		// 用于加速charLRU的查找
		uint			 curX, curY;		// 纹理缓冲空闲处的标志
		ComposeSizeMap	 composeSizeMap;	// 字符排版大小, key为wchar_t
		ITexture		*cacheTex;			// 用作文字缓冲的纹理
		WCharSet		 m_disChar;			// 当前字体中没有的wchar
		const wchar_t	*m_curChar;			// 但前str的指针
		wstring			 m_oldChar;			// 处理后缓存的wstr
		CFontRenderer	*fontRenderer;
		bool			 m_bAlignToPixel;
		int              m_PixelSize;
		float			 m_formatHeight;	// 教正高度
		bool			 m_beFrameFirst;	// 当前帧第一个字
		CFontListNode*	 m_FirstCharItr;
	};

	//------------------------------------------------------------------------------
	inline int32 CFontBase::GetID() const
	{
		return this->id;
	}

	//------------------------------------------------------------------------------
	inline void CFontBase::SetFrameFirst()
	{
		m_beFrameFirst = true;
	}

	//------------------------------------------------------------------------------
	inline float
	CFontBase::GetSize() const
	{
		return m_font_size;
	}

	//------------------------------------------------------------------------------
	inline bool
	CFontBase::IsBold() const
	{
		return this->isBold;
	}

	//------------------------------------------------------------------------------
	inline bool
	CFontBase::IsScalable() const
	{
		return this->isScalable;
	}

	//------------------------------------------------------------------------------
	inline float CFontBase::GetLineSpacing() const
	{
		return 0;
	}

	//------------------------------------------------------------------------------
	inline float CFontBase::GetFontGlyphHeight(wchar_t c)	const
	{
		return 0;
	}

	//------------------------------------------------------------------------------
	inline float CFontBase::GetFontHeight() const
	{
		return 0;
	}

	inline void CFontBase::SetAlignToPixel(bool bAlignToPixel)
	{
		m_bAlignToPixel = bAlignToPixel;
	}
}
