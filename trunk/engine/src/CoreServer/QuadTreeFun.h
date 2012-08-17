#pragma once 

#include "CPos.h"
#include "CRectangle.h"


namespace sqr
{
	//判断点是否在quadnode范围里面
	/*
		因为对于处于边界的对象我们做了特殊处理，所以一个点如果处于一个节点中
		那么他的坐标一定在(left, right] (top, bottom]之间
	*/
	inline bool BeInQuadNodeBox(const CFPos& pos, const CFRect& rcBox)
	{
		
		if (pos.x > rcBox.left && pos.x <= rcBox.right &&
			pos.y < rcBox.top && pos.y >= rcBox.bottom)
		{
			return true;
		}
		
		
		return false;
	}

	//判断两个矩形是否完全重叠
	inline bool BeCoincide(const CFRect& rcRect, const CFRect& rcBox)
	{
		if (rcRect.left >= rcBox.left && rcRect.right <= rcBox.right &&
				rcRect.top <= rcBox.top && rcRect.bottom >= rcBox.bottom)
		{
			return true;
		}
		return false;
	}

	//判断矩形是否在quadnode范围里面
	inline bool BeInQuadNodeBox(const CFRect& rcRect, const CFRect& rcBox)
	{
		if (rcRect.left > rcBox.left && rcRect.right <= rcBox.right &&
			rcRect.top < rcBox.top && rcRect.bottom >= rcBox.bottom)
		{
			return true;
		}

		return false;
	}

	//判断矩形是否与quadnode范围重叠
	inline bool BeOverlappedQuadNode(const CFRect& rcRect, const CFRect& rcBox)
	{
		if (BeCoincide(rcRect, rcBox))
		{
			return true;
		}

		if (BeCoincide(rcBox, rcRect))
		{
			return true;
		}

		//下面情况表明两个矩形区域没有任何关系（在先前两个条件判断的情况下）
		if (rcRect.right <= rcBox.left || rcRect.left > rcBox.right ||
			rcRect.bottom >= rcBox.top || rcRect.top < rcBox.bottom)
		{
			return false;
		}

		return true;
	}

	//得到根四叉树区域
	CFRect GetQuadRect(uint32 uWidth, uint32 uHeight);


	//得到以pos为中心，fRadius为边半径的矩形区域
	inline void GetRadiusRect(CFRect& rect, const CFPos& pos, float fRadius, const CFRect& rcMaxBox)
	{
		float fLeft = pos.x - fRadius;
		rect.left = fLeft > rcMaxBox.left ? fLeft : rcMaxBox.left;

		float fRight = pos.x + fRadius;
		rect.right = fRight < rcMaxBox.right ? fRight : rcMaxBox.right;

		float fTop = pos.y + fRadius;
		rect.top = fTop < rcMaxBox.top ? fTop : rcMaxBox.top;

		float fBottom = pos.y - fRadius;
		rect.bottom = fBottom > rcMaxBox.bottom ? fBottom : rcMaxBox.bottom;

	}
}

