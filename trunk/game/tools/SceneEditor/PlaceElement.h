#pragma once
#include "stdafx.h"
#include "DataType.h"

enum ESelectMark
{
	eSM_NONE		= 0,
	eSM_SELECTED	= 1,
	eSM_FOCUS		= 1<<1,
};

enum EElementType//会用到搜索功能中指定不同的搜索内容, 进行按位赋值
{
	eET_OTHERS			=0,
	eET_NPC				= 1,
	eET_OBJ				= 1<<1,
	eET_TRAP			= 1<<2,
	eET_PATH_KEY_POS	= 1<<3,
	eET_ARROW			= 1<<4,
	eET_PATH_LINE		= 1<<5,

	eET_SINGLE_END,
	eET_ALL = ((eET_SINGLE_END - 1) << 1) - 1
};

class CPlaceNpc;
class CElement 
{
public:
	FPos			fPos;
	int			direction;
	CElement():eMark(eSM_NONE),eType(eET_OTHERS),direction(0), index(-1){}
	CElement(EElementType type):eMark(eSM_NONE),eType(type),direction(0), index(-1){}
	virtual ~CElement(){}
	void SetMark(ESelectMark mark)
	{
		if (mark == eSM_NONE)
		{
			eMark = eSM_NONE;
			return;
		}
		eMark |= mark;
	}
	void RemoveMark(ESelectMark mark){eMark &= ~mark;}
	bool IsMark(ESelectMark mark)
	{
		if (eSM_NONE == mark)
		{
			if (eMark == mark)
			{
				return true;
			}
			return false;
		}
		return (eMark & mark) == mark;
	}
	virtual void Draw(CDC* pDC){};
	virtual void SetPos(const FPos& fPos){this->fPos = fPos;};
	virtual void SetDir(int gameDir);
	int		eMark;
	const EElementType	eType;
	CString			name;
	int		index; //原始文件的序号, 为-1则为新加的,  为实现保存时候和之前的顺序一致,能被svn融合
};



typedef class CPathKeyPos : public CElement
{
public:
	CPathKeyPos():CElement(eET_PATH_KEY_POS),pOwner(NULL){}
	virtual void Draw(CDC* pDC);
	int				speed;
	int				delayTime;
	CPlaceNpc*		pOwner;
}PathKeyPos,*PPathKeyPos;



class CArrow :public CElement
{
public:
	CArrow():CElement(eET_ARROW)
	{
	}
	void SetOwner(CElement* pOwner);
	void Reset();//根据pOwnerNpc 重新设置属性
	virtual void SetDir(int gameDir);

	virtual void Draw(CDC* pDC);
	Pos arrowHeadPos, leftPos, rightPos;//像素偏移位置, 根据箭头根(npc屏蔽坐标分别 + 3个坐标得到箭头的3个点的屏幕位置)
	CElement* pOwner;
};


typedef class CPlaceObj: public CElement
{
public:
	CPlaceObj():CElement(eET_OBJ){Arrow.pOwner = static_cast<CElement*>(this);}
	virtual void Draw(CDC* pDC);
	virtual void SetPos(const FPos& fPos);
	virtual void SetDir(int gameDir);
	//CString			name;
	CArrow		Arrow;
}PlaceObj, *PPlaceObj;

typedef class CPlaceTrap : public CElement
{
public:
	CPlaceTrap():CElement(eET_TRAP){Arrow.pOwner = static_cast<CElement*>(this);}
	virtual ~CPlaceTrap(){};
	virtual void Draw(CDC* pDC);
	virtual void SetPos(const FPos& fPos);
	virtual void SetDir(int gameDir);
	//CString			name;
	CArrow		Arrow;
}PlaceTrap, *PPlaceTrap;


class CPathLine:public CElement
{
public:
	CPathLine():CElement(eET_PATH_LINE){}
	virtual void Draw(CDC* pDC);
	PPathKeyPos pFrontKeyPos;//前一关键点
	PPathKeyPos pBackKeyPos;//后一关键点
};

typedef class CPlaceNpc: public virtual CDynamicObject, public CElement
{
public:
	typedef std::list<PPathKeyPos>			Path_List;
	typedef Path_List::iterator				Path_Iter;
	
	CPlaceNpc():CElement(eET_NPC){Arrow.pOwner = static_cast<CElement*>(this);}
	~CPlaceNpc();
	
	static void CopyeNpc(OUT CPlaceNpc* pNpcDest, CPlaceNpc* pNpcSrc);//注意这里没有拷贝 pathList
	static bool CheckNpc(const CPlaceNpc& npc, OUT CString& errorMessage, bool isPut = true);
		
	static bool IsBoos(CPlaceNpc* pNpc);
	static bool IsBoos(CElement* pElement);
	static bool IsServant(CPlaceNpc* pNpc);
	static bool IsServant(CElement* pElement);

	bool Exp_AddPathKeyPos(float _x, float _y);
	void Exp_SetData(int _moveType, 
		int _moveRange, 
		int _direction,
		int _minDelayTime, 
		int _maxDelayTime,
		int _speed,
		int _idleType,
		int _colonyID,
		int _colonyType,
		int _isIgnoreClog);
	virtual void SetPos(const FPos& fPos);
	virtual void SetDir(int gameDir);

	PPathKeyPos InsetPathKeyPos(PPathKeyPos pBackKeyPos, const FPos& fPos); //pBackKeyPos后一个巡逻点, 如果为空则在结尾添加点
	PPathKeyPos FindNextKeyPos(PPathKeyPos pPreKeyPos);
	bool DeletePathKeyPos(PPathKeyPos pKeyPos);
	bool DeletePath();
	virtual void Draw(CDC* pDC);
	//CString		name;
	int			moveType;
	int			moveRange;
	int			minDelayTime;
	int			maxDelayTime;
	int			speed;
	uint32		colonyId;
	uint8		colonyType;
	uint8		idleType;
	CString		AIType;
	CString     type;
	Path_List	pathList;
	int			isIgnoreClog;
	CArrow		Arrow;
}PlaceNpc, *PPlaceNpc;

