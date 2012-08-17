#pragma once
//------------------------------------------------------------------------------
/**
	@class CTextureFontMgr

	表情类.
	将表情做进字体系统，考虑给字体系统增加特殊字符集配置文件, 
	表情动画和普通字不在一张纹理，而是放到一个特殊字符集索引的Map中
*/
#include "TSingleton.h"
#include "CGraphicMallocObject.h"
#include "CTexture.h"
#include "CRectangle.h"
#include "CTextureFontDef.h"
#include "TSqrAllocator.inl"
#include <hash_map>

namespace sqr
{
	class CTextureFontMgr
		: public virtual CDynamicObject
		, public Singleton<CTextureFontMgr>
		, public CGraphicMallocObject
	{
	public:
		CTextureFontMgr();
		~CTextureFontMgr();
		
		static CTextureFontMgr*		Inst();
		static const GString		Emt_Root;				// 根
		static const GString		Emt_Face;				// 节点
		static const GString		Emt_Fc_Tex;				// 材质
		static const GString		Emt_Fc_Id;				// ID
		static const GString		Emt_Fc_Key;				// key
		static const GString		Emt_Fc_Pos;				// 位置
		static const GString		Emt_Fc_Tip;				// 描述
		static const GString		Emt_Fc_Idx;				// 索引
		typedef GVector<TextureFontDraw*>		 TextureFontDrawList;
		typedef GMap<uint32, TextureFontInfo*>	 TextureFontMap;	// 57345 emt 自行使用区域
		typedef GMap<GString, GWString>			 EmtIndexMap;	// #0001 '57345'
		typedef GMap<IdxKey, TextureFontInfoII*> TextureFontIIMap;	// 取代原有数据区域，使用自定义贴图
		TextureFontDrawList			m_EmtDList;
		

		bool Initialize(const string& Path, IGraphic* pIGraphic);
		bool InitializeEmt(const string& Path);
		bool InitializeTex(const string& Path);
		void Release(void);
		void EraserTextureFont(TextureFontMap::iterator it);

		void		  DrawEmtList();
		void		  DrawRect( const CFRect& rt, uint32 uColor, ITexture* pTexture, 
								const CFRect& rtText, ERenderStyle rs, float z );
		void		  FlushRect();

		wchar_t			 GetEmtByIdx(char* idx);
		const wchar_t*	 GetEmtPtrByIdx(char* idx);
		const char*		 GetEmtStrByIdx(char* idx);
		void			 SetImgScale(float sc);
		float			 GetImgScale();

		TextureFontInfo* GetTexFontInfo(uint32 id, uint8 idx = 0);

		inline TextureFontMap::iterator EmtBegin(void)		{ return m_EmtMap.begin();		};
		inline TextureFontMap::iterator EmtEnd(void)		{ return m_EmtMap.end();		};
		inline EmtIndexMap::iterator	EmtIdxBegin(void)	{ return m_EmtIdxMap.begin();	};
		inline EmtIndexMap::iterator	EmtIdxEnd(void)		{ return m_EmtIdxMap.end();		};
		inline TextureFontMap&			TextureFontmap(void){ return m_EmtMap;				};
		inline EmtIndexMap&				EmtIndexmap(void)	{ return m_EmtIdxMap;			};

		inline TextureFontIIMap::iterator TexBegin(void)	{ return m_TexMap.begin();		};
		inline TextureFontIIMap::iterator TexEnd(void)		{ return m_TexMap.end();		};
		inline TextureFontIIMap&		  TexIImap(void)	{ return m_TexMap;				};

		
	private:
		static CTextureFontMgr*& IntInst();
		CFRect StringToCFRect(std::string& str);
		
		TextureFontMap			m_EmtMap;			// 表情资源，预加载
		TextureFontIIMap		m_TexMap;			// 取代数据区资源， 预加载
		EmtIndexMap				m_EmtIdxMap;
		bool					m_isLoad;
		float					m_scale;

		//----------------渲染------------------
		IGraphic*				m_pGraphic;
		ITexture*				m_pTextue;
		ERenderStyle			m_RS;
		GVector<RectGroup>		m_TextureBuf;
		uint32					m_nCurTextureBufPos;
		uint32					m_nTotalRect;
		uint32					m_nFlush;
	};
}
