#pragma once
#include "CDynamicObject.h"
using namespace sqr;

typedef class tag_Npc : public virtual CDynamicObject
{
	public:
	uint32		uID;							// ID
	CString		_名字;					// 名字
	CString 		_类型;					// 怪物类型
	CString      _AI类型;
	CString      _AI数据;
	uint8			_最低等级;			// 最低等级
	uint8			_最高等级;			// 最高等级
	uint32		_最小攻击;
	uint32		_最大攻击;
	uint32		_防御;
	uint32		_血;
	float			_攻击速度;
	uint8			_属性伤害类型;
	uint32		_属性伤害值;
	uint32		_爆击;
	uint32		_命中;
	uint32		_格挡;
	uint32		_躲避;
	uint32		_冰抗;
	uint32		_火抗;
	uint32		_毒抗;
	uint32		_风抗;
	uint32		_致命一击;
	uint32		_经验;
	uint32		_移动类型;
	uint32		_移动范围;
	uint32		_索敌人类型;
	uint32		_视野范围;
	uint32		_索敌限制;
	uint32		_攻击模式;
	uint32		_技能编号;
	uint32		_释放间隔;
	uint32		_释放概率;
	uint32		_追击时间;
	uint32		_追击距离;
	uint32		_是否返回;
	uint32		_呼叫范围;
	uint32		_所属阵营;
	CString		ModelFilePath;
	CString		FxFilePath;
	CString		HurtFxFilePath;
	CString		HitDownFxFilePath;
	CString		SoundFilePath;
	uint32		RenderStyleID;
	uint32		GoldenRenderStyleID;
	CString		GoldenFxFilePath;
	//	uint8		_类型,
	uint8		_Npc是否唯一;
	uint8		_功能NPC类型;
	uint8		_战斗NPC类型;
	uint8		_NPC种族;
	uint8		_NPC阵营;
	uint8		_NPC职业;
	uint8		_NPC体积;
	uint8		_等级下限;
	uint8		_等级上限;
	uint8		_移动方式;
	//		uint32		_移动范围;
	uint32		_移动速度;
	uint32		_刷新时间;
	uint32		_警戒范围;
	uint32		_求救范围;

}NPC, *PNPC;

typedef struct tag_Obj
{
	CString name;
	uint8 isSingle;
	uint8 isBarrier;
}OBJ, *POBJ;

typedef struct tag_Trap
{
	CString name;
	uint8 isSingle;
}TRAP, *PTRAP;

typedef struct tag_SceneCfg
{
	tag_SceneCfg(CString sMap, CString sScene, CString sArea):mapFile(sMap),sceneFile(sScene),areaFile(sArea){}
	CString mapFile;
	CString sceneFile;
	CString areaFile;
}SceneCfg, *PSceneCfg;


typedef struct tag_AreaData
{
	tag_AreaData(const CString& strName):name(strName),isFb(false),color(-1),ratingPlayerNum(0),maxPlayerNum(0),music("无"){};
	CString name;
	bool	isFb;
	COLORREF color;
	uint32 ratingPlayerNum;	//额定人数
	uint32 maxPlayerNum;	//最大人数 为副本才有效
	CString music;
}AreaData, *PAreaData;



template <typename> struct T_Rect;
template <typename T> struct T_Pos;
template <typename T>
struct T_Pos 
{
	T_Pos(){}
	T_Pos(T _x, T _y):x(_x),y(_y){}
	T_Pos(CPoint pt):x(pt.x),y(pt.y){}
	inline bool IsInRect(const T_Rect<T>& rect) const
	{
		return rect.IsContainPos(*this);
	}
	CPoint ToCPoint()const
	{
		return CPoint(x,y);
	}
	inline T_Pos<T> operator +(const T_Pos<T> &  other)const
	{
		return T_Pos(x+ other.x, y + other.y);
	}
	inline T_Pos<T>& operator += (const T_Pos<T> &  other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	inline T_Pos<T> operator -(const T_Pos<T> &  other)const
	{
		return T_Pos(x- other.x, y - other.y);
	}
	inline T_Pos<T>& operator -= (const T_Pos<T> &  other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	inline T_Pos operator*(T value)const
	{
		return T_Pos(x * value ,y * value);
	}
	//点乘
	inline T operator*(const T_Pos<T> &  other)const
	{
		return x * other.x + y * other.y;
	}
	//叉乘
	inline T operator^(const T_Pos<T> &  other)const
	{
		return x * other.y - y * other.x;
	}

	T_Pos<T> Rotate(double radian, bool isClockwise)const
	{
		const double cosa = cos(radian);
		const double sina = sin(radian);
		if (isClockwise)
			return T_Pos<T>(static_cast<T>(x * cosa + y * sina), static_cast<T>(y * cosa - x * sina));
		else
			return T_Pos<T>(static_cast<T>(x * cosa - y * sina), static_cast<T>(y * cosa + x * sina));
	}

	inline T Length() const
	{
		return (T)sqrt(float(x*x + y*y));
	}
	T x;
	T y;
};

typedef T_Pos<int>   Pos;
typedef T_Pos<float> FPos;
typedef T_Pos<float> Vector2;

template <typename T>
struct T_Rect 
{
	union
	{
		struct  
		{
			T left, top, right, bottom;
		};
		struct  
		{
			T minX, minY, maxX, maxY;
		};
		struct  
		{
			T_Pos<T> left_top, right_bottom;
		};
	};
	T_Rect(){}
	T_Rect(T _left, T _top, T _right, T _bottom):left(_left),top(_top),right(_right),bottom(_bottom){}
	T_Rect(const T_Pos<T> _left_top, T_Pos<T> _right_bottom)
		:left_top(_left_top),right_bottom(_right_bottom){}
	
	inline bool IsContainPos(const T_Pos<T>& pos)const
	{
		return (pos.x >= left && pos.x <= right && pos.y >= top && pos.y <= bottom);
	}
	CRect ToCRect()
	{
		return CRect(left,top,right,bottom);
	}
	T_Rect<T> Correct()const
	{
		T_Rect<T> rect = *this;
		T temp;
		if (rect.left > rect.right)
		{
			temp = rect.left;
			rect.left = rect.right;
			rect.right = temp;
		}
		if (rect.top > rect.bottom)
		{
			temp = rect.top;
			rect.top = rect.bottom;
			rect.bottom = temp;
		}
		return rect;
	}
	void GetCenter(T_Pos<T>& fPosCenter)
	{
		fPosCenter.x = (left + right)/2;
		fPosCenter.y = (top + bottom)/2;
	}
	T_Pos<T> GetCenter()
	{
		T_Pos<T> center;
		GetCenter(center);
		return center;
	}
};

typedef T_Rect<int>   Rect;
typedef T_Rect<float> FRect;
