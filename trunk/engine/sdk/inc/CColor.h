#pragma once
#include "CQuickRand.h"
#include "CMath.h"
#include "CMathMallocObject.h"

namespace sqr
{
	/// @brief 颜色结构 ARGB 格式
	struct CColor : public CMathMallocObject
	{
		union
		{
			uint32	dwColor;	///< ARGB 格式颜色
			uint8	byColor[4];	///< 从前到后按照BGRA取得各颜色分量
			struct
			{
				uint8 B;
				uint8 G;
				uint8 R;
				uint8 A;
			};
		};

		CColor();
		CColor( uint32 c );
		CColor( uint8 a, uint8 r, uint8 g, uint8 b );

		operator uint32() const;

		const CColor& operator= ( const CColor& arg );

		const CColor operator+ ( const CColor& arg ) const;
		const CColor operator- ( const CColor& arg ) const;
		const CColor operator* ( float arg ) const;
		const CColor operator/ (float arg) const;
		const CColor operator* (const CColor& rhs) const;

		CColor& operator+=(const CColor& rhs);
		CColor& operator-=(const CColor& rhs);
		CColor& operator*=(const CColor& rhs);
		CColor& operator*=(float ratio);
		CColor& operator/=(float ratio);


		CColor Modulate(const CColor& rhs);

		/// @brief 将颜色转换为ABGR格式
		uint32 ToABGR()const;
		/// @brief 将颜色转换为ARGB格式
		uint32 ToRGBA()const;
		/// @brief 将颜色转换为ARGB格式
		uint32 ToARGB()const;

		/// @brief 求颜色的反色
		CColor Reverse()const;
		/// 转为灰度值
		uint8 ToGray() const;

