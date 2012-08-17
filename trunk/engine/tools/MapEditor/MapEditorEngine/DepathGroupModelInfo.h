#pragma once
#include "CRefObject.h"

namespace sqr
{
	SmartPointer(DepathModelInfo);
	class DepathModelInfo : public CRefObject
	{
	public:
		DepathModelInfo() 
			: nModelIndex(0)
			, nGridModelIndex(0)
			, nGroupModelIndex(0)
			, nOriginGridIndex(0)
		{
		}
		string modelName;
		string agpName;
		string skeName;
		string effectName;
		string effectFileName;
		vector<string> modelNames;
		string strItemName;

		float fRotationX;
		float fRotationY;
		float fRotationZ;
		float fScaleX;
		float fScaleY;
		float fScaleZ;
		float fX;
		float fZ;
		float fY;

		float r;
		float g;
		float b;

		bool bShadow;
		bool bSecondShadow;
		bool bTranslucent;
		bool bZTest;
		bool bSelfZTest;
		bool bRecieveShadow;
		bool bPlayAnimate;
		byte byRenderStyle;
		bool bVisibleByConfigure;
		///物件增加摄像机碰撞标记（有些物体不想使用消隐，而是想使用摄像机碰撞）
		bool bUseCameraCollision;

		int nModelIndex;///导客户端用于记录在ars中模型第几个索引信息属性
		size_t nGridModelIndex;//该图素在属于该格子（nOriginGridIndex）中的第几个modelgroup
		size_t nGroupModelIndex;///该图素在复合图素中是第几个位置
		int nOriginGridIndex;

		///给editor用的变量
		CVector3f vUserOffsetEx; //操作移动
		bool	  bBakingColor;
		int		  nMirrorType;
		///trans state 0 - auto 1 -  trans 2 - untrans
		BYTE	  bTransState;
		int		  nRatio;
	};
}