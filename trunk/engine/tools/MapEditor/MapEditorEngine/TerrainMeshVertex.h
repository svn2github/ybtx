#pragma once
#include "CVector3.h"

namespace sqr
{
	class CWireBox;

	class SVertex
	{
	public:
		SVertex(void);
		~SVertex(void);

		DWORD		dwVertexIndex;
		CVector3f	vPosition;
		CVector3f   vNormal;

		D3DCOLOR	oColor;				///original color 顶点原始颜色（包括刷的顶点色）
		D3DCOLOR	bColor;				///baking   color 烘培后的顶点色
		D3DCOLOR	sColor;				///specular color 顶点高光值
		D3DCOLOR	tColor;				///temp baking   color 烘培后的顶点色的临时存储值

		BYTE		usbUsedCnt;
		DWORD       dwGridsBelongto[4];
		float		fLogicHeight;
		CWireBox   *pWireBoxLogicHeight;
		bool		bHasCalculate;

		CWireBox   *GetWireBoxLogicHeight();
	};
}
