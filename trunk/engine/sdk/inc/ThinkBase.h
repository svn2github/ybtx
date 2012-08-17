#pragma once
extern "C"
{
	#include "tg/thinkgear.h"
}




namespace sqr
{

	// 可以从TG获取的数据结构
	struct TGData
	{
		float TG_Battery;			
		float TG_PoorSignal;		
		float TG_Attention;			
		float TG_Meditation;		
		float TG_Raw;				
		float TG_Delta;			
		float TG_Theta;			
		float TG_Alpha1;			
		float TG_Alpha2;			
		float TG_Beta1;				
		float TG_Beta2;				
		float TG_Gamma1;		
		float TG_Gamma2;		
		void InitData()
		{
			TG_Battery = -1.0f;
			TG_PoorSignal = -1.0f;
			TG_Attention = -1.0f;
			TG_Meditation = -1.0f;
			TG_Raw = -1.0f;
			TG_Delta = -1.0f;	
			TG_Theta = -1.0f;
			TG_Alpha1 = -1.0f;
			TG_Alpha2 = -1.0f;
			TG_Beta1 = -1.0f;	
			TG_Beta2 = -1.0f;
			TG_Gamma1 = -1.0f;
			TG_Gamma2 = -1.0f;
		}
	};

}