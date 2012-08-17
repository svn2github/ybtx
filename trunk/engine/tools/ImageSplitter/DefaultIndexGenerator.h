#pragma once
#include "IIndexGenerator.h"

// ---- 类定义 --------------------------------------------------------------------------------
		
	/*
	
	生成规则:
	
	m = m_uWidth - 1 
	n = m_uHeiht - 1 
	
	M_0_n M_1_n M_2_n  . . .  M_m_n
 	.	 .	   .	  . . .  .
	.	 .	   .	  . . .  .
	.	 .	   .	  . . .  .
	M_0_2 M_1_2 M_2_2  . . .  M_m_2
	M_0_1 M_1_1 M_2_1  . . .  M_m_1
	M_0_0 M_1_0 M_2_0  . . .  M_m_0
	
	*/

	class CDefaultIndexGenerator : public IIndexGenerator
	{
		uint32			m_uWidth;
		uint32			m_uHeight;
		uint32			m_uIndexCount;
		uint32			m_uCurrIndex;

	public:
		CDefaultIndexGenerator();

		// ---- 传入区域宽度和高度，以此来决定切分索引数目
		virtual void	SetRaginInfo( uint32 uWidth, uint32 uHeight );

		// ---- 按照切割规则逐次返回切割坐标以及切分结果图片名称 
		//		当所有索引都已生成完毕，则返回false
		virtual bool	GetNextIndex( IndexStruct *sIndex );

		// ---- 释放生成器
		virtual void	Release();
	};