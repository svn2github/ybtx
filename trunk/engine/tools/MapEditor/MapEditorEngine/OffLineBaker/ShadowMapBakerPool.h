#pragma once

#include "TSingleton.h"

#include "../Graphic/CShadowMatrixBuilder.h"
#include "../Graphic/CShadowClipper.h"
#include "CRenderTexture.h"

namespace sqr
{
	SmartPointer(CRenderTexture);
	enum BUF_FLAG
	{
		FLAG_NONE,         //未用
		FLAG_USE,          //使用
		FLAG_UPDATE        //渲染
	};

	struct ShadowMapBuf
	{
		int                      TileIndex;        //地形块索引
		BUF_FLAG                 Flag;             //Buf状态
		CAxisAlignedBox			 Box;
		CRenderTexturePtr        ShadowTexture;    
		CShadowMatrixBuilderPtr  MatrixBuild;      
	};
	
	class CShadowMapBakingPool :public TSingleton<CShadowMapBakingPool>
	{
	public:
		CShadowMapBakingPool(void);
		~CShadowMapBakingPool(void);
		
		bool AddShadowBuf(int tileIndex,CAxisAlignedBox& box,BUF_FLAG flag=FLAG_UPDATE);
		
		bool DelShadowBuf(int tileIndex);
		
		void SetShadowMapBufFlag(int tileIndex,BUF_FLAG flag);
		void ResetShadowBufFlag(int tileIndex);
		void ResetShadowBufFlag();
		
		bool GetShadowBuf(int tileIndex,ShadowMapBuf& buf);
		
		void ClearShadowBuf();

		void SetBufNum(int num);
		int GetBufNum();
		
	private:
		bool FindShadowBuf(int tileIndex,int& bufID);

		vector<ShadowMapBuf> DepthBufList;
		int MaxNum;
	};

}