	public:
		/// @brief 产生随机颜色
		static CColor Rand();
		/// @brief 产生随机的深色
		static CColor RandDark();
		/// 返回最大值(每个分量的最大值)
		static const CColor Max(const CColor& color1, const CColor& color2);
		/// 返回最小值(每个分量的最小值)
		static const CColor Min(const CColor& color1, const CColor& color2);
	public:
		/// @brief 常用的颜色值，具体色样参考 http://www.ttpw.com/rgb.htm
#ifdef _WIN32
#pragma region ConstantColors
#endif
		static const uint32 LightPink			= 0xFFFFB6C1;	///< 浅粉红
		static const uint32 Pink				= 0xFFFFC0CB;	///< 粉红
		static const uint32 Crimson				= 0xFFDC143C;	///< 深红/猩红
		static const uint32 LavenderBlush		= 0xFFFFF0F5;	///< 淡紫红
		static const uint32 PaleVioletRed		= 0xFFDB7093;	///< 弱紫罗兰红
		static const uint32 HotPink				= 0xFFFF69B4;	///< 热情的粉红
		static const uint32 DeepPink			= 0xFFFF1493;	///< 深粉红
		static const uint32 MediumVioletRed 	= 0xFFC71585;	///< 中紫罗兰红
		static const uint32 Orchid				= 0xFFDA70D6;	///< 暗紫色/兰花紫
		static const uint32 Thistle				= 0xFFD8BFD8;	///< 蓟色
		static const uint32 Plum				= 0xFFDDA0DD;	///< 洋李色/李子紫
		static const uint32 Violet				= 0xFFEE82EE;	///< 紫罗兰
		static const uint32 Magenta				= 0xFFFF00FF;	///< 洋红/玫瑰红
		static const uint32 Fuchsia				= 0xFFFF00FF;	///< 紫红/灯笼海棠
		static const uint32 DarkMagenta			= 0xFF8B008B;	///< 深洋红
		static const uint32 Purple				= 0xFF800080;	///< 紫色
		static const uint32 MediumOrchid		= 0xFFBA55D3;	///< 中兰花紫
		static const uint32 DarkViolet			= 0xFF9400D3;	///< 暗紫罗兰
		static const uint32 DarkOrchid			= 0xFF9932CC;	///< 暗兰花紫
		static const uint32 Indigo				= 0xFF4B0082;	///< 靛青/紫兰色
		static const uint32 BlueViolet			= 0xFF8A2BE2;	///< 蓝紫罗兰
		static const uint32 MediumPurple		= 0xFF9370DB;	///< 中紫色
		static const uint32 MediumSlateBlue 	= 0xFF7B68EE;	///< 中暗蓝色/中板岩蓝
		static const uint32 SlateBlue			= 0xFF6A5ACD;	///< 石蓝色/板岩蓝
		static const uint32 DarkSlateBlue		= 0xFF483D8B;	///< 暗灰蓝色/暗板岩蓝
		static const uint32 Lavender			= 0xFFE6E6FA;	///< 淡紫色/熏衣草淡紫
		static const uint32 GhostWhite			= 0xFFF8F8FF;	///< 幽灵白
		static const uint32 Blue				= 0xFF0000FF;	///< 纯蓝
		static const uint32 MediumBlue			= 0xFF0000CD;	///< 中蓝色
		static const uint32 MidnightBlue		= 0xFF191970;	///< 午夜蓝
		static const uint32 DarkBlue			= 0xFF00008B;	///< 暗蓝色
		static const uint32 Navy				= 0xFF000080;	///< 海军蓝
		static const uint32 RoyalBlue			= 0xFF4169E1;	///< 皇家蓝/宝蓝
		static const uint32 CornflowerBlue		= 0xFF6495ED;	///< 矢车菊蓝
		static const uint32 LightSteelBlue		= 0xFFB0C4DE;	///< 亮钢蓝
		static const uint32 LightSlateGray		= 0xFF778899;	///< 亮蓝灰/亮石板灰
		static const uint32 SlateGray			= 0xFF708090;	///< 灰石色/石板灰
		static const uint32 DodgerBlue			= 0xFF1E90FF;	///< 闪兰色/道奇蓝
		static const uint32 AliceBlue			= 0xFFF0F8FF;	///< 爱丽丝蓝
		static const uint32 SteelBlue			= 0xFF4682B4;	///< 钢蓝/铁青
		static const uint32 LightSkyBlue		= 0xFF87CEFA;	///< 亮天蓝色
		static const uint32 SkyBlue				= 0xFF87CEEB;	///< 天蓝色
		static const uint32 DeepSkyBlue			= 0xFF00BFFF;	///< 深天蓝
		static const uint32 LightBlue			= 0xFFADD8E6;	///< 亮蓝
		static const uint32 PowderBlue			= 0xFFB0E0E6;	///< 粉蓝色/火药青
		static const uint32 CadetBlue			= 0xFF5F9EA0;	///< 军兰色/军服蓝
		static const uint32 Azure				= 0xFFF0FFFF;	///< 蔚蓝色
		static const uint32 LightCyan			= 0xFFE0FFFF;	///< 淡青色
		static const uint32 PaleTurquoise		= 0xFFAFEEEE;	///< 弱绿宝石
		static const uint32 Cyan				= 0xFF00FFFF;	///< 青色
		static const uint32 Aqua				= 0xFF00FFFF;	///< 浅绿色/水色
		static const uint32 DarkTurquoise		= 0xFF00CED1;	///< 暗绿宝石
		static const uint32 DarkSlateGray		= 0xFF2F4F4F;	///< 暗瓦灰色/暗石板灰
		static const uint32 DarkCyan			= 0xFF008B8B;	///< 暗青色
		static const uint32 Teal				= 0xFF008080;	///< 水鸭色
		static const uint32 MediumTurquoise 	= 0xFF48D1CC;	///< 中绿宝石
		static const uint32 LightSeaGreen		= 0xFF20B2AA;	///< 浅海洋绿
		static const uint32 Turquoise			= 0xFF40E0D0;	///< 绿宝石
		static const uint32 Aquamarine			= 0xFF7FFFD4;	///< 宝石碧绿
		static const uint32 MediumAquamarine 	= 0xFF66CDAA;	///< 中宝石碧绿
		static const uint32 MediumSpringGreen 	= 0xFF00FA9A;	///< 中春绿色
		static const uint32 MintCream			= 0xFFF5FFFA;	///< 薄荷奶油
		static const uint32 SpringGreen			= 0xFF00FF7F;	///< 春绿色
		static const uint32 MediumSeaGreen		= 0xFF3CB371;	///< 中海洋绿
		static const uint32 SeaGreen			= 0xFF2E8B57;	///< 海洋绿
		static const uint32 Honeydew			= 0xFFF0FFF0;	///< 蜜色/蜜瓜色
		static const uint32 LightGreen			= 0xFF90EE90;	///< 淡绿色
		static const uint32 PaleGreen			= 0xFF98FB98;	///< 弱绿色
		static const uint32 DarkSeaGreen		= 0xFF8FBC8F;	///< 暗海洋绿
		static const uint32 LimeGreen			= 0xFF32CD32;	///< 闪光深绿
		static const uint32 Lime				= 0xFF00FF00;	///< 闪光绿
		static const uint32 ForestGreen			= 0xFF228B22;	///< 森林绿
		static const uint32 Green				= 0xFF008000;	///< 纯绿
		static const uint32 DarkGreen			= 0xFF006400;	///< 暗绿色
		static const uint32 Chartreuse			= 0xFF7FFF00;	///< 黄绿色/查特酒绿
		static const uint32 LawnGreen			= 0xFF7CFC00;	///< 草绿色/草坪绿
		static const uint32 GreenYellow			= 0xFFADFF2F;	///< 绿黄色
		static const uint32 DarkOliveGreen		= 0xFF556B2F;	///< 暗橄榄绿
		static const uint32 YellowGreen			= 0xFF9ACD32;	///< 黄绿色
		static const uint32 OliveDrab			= 0xFF6B8E23;	///< 橄榄褐色
		static const uint32 Beige				= 0xFFF5F5DC;	///< 米色/灰棕色
		static const uint32 LightGoldenrodYellow= 0xFFFAFAD2;	///< 亮菊黄
		static const uint32 Ivory				= 0xFFFFFFF0;	///< 象牙色
		static const uint32 LightYellow			= 0xFFFFFFE0;	///< 浅黄色
		static const uint32 Yellow				= 0xFFFFFF00;	///< 纯黄
		static const uint32 Olive				= 0xFF808000;	///< 橄榄
		static const uint32 DarkKhaki			= 0xFFBDB76B;	///< 暗黄褐色/深卡叽布
		static const uint32 LemonChiffon		= 0xFFFFFACD;	///< 柠檬绸
		static const uint32 PaleGoldenrod		= 0xFFEEE8AA;	///< 灰菊黄/苍麒麟色
		static const uint32 Khaki				= 0xFFF0E68C;	///< 黄褐色/卡叽布
		static const uint32 Gold				= 0xFFFFD700;	///< 金色
		static const uint32 Cornsilk			= 0xFFFFF8DC;	///< 玉米丝色
		static const uint32 Goldenrod			= 0xFFDAA520;	///< 金菊黄
		static const uint32 DarkGoldenrod		= 0xFFB8860B;	///< 暗金菊黄
		static const uint32 FloralWhite			= 0xFFFFFAF0;	///< 花的白色
		static const uint32 OldLace				= 0xFFFDF5E6;	///< 老花色/旧蕾丝
		static const uint32 Wheat				= 0xFFF5DEB3;	///< 浅黄色/小麦色
		static const uint32 Moccasin			= 0xFFFFE4B5;	///< 鹿皮色/鹿皮靴
		static const uint32 Orange				= 0xFFFFA500;	///< 橙色
		static const uint32 PapayaWhip			= 0xFFFFEFD5;	///< 番木色/番木瓜
		static const uint32 BlanchedAlmond		= 0xFFFFEBCD;	///< 白杏色
		static const uint32 NavajoWhite			= 0xFFFFDEAD;	///< 纳瓦白/土著白
		static const uint32 AntiqueWhite		= 0xFFFAEBD7;	///< 古董白
		static const uint32 Tan					= 0xFFD2B48C;	///< 茶色
		static const uint32 BurlyWood			= 0xFFDEB887;	///< 硬木色
		static const uint32 Bisque				= 0xFFFFE4C4;	///< 陶坯黄
		static const uint32 DarkOrange			= 0xFFFF8C00;	///< 深橙色
		static const uint32 Linen				= 0xFFFAF0E6;	///< 亚麻布
		static const uint32 Peru				= 0xFFCD853F;	///< 秘鲁色
		static const uint32 PeachPuff			= 0xFFFFDAB9;	///< 桃肉色
		static const uint32 SandyBrown			= 0xFFF4A460;	///< 沙棕色
		static const uint32 Chocolate			= 0xFFD2691E;	///< 巧克力色
		static const uint32 SaddleBrown			= 0xFF8B4513;	///< 重褐色/马鞍棕色
		static const uint32 Seashell			= 0xFFFFF5EE;	///< 海贝壳
		static const uint32 Sienna				= 0xFFA0522D;	///< 黄土赭色
		static const uint32 LightSalmon			= 0xFFFFA07A;	///< 浅鲑鱼肉色
		static const uint32 Coral				= 0xFFFF7F50;	///< 珊瑚
		static const uint32 OrangeRed			= 0xFFFF4500;	///< 橙红色
		static const uint32 DarkSalmon			= 0xFFE9967A;	///< 深鲜肉/鲑鱼色
		static const uint32 Tomato				= 0xFFFF6347;	///< 番茄红
		static const uint32 MistyRose			= 0xFFFFE4E1;	///< 浅玫瑰色/薄雾玫瑰
		static const uint32 Salmon				= 0xFFFA8072;	///< 鲜肉/鲑鱼色
		static const uint32 Snow				= 0xFFFFFAFA;	///< 雪白色
		static const uint32 LightCoral			= 0xFFF08080;	///< 淡珊瑚色
		static const uint32 RosyBrown			= 0xFFBC8F8F;	///< 玫瑰棕色
		static const uint32 IndianRed			= 0xFFCD5C5C;	///< 印度红
		static const uint32 Red					= 0xFFFF0000;	///< 纯红
		static const uint32 Brown				= 0xFFA52A2A;	///< 棕色
		static const uint32 FireBrick			= 0xFFB22222;	///< 火砖色/耐火砖
		static const uint32 DarkRed				= 0xFF8B0000;	///< 深红色
		static const uint32 Maroon				= 0xFF800000;	///< 栗色
		static const uint32 White				= 0xFFFFFFFF;	///< 纯白
		static const uint32 WhiteSmoke			= 0xFFF5F5F5;	///< 白烟
		static const uint32 Gainsboro			= 0xFFDCDCDC;	///< 淡灰色
		static const uint32 LightGrey			= 0xFFD3D3D3;	///< 浅灰色
		static const uint32 Silver				= 0xFFC0C0C0;	///< 银灰色
		static const uint32 DarkGray			= 0xFFA9A9A9;	///< 深灰色
		static const uint32 Gray				= 0xFF808080;	///< 灰色
		static const uint32 DimGray				= 0xFF696969;	///< 暗淡灰
		static const uint32 Black				= 0xFF000000;	///< 纯黑
#ifdef _WIN32
#pragma endregion ConstantColors
#endif
	};



}// namespace sqr
