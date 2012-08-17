#pragma once
#include "CGraphicMallocObject.h"
#include "IGraphic.h"
#include "CRectangle.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CDynamicObject.h"

namespace sqr
{
	struct TextureFontImg : public CGraphicMallocObject
	{
		TextureFontImg();
		ITexture*			pTexture;		// 贴图
		CFRect				texRt;			// 贴图使用区域
		uint32				dwColor;		// 贴图附加颜色
		uint32				dwLockFlg;		// 贴图锁定方式

		void SetColor(uint32 uc);
		void SetTexRect( CFRect& rt );
		CFRect GetTexRect();
	};

	class TextureFontInfo 
		: public virtual CDynamicObject
		, public CGraphicMallocObject
	{
	public:
		TextureFontInfo();
		~TextureFontInfo();

		uint32				EmtIdx;			// 贴图索引
		GString				EmtTexName;		// 名字
		TextureFontImg		EmtImg;			// 贴图图片信息
		GWString			EmtDes;			// 表情描述

		bool Init(IGraphic* pGraphic, uint32 idx, GString texname, CFRect pos, GWString des);
		void Destroy();
		void SetTextureFontIdx(uint32 idx);
		uint32 GetTextureFontIdx();
		const char* GetTexName();
	};

	typedef pair<uint32, uint32> IdxKey;

	class TextureFontInfoII : public virtual TextureFontInfo
	{
	public:
		TextureFontInfoII();
		~TextureFontInfoII();

		IdxKey TexIdx;

		bool Init(IGraphic* pGraphic, IdxKey idx, GString texname, CFRect pos, GWString des);
		void SetTextureFontIdxII(IdxKey idx);
		IdxKey GetTextureFontIdxII();
	};

	struct TextureFontDraw : public CGraphicMallocObject
	{
		TextureFontDraw();
		TextureFontImg*	pEmtImg;
		CFRect			posRect;
		CFRect			texRect;
	};

}
