 #pragma once
#include "CFontBase.h"
/**
	@class CDistanceFont

	基于distance-field的字体.
	贴图中的字体点阵存储的这个像素到字体轮廓边的距离值, 以灰度表示.
	因为距离进行插值会比点阵字体精度高, 所以放大字体基本不会出现走样.
	对于这种字体无所谓字号, 画多大就是多大. 纹理区域大小由字体文件决定, 一般为32
	缺点是需要shader渲染, 字体需要预生成后读入内存.
	详见KlayGE文档

	@todo 描边和阴影效果需要优化(可能要传屏幕参数到shader来计算偏移)
 */

namespace sqr
{

class CDistanceFont : public CFontBase
{
public:
	CDistanceFont();
	virtual CVector2f CalculateSize(const wstring& text);
private:
	virtual bool OnCreate();
	virtual void OnResize();
	virtual void OnDestroy();
	virtual void CacheCharacter(wchar_t ch, int x, int y);

	virtual bool LoadFontFile();
	virtual bool UpdateShaderParams();
	
protected:
	virtual const CharInfo* GetCharInfo(wchar_t ch, uint8 idx = 0);
	virtual float GetLineSpacing() const;
	virtual DWORD GetShaderID();
	uint	fontCharSize;						///< 字体文件中的字体大小

private:
	typedef GHashMap<wchar_t, index_t> IndexMap;
	IndexMap charIndexMap;						///< key为字符, value为CharInfo的Index
	GVector<CharInfo> charInfos;				///< 字符实际大小(绘制矩形)
	
	float distRange[2];							///< distance范围
	GVector<uint8> distances;					///< 字符distance信息(可以看作是像素集)

	enum
	{
		FontEffectNormal,
		FontEffectBold,

		FontEffectNum
	};
	uint shaderIds[FontEffectNum];
	static uint ShaderIdBase;
	float m_fShowSize;
};

}// namespace sqr
