#pragma once
//------------------------------------------------------------------------------
/**
	@class CEmoticonMgr

	表情类.
	将表情做进字体系统，考虑给字体系统增加特殊字符集配置文件, 
	表情动画和普通字不在一张纹理，而是放到一个特殊字符集索引的Map中
*/
#include "TSingleton.h"
#include "CGraphicMallocObject.h"
#include "ITexture.h"
#include "CRectangle.h"
#include "CEmoticonDef.h"
#include "TNedAllocator.inl"
#include <hash_map>

namespace sqr
{
	class CEmoticonMgr
		: public virtual CBaseObject
		, public Singleton<CEmoticonMgr>
		, public CGraphicMallocObject
	{
	public:
		CEmoticonMgr();
		~CEmoticonMgr();
		
		static CEmoticonMgr*		Inst();
		static const GString		Emt_Root;				// 根
		static const GString		Emt_Face;				// 节点
		static const GString		Emt_Fc_Tex;				// 材质
		static const GString		Emt_Fc_Id;				// ID
		static const GString		Emt_Fc_Pos;				// 位置
		static const GString		Emt_Fc_Tip;				// 描述
		static const GString		Emt_Fc_Idx;				// 索引
		typedef GVector<EmoticonDraw*>		EmoticonDrawList;
		typedef GMap<uint32, EmoctionInfo*> EmoticonMap;	// 57345 emt
		typedef GMap<GString, GWString>		EmtIndexMap;	// #0001 '57345'
		EmoticonDrawList			m_EmtDList;
		

		bool Initialize(const string& Path, IGraphic* pIGraphic);
		void Release(void);
		void EraserEmoticon(EmoticonMap::iterator it);

		void		  DrawEmtList();
		void		  DrawRect( const CFRect& rt, uint32 uColor, ITexture* pTexture, 
								const CFRect& rtText, ERenderStyle rs, float z );
		void		  FlushRect();

		EmoctionInfo* GetEmtInfo(uint32 id);
		wchar_t		  GetEmtByIdx(char* idx);
		const wchar_t*GetEmtPtrByIdx(char* idx);
		const char*	  GetEmtStrByIdx(char* idx);
		void		  SetImgScale(float sc);
		float		  GetImgScale();

		inline EmoticonMap::iterator EmtBegin(void)		{ return m_EmtMap.begin();		};
		inline EmoticonMap::iterator EmtEnd(void)		{ return m_EmtMap.end();		};
		inline EmtIndexMap::iterator EmtIdxBegin(void)	{ return m_EmtIdxMap.begin();	};
		inline EmtIndexMap::iterator EmtIdxEnd(void)	{ return m_EmtIdxMap.end();		};
		inline EmoticonMap&			 Emoticonmap(void)	{ return m_EmtMap;				};
		inline EmtIndexMap&			 EmtIndexmap(void)	{ return m_EmtIdxMap;			};

		
	private:
		static CEmoticonMgr*& IntInst();
		CFRect StringToCFRect(std::string& str);
		
		EmoticonMap			m_EmtMap;			// 表情资源，预加载
		EmtIndexMap			m_EmtIdxMap;
		bool				m_isLoad;
		float				m_scale;

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
